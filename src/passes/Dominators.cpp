#include "Dominators.hpp"

void Dominators::run() {
    for (auto& f1 : m_->get_functions()) {
        auto f = &f1;
        if (f->get_basic_blocks().size() == 0)
            continue;
        for (auto& bb1 : f->get_basic_blocks()) {
            auto bb = &bb1;
            allBBs.insert(bb);
        }
        for (auto& bb1 : f->get_basic_blocks()) {
            auto bb = &bb1;
            dom_.insert({ bb, allBBs });
            idom_.insert({ bb, {} });
            dom_frontier_.insert({ bb, {} });
            dom_tree_succ_blocks_.insert({ bb, {} });
        }

        create_dom(f);
        create_idom(f);
        create_dominance_frontier(f);
        create_dom_tree_succ(f);
    }
}

void Dominators::create_dom(Function* f) {
    bool changed = true;
    while (changed) {
        changed = false;
        // 反向后序遍历基本块
        for (auto it = allBBs.begin(); it != allBBs.end(); ++it) {
            auto bb = *it;
            BBSet new_set;
            BBSet intersection;
            for (auto pred : bb->get_pre_basic_blocks()) {
                if (intersection.size() == 0) {
                    intersection = dom_[pred];
                }
                else {
                    std::set_intersection(intersection.begin(), intersection.end(), dom_[pred].begin(), dom_[pred].end(), std::inserter(intersection, intersection.begin()));
                }
            }
            new_set = intersection;
            new_set.insert(bb);
            if (new_set != dom_[bb]) {
                dom_[bb] = new_set;
                changed = true;
            }
        }
    }
}

BasicBlock* Dominators::intersect(BasicBlock* b1, BasicBlock* b2) {
    BasicBlock* finger1 = b1;
    BasicBlock* finger2 = b2;
    while (finger1 != finger2) {
        if (dom_[finger1].size() == dom_[finger2].size()) {
            finger1 = idom_[finger1];
            finger2 = idom_[finger2];
        }
        if (dom_[finger1].size() > dom_[finger2].size()) {
            finger1 = idom_[finger1];
        }
        if (dom_[finger2].size() > dom_[finger1].size()) {
            finger2 = idom_[finger2];
        }
    }
    return finger1;
}

void Dominators::create_idom(Function* f) {
    // TODO 分析得到 f 中各个基本块的 idom
    idom_.at(f->get_entry_block()) = f->get_entry_block();
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it = allBBs.begin(); it != allBBs.end(); ++it) {
            auto bb1 = *it;
            if (bb1 == f->get_entry_block())
                continue;
            auto new_idom = *bb1->get_pre_basic_blocks().begin();
            for (auto pred : bb1->get_pre_basic_blocks()) {
                auto bb2 = &(*pred);
                if (bb2 == new_idom)
                    continue;
                if (idom_.at(bb2) != nullptr) {
                    new_idom = intersect(bb2, new_idom);
                }
            }
            if (idom_.at(bb1) != new_idom) {
                idom_.at(bb1) = new_idom;
                changed = true;
            }
        }
    }
}

void Dominators::create_dominance_frontier(Function* f) {
    // TODO 分析得到 f 中各个基本块的支配边界集合
    for (auto& bb1 : f->get_basic_blocks()) {
        auto bb = &bb1;
        if (bb->get_pre_basic_blocks().size() >= 2) {
            for (auto pred : bb->get_pre_basic_blocks()) {
                auto bb2 = &(*pred);
                while (bb2 != idom_.at(bb)) {
                    dom_frontier_.at(bb2).insert(bb);
                    bb2 = idom_.at(bb2);
                }
            }
        }
    }
}

void Dominators::create_dom_tree_succ(Function* f) {
    for (auto& bb1 : f->get_basic_blocks()) {
        auto bb = &bb1;
        if (idom_[bb] != nullptr) {
            dom_tree_succ_blocks_[idom_[bb]].insert(bb);
        }
    }
}

// void Dominators::create_dom_tree_succ(Function* f) {
//     for (auto& bb1 : f->get_basic_blocks()) {
//         // dom_tree_succ_blocks_[&bb1] = bb1.get_succ_basic_blocks();
//         auto bb = &bb1;
//         for (auto& bb2 : bb->get_succ_basic_blocks()) {
//             dom_tree_succ_blocks_[bb].insert(bb2);
//         }
//     }
// }