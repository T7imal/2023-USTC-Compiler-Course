#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"

#include <iostream>
#include <memory>

// 定义一个从常数值获取/创建 ConstantInt 类实例化的宏，方便多次调用
#define CONST_INT(num) \
    ConstantInt::get(num, module)

// 定义一个从常数值获取/创建 ConstantFP 类实例化的宏，方便多次调用
#define CONST_FP(num) \
    ConstantFP::get(num, module)

int main() {
    auto module = new Module();
    auto builder = new IRBuilder(nullptr, module);
    Type* Int32Type = module->get_int32_type();
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto retAlloca = builder->create_alloca(Int32Type);
    auto a = builder->create_alloca(Int32Type);
    auto i = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(10), a);
    builder->create_store(CONST_INT(0), i);
    auto while_cond = BasicBlock::create(module, "while.cond", mainFun);
    builder->create_br(while_cond);
    builder->set_insert_point(while_cond);
    auto iLoad = builder->create_load(i);
    auto cmp = builder->create_icmp_lt(iLoad, CONST_INT(10));
    auto while_body = BasicBlock::create(module, "while.body", mainFun);
    auto while_end = BasicBlock::create(module, "while.end", mainFun);
    builder->create_cond_br(cmp, while_body, while_end);
    builder->set_insert_point(while_body);
    iLoad = builder->create_load(i);
    auto add = builder->create_iadd(iLoad, CONST_INT(1));
    builder->create_store(add, i);
    iLoad = builder->create_load(i);
    auto aLoad = builder->create_load(a);
    add = builder->create_iadd(aLoad, iLoad);
    builder->create_store(add, a);
    builder->create_br(while_cond);
    builder->set_insert_point(while_end);
    aLoad = builder->create_load(a);
    builder->create_ret(aLoad);
    std::cout << module->print();
    delete module;
    return 0;
}