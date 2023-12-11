#include "Mem2Reg.hpp"
#include "IRBuilder.hpp"
#include "Value.hpp"

#include <memory>

using BBSet = std::set<BasicBlock*>;

void Mem2Reg::run() {
    // 创建支配树分析 Pass 的实例
    dominators_ = std::make_unique<Dominators>(m_);
    // 建立支配树
    dominators_->run();
    // 以函数为单元遍历实现 Mem2Reg 算法
    for (auto& f : m_->get_functions()) {
        if (f.is_declaration())
            continue;
        func_ = &f;
        if (func_->get_basic_blocks().size() >= 1) {
            rename_stack_.clear();
            phi_values_.clear();
            gep_values_.clear();
            is_dead_.clear();
            // 对应伪代码中 phi 指令插入的阶段
            generate_phi();
            // 为每个内存变量建立一个栈，用于存放变量的最新定值
            // for (auto& bb : func_->get_basic_blocks()) {
            //     for (auto& inst : bb.get_instructions()) {
            //         if (inst.is_alloca() || inst.is_load() || inst.is_store()) {
            //             auto lval = static_cast<Value*>(&inst);
            //             rename_stack_[lval] = std::stack<Value*>();
            //         }
            //     }
            // }
            // 对应伪代码中重命名阶段
            rename(func_->get_entry_block());
        }
        // 后续 DeadCode 将移除冗余的局部变量的分配空间
    }
}

void Mem2Reg::generate_phi() {
    // TODO
    // 步骤一：找到活跃在多个 block 的全局名字集合，以及它们所属的 bb 块
    // 步骤二：从支配树获取支配边界信息，并在对应位置插入 phi 指令
    std::map<Value*, BBSet> globals;
    for (auto& bb : func_->get_basic_blocks()) {
        for (auto& inst : bb.get_instructions()) {
            if (inst.is_call())
                continue;
            auto lval = static_cast<Value*>(&inst);
            if (is_valid_ptr(lval)) {
                globals[lval].insert(&bb);
                for (auto& op : inst.get_operands()) {
                    auto def = dynamic_cast<Instruction*>(op);
                    // 排除常数
                    if (def != nullptr) {
                        globals[op].insert(&bb);
                    }
                }
            }
            // 记录 gep 指令
            else if (is_gep_instr(lval)) {
                auto gep = static_cast<GetElementPtrInst*>(&inst);
                auto op_num = gep->get_num_operand();
                if (gep_values_.size() == 0) {
                    for (int i = 0; i < op_num; i++) {
                        gep_values_[gep].push_back(gep->get_operand(i));
                    }
                }
                bool exist = false;
                for (auto it = gep_values_.begin(); it != gep_values_.end(); it++) {
                    if (it->first != gep) {
                        if (it->second.size() == op_num) {
                            bool flag = true;
                            for (int i = 0; i < op_num; i++) {
                                if (it->second[i] != gep->get_operand(i)) {
                                    flag = false;
                                    break;
                                }
                            }
                            if (flag) {
                                exist = true;
                                break;
                            }
                        }
                    }
                    else {
                        exist = true;
                        continue;
                    }
                }
                if (!exist) {
                    for (int i = 0; i < op_num; i++) {
                        gep_values_[gep].push_back(gep->get_operand(i));
                    }
                }
            }
        }
    }
    for (auto& [val, bbs] : globals) {
        if (bbs.size() > 1) {
            std::set<BasicBlock*>F; //已经插入关于变量 v 的 phi 函数的基本块集合
            std::set<BasicBlock*>W; //所有定义了变量 v 的基本块集合
            for (auto bb : bbs) {
                W.insert(bb);
            }
            while (!W.empty()) {
                auto bb = *W.begin();
                W.erase(bb);
                for (auto df : dominators_->get_dominance_frontier(bb)) {
                    if (F.find(df) == F.end()) {
                        if (val->get_type()->is_pointer_type()) {
                            auto phi = PhiInst::create_phi(val->get_type()->get_pointer_element_type(), df, {  }, {  });
                            phi_values_[phi] = val; // 记录 phi 指令对应的变量
                            df->add_instr_begin(phi);
                            F.insert(df);
                            if (W.find(df) == W.end()) {
                                W.insert(df);
                            }
                        }
                        else {
                            auto phi = PhiInst::create_phi(val->get_type(), df, {  }, {  });
                            phi_values_[phi] = val; // 记录 phi 指令对应的变量
                            df->add_instr_begin(phi);
                            F.insert(df);
                            if (W.find(df) == W.end()) {
                                W.insert(df);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Mem2Reg::rename(BasicBlock* bb) {
    // TODO
    // 步骤三：将 phi 指令作为 lval 的最新定值，lval 即是为局部变量 alloca 出的地址空间
    // 步骤四：用 lval 最新的定值替代对应的load指令
    // 步骤五：将 store 指令的 rval，也即被存入内存的值，作为 lval 的最新定值
    // 步骤六：为 lval 对应的 phi 指令参数补充完整
    // 步骤七：对 bb 在支配树上的所有后继节点，递归执行 re_name 操作
    // 步骤八：pop出 lval 的最新定值
    // 步骤九：清除冗余的指令
    for (auto& inst : bb->get_instructions()) {
        if (inst.is_phi()) {
            auto phi = static_cast<PhiInst*>(&inst);
            auto rval = phi_values_[phi];
            rename_stack_[rval].push(phi);
        }
        else if (inst.is_load()) {
            auto load = static_cast<LoadInst*>(&inst);
            auto lval = static_cast<Value*>(load);
            auto rval = load->get_lval();
            if (rename_stack_[rval].empty())
                continue;
            auto replace_val = rename_stack_[rval].top();
            if (is_valid_ptr(rval)) {
                is_dead_[rval] = true;  // 定义被使用，标记为冗余
            }
            else {
                is_dead_[rval] = false; // 可能是内存操作，标记为非冗余
            }
            auto inst_after = std::next(load->getIterator());
            // 替换 load 指令后面的所有指令中的操作数
            // 本基本块
            while (inst_after != bb->get_instructions().end()) {
                auto inst = &(*inst_after);
                for (int i = 0; i < inst->get_num_operand(); i++) {
                    auto op = inst->get_operand(i);
                    if (op == lval) {
                        inst->set_operand(i, replace_val);
                    }
                }
                inst_after++;
            }
            // 后续基本块
            BBSet succs = dominators_->get_dom_tree_succ_blocks(bb);
            while (!succs.empty()) {
                auto succ = *succs.begin();
                succs.erase(succ);
                succs.insert(dominators_->get_dom_tree_succ_blocks(succ).begin(), dominators_->get_dom_tree_succ_blocks(succ).end());
                for (auto inst = succ->get_instructions().begin(); inst != succ->get_instructions().end(); inst++) {
                    for (int i = 0; i < inst->get_num_operand(); i++) {
                        auto op = inst->get_operand(i);
                        if (op == lval) {
                            inst->set_operand(i, replace_val);
                        }
                    }
                }
            }
        }
        else if (inst.is_store()) {
            auto store = static_cast<StoreInst*>(&inst);
            auto lval = store->get_lval();
            auto rval = store->get_rval();
            rename_stack_[lval].push(rval);
            if (is_valid_ptr(lval)) {
                is_dead_[lval] = true;  // 定义被使用，标记为冗余
            }
            else {
                is_dead_[lval] = false; // 可能是内存操作，标记为非冗余
            }
        }
        else if (inst.is_gep()) {
            auto gep = static_cast<GetElementPtrInst*>(&inst);
            auto lval = static_cast<Value*>(gep);
            int op_num = gep->get_num_operand();
            for (auto it = gep_values_.begin(); it != gep_values_.end(); it++) {
                if (it->first != gep) {
                    if (it->second.size() == op_num) {
                        bool flag = true;
                        for (int i = 0; i < op_num; i++) {
                            if (it->second[i] != gep->get_operand(i)) {
                                flag = false;
                                break;
                            }
                        }
                        if (flag) {
                            auto replace_val = it->first;
                            auto inst_after = std::next(gep->getIterator());
                            is_dead_[replace_val] = false;  // 定义被使用，标记为非冗余
                            // 替换 gep 指令后面的所有指令中的操作数
                            // 本基本块
                            while (inst_after != bb->get_instructions().end()) {
                                auto inst = &(*inst_after);
                                for (int i = 0; i < inst->get_num_operand(); i++) {
                                    auto op = inst->get_operand(i);
                                    if (op == lval) {
                                        inst->set_operand(i, replace_val);
                                    }
                                }
                                inst_after++;
                            }
                            // 后续基本块
                            BBSet succs = dominators_->get_dom_tree_succ_blocks(bb);
                            while (!succs.empty()) {
                                auto succ = *succs.begin();
                                succs.erase(succ);
                                succs.insert(dominators_->get_dom_tree_succ_blocks(succ).begin(), dominators_->get_dom_tree_succ_blocks(succ).end());
                                for (auto inst = succ->get_instructions().begin(); inst != succ->get_instructions().end(); inst++) {
                                    for (int i = 0; i < inst->get_num_operand(); i++) {
                                        auto op = inst->get_operand(i);
                                        if (op == lval) {
                                            inst->set_operand(i, replace_val);
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                else {
                    continue;
                }
            }
        }
    }
    // 填充 phi 指令的参数
    // 使用基本块后继，而不是支配树后继
    for (auto& bb_succ : bb->get_succ_basic_blocks()) {
        for (auto& inst : bb_succ->get_instructions()) {
            if (inst.is_phi()) {
                auto phi = static_cast<PhiInst*>(&inst);
                auto rval = phi_values_[phi];
                if (!rename_stack_[rval].empty()) {
                    phi->add_phi_pair_operand(rename_stack_[rval].top(), bb);
                }
            }
        }
    }


    // 递归遍历基本块
    for (auto& bb_succ : dominators_->get_dom_tree_succ_blocks(bb)) {
        rename(bb_succ);
    }

    // 恢复栈空间
    for (auto& inst : bb->get_instructions()) {
        if (inst.is_phi()) {
            auto phi = static_cast<PhiInst*>(&inst);
            auto rval = phi_values_[phi];
            rename_stack_[rval].pop();
        }
        else if (inst.is_store()) {
            auto store = static_cast<StoreInst*>(&inst);
            auto lval = store->get_lval();
            rename_stack_[lval].pop();
        }
    }
    // 清除冗余指令
    bool flag = true;
    while (flag) {
        flag = false;
        for (auto& inst : bb->get_instructions()) {
            if (inst.is_store()) {
                auto store = static_cast<StoreInst*>(&inst);
                auto lval = store->get_lval();
                if (is_dead_[lval]) {
                    flag = true;
                    inst.remove_all_operands();
                    inst.get_parent()->get_instructions().erase(inst);
                    break;
                }
            }
        }
    }
}
