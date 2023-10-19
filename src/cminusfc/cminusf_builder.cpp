#include "cminusf_builder.hpp"

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type* VOID_T;
Type* INT1_T;
Type* INT32_T;
Type* INT32PTR_T;
Type* FLOAT_T;
Type* FLOATPTR_T;

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */


Value* CminusfBuilder::visit(ASTProgram& node) {
    VOID_T = module->get_void_type();
    INT1_T = module->get_int1_type();
    INT32_T = module->get_int32_type();
    INT32PTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();

    Value* ret_val = nullptr;
    for (auto& decl : node.declarations) {
        ret_val = decl->accept(*this);
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTNum& node) {
    // TODO: This function is empty now.
    // Add some code here.
    if (node.type == TYPE_INT)
        return CONST_INT(node.i_val);
    else if (node.type == TYPE_FLOAT)
        return CONST_FP(node.f_val);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTVarDeclaration& node) {
    // TODO: This function is empty now.
    // Add some code here.
    Type* varType;
    if (node.type == TYPE_INT) {
        varType = INT32_T;

    }
    else if (node.type == TYPE_FLOAT) {
        varType = FLOAT_T;
    }
    if (scope.in_global()) {
        if (node.num == nullptr) {
            GlobalVariable* globalVar;
            if (varType == INT32_T)
                globalVar = GlobalVariable::create(node.id, module.get(), varType, false, CONST_INT(0));
            else
                globalVar = GlobalVariable::create(node.id, module.get(), varType, false, CONST_FP(0.));
            scope.push(node.id, globalVar);
        }
        else {
            auto arrayType = ArrayType::get(varType, node.num->i_val);
            auto init = ConstantZero::get(arrayType, module.get());
            auto globalVar = GlobalVariable::create(node.id, module.get(), arrayType, false, init);
            scope.push(node.id, globalVar);
        }
    }
    else {
        if (node.num == nullptr) {
            auto alloca = builder->create_alloca(varType);
            scope.push(node.id, alloca);
        }
        else {
            auto arrayType = ArrayType::get(varType, node.num->i_val);
            auto alloca = builder->create_alloca(arrayType);
            scope.push(node.id, alloca);
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTFunDeclaration& node) {
    FunctionType* fun_type;
    Type* ret_type;
    std::vector<Type*> param_types;
    if (node.type == TYPE_INT)
        ret_type = INT32_T;
    else if (node.type == TYPE_FLOAT)
        ret_type = FLOAT_T;
    else
        ret_type = VOID_T;

    for (auto& param : node.params) {
        // TODO: Please accomplish param_types.
        if (param->type == TYPE_INT)
            if (param->isarray)
                param_types.push_back(INT32PTR_T);
            else
                param_types.push_back(INT32_T);
        else if (param->type == TYPE_FLOAT)
            if (param->isarray)
                param_types.push_back(FLOATPTR_T);
            else
                param_types.push_back(FLOAT_T);
    }

    fun_type = FunctionType::get(ret_type, param_types);
    auto func = Function::create(fun_type, node.id, module.get());
    scope.push(node.id, func);
    context.func = func;
    auto funBB = BasicBlock::create(module.get(), "entry", func);
    builder->set_insert_point(funBB);
    scope.enter();
    std::vector<Value*> args;
    for (auto& arg : func->get_args()) {
        args.push_back(&arg);
    }
    for (int i = 0; i < node.params.size(); ++i) {
        // TODO: You need to deal with params and store them in the scope.
        context.argVar = args[i];
        node.params[i]->accept(*this);
    }
    node.compound_stmt->accept(*this);
    if (not builder->get_insert_block()->is_terminated()) {
        if (context.func->get_return_type()->is_void_type())
            builder->create_void_ret();
        else if (context.func->get_return_type()->is_float_type())
            builder->create_ret(CONST_FP(0.));
        else
            builder->create_ret(CONST_INT(0));
    }
    scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTParam& node) {
    // TODO: This function is empty now.
    // Add some code here.
    Type* paramType;
    if (node.isarray) {
        if (node.type == TYPE_INT)
            paramType = INT32PTR_T;
        else if (node.type == TYPE_FLOAT)
            paramType = FLOATPTR_T;
    }
    else {
        if (node.type == TYPE_INT)
            paramType = INT32_T;
        else if (node.type == TYPE_FLOAT)
            paramType = FLOAT_T;
    }
    auto alloca = builder->create_alloca(paramType);
    scope.push(node.id, alloca);
    auto param = scope.find(node.id);
    builder->create_store(context.argVar, param);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTCompoundStmt& node) {
    // TODO: This function is not complete.
    // You may need to add some code here
    // to deal with complex statements.
    scope.enter();
    for (auto& decl : node.local_declarations) {
        decl->accept(*this);
    }

    for (auto& stmt : node.statement_list) {
        stmt->accept(*this);
        if (builder->get_insert_block()->is_terminated())
            break;
    }
    scope.exit();
    return nullptr;
}

Value* CminusfBuilder::visit(ASTExpressionStmt& node) {
    // TODO: This function is empty now.
    // Add some code here.
    if (node.expression != nullptr)
        node.expression->accept(*this);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTSelectionStmt& node) {
    // TODO: This function is empty now.
    // Add some code here.

    node.expression->accept(*this);
    auto cond = context.cmpResult;
    auto trueBB = BasicBlock::create(module.get(), "", context.func);
    auto falseBB = BasicBlock::create(module.get(), "", context.func);
    auto endBB = BasicBlock::create(module.get(), "", context.func);
    builder->create_cond_br(cond, trueBB, falseBB);
    builder->set_insert_point(trueBB);
    node.if_statement->accept(*this);
    builder->create_br(endBB);
    builder->set_insert_point(falseBB);
    if (node.else_statement != nullptr)
        node.else_statement->accept(*this);
    builder->create_br(endBB);
    builder->set_insert_point(endBB);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTIterationStmt& node) {
    // TODO: This function is empty now.
    // Add some code here.
    auto condBB = BasicBlock::create(module.get(), "", context.func);
    auto bodyBB = BasicBlock::create(module.get(), "", context.func);
    auto endBB = BasicBlock::create(module.get(), "", context.func);
    builder->create_br(condBB);
    builder->set_insert_point(condBB);
    node.expression->accept(*this);
    auto cond = context.cmpResult;
    builder->create_cond_br(cond, bodyBB, endBB);
    builder->set_insert_point(bodyBB);
    node.statement->accept(*this);
    builder->create_br(condBB);
    builder->set_insert_point(endBB);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTReturnStmt& node) {
    if (node.expression == nullptr) {
        builder->create_void_ret();
        return nullptr;
    }
    else {
        // TODO: The given code is incomplete.
        // You need to solve other return cases (e.g. return an integer).
        auto ret_val = node.expression->accept(*this);
        builder->create_ret(ret_val);
        return ret_val;
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTVar& node) {
    // TODO: This function is empty now.
    // Add some code here.
    if (node.expression == nullptr) {
        auto var = scope.find(node.id);
        if (var->get_type()->is_pointer_type()) {
            if (var->get_type()->get_pointer_element_type()->is_array_type())
                var = builder->create_gep(var, { CONST_INT(0), CONST_INT(0) });
            else
                var = builder->create_gep(var, { CONST_INT(0) });
        }
        context.varLocation = var;
        return builder->create_load(var);
    }
    else {
        auto var = scope.find(node.id);
        auto idx = node.expression->accept(*this);
        if (idx->get_type()->is_float_type())
            idx = builder->create_fptosi(idx, INT32_T);
        auto posBB = BasicBlock::create(module.get(), "", context.func);
        auto negBB = BasicBlock::create(module.get(), "", context.func);
        auto endBB = BasicBlock::create(module.get(), "", context.func);
        auto cond = builder->create_icmp_ge(idx, CONST_INT(0));
        builder->create_cond_br(cond, posBB, negBB);
        builder->set_insert_point(posBB);
        if (var->get_type()->get_pointer_element_type()->is_integer_type()
            || var->get_type()->get_pointer_element_type()->is_float_type()) {
            var = builder->create_gep(var, { idx });
        }
        else if (var->get_type()->get_pointer_element_type()->is_array_type()) {
            var = builder->create_gep(var, { CONST_INT(0), idx });
        }
        else {
            var = builder->create_load(var);
            var = builder->create_gep(var, { idx });
        }
        builder->create_br(endBB);
        builder->set_insert_point(negBB);
        builder->create_call(scope.find("neg_idx_except"), {});
        builder->create_br(endBB);
        builder->set_insert_point(endBB);
        context.varLocation = var;
        return builder->create_load(var);
    }
}

Value* CminusfBuilder::visit(ASTAssignExpression& node) {
    // TODO: This function is empty now.
    // Add some code here.
    auto var = node.var->accept(*this);
    auto varLocation = context.varLocation;
    auto val = node.expression->accept(*this);
    if (var->get_type() != val->get_type()) {
        if (var->get_type()->is_integer_type())
            val = builder->create_fptosi(val, INT32_T);
        else
            val = builder->create_sitofp(val, FLOAT_T);
    }
    builder->create_store(val, varLocation);

    return val;
}

Value* CminusfBuilder::visit(ASTSimpleExpression& node) {
    // TODO: This function is empty now.
    // Add some code here.
    if (node.additive_expression_r == nullptr) {
        auto val = node.additive_expression_l->accept(*this);
        if (val->get_type()->is_int1_type())
            context.cmpResult = val;
        if (val->get_type()->is_int32_type())
            context.cmpResult = builder->create_icmp_ne(val, CONST_INT(0));
        if (val->get_type()->is_float_type())
            context.cmpResult = builder->create_fcmp_ne(val, CONST_FP(0.));
        return val;
    }
    else {
        auto val1 = node.additive_expression_l->accept(*this);
        auto val2 = node.additive_expression_r->accept(*this);
        if (val1->get_type()->is_int32_type() && val2->get_type()->is_int32_type()) {
            switch (node.op) {
            case OP_LT:
                context.cmpResult = builder->create_icmp_lt(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_LE:
                context.cmpResult = builder->create_icmp_le(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_GT:
                context.cmpResult = builder->create_icmp_gt(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_GE:
                context.cmpResult = builder->create_icmp_ge(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_EQ:
                context.cmpResult = builder->create_icmp_eq(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_NEQ:
                context.cmpResult = builder->create_icmp_ne(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            }
        }
        else {
            if (val1->get_type()->is_int32_type())
                val1 = builder->create_sitofp(val1, FLOAT_T);
            if (val2->get_type()->is_int32_type())
                val2 = builder->create_sitofp(val2, FLOAT_T);
            switch (node.op) {
            case OP_LT:
                context.cmpResult = builder->create_fcmp_lt(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_LE:
                context.cmpResult = builder->create_fcmp_le(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_GT:
                context.cmpResult = builder->create_fcmp_gt(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_GE:
                context.cmpResult = builder->create_fcmp_ge(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_EQ:
                context.cmpResult = builder->create_fcmp_eq(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            case OP_NEQ:
                context.cmpResult = builder->create_fcmp_ne(val1, val2);
                return builder->create_zext(context.cmpResult, INT32_T);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTAdditiveExpression& node) {
    // TODO: This function is empty now.
    // Add some code here.
    if (node.additive_expression == nullptr) {
        auto val = node.term->accept(*this);
        return val;
    }
    else {
        auto val1 = node.additive_expression->accept(*this);
        auto val2 = node.term->accept(*this);
        if (val1->get_type()->is_int32_type() && val2->get_type()->is_int32_type()) {
            switch (node.op) {
            case OP_PLUS:
                return builder->create_iadd(val1, val2);
            case OP_MINUS:
                return builder->create_isub(val1, val2);
            }
        }
        else {
            if (val1->get_type()->is_int32_type())
                val1 = builder->create_sitofp(val1, FLOAT_T);
            if (val2->get_type()->is_int32_type())
                val2 = builder->create_sitofp(val2, FLOAT_T);
            switch (node.op) {
            case OP_PLUS:
                return builder->create_fadd(val1, val2);
            case OP_MINUS:
                return builder->create_fsub(val1, val2);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTTerm& node) {
    // TODO: This function is empty now.
    // Add some code here.
    if (node.term == nullptr) {
        auto val = node.factor->accept(*this);
        return val;
    }
    else {
        auto val1 = node.term->accept(*this);
        auto val2 = node.factor->accept(*this);
        if (val1->get_type()->is_int32_type() && val2->get_type()->is_int32_type()) {
            switch (node.op) {
            case OP_MUL:
                return builder->create_imul(val1, val2);
            case OP_DIV:
                return builder->create_isdiv(val1, val2);
            }
        }
        else {
            if (val1->get_type()->is_int32_type())
                val1 = builder->create_sitofp(val1, FLOAT_T);
            if (val2->get_type()->is_int32_type())
                val2 = builder->create_sitofp(val2, FLOAT_T);
            switch (node.op) {
            case OP_MUL:
                return builder->create_fmul(val1, val2);
            case OP_DIV:
                return builder->create_fdiv(val1, val2);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTCall& node) {
    // TODO: This function is empty now.
    // Add some code here.
    auto func = scope.find(node.id);
    auto funcType = static_cast<FunctionType*>(func->get_type());
    std::vector<Value*> args;
    int i = 0;
    for (auto arg : node.args) {
        auto var = arg->accept(*this);
        if (var->get_type()->is_pointer_type()) {
            args.push_back(context.varLocation);
        }
        else {
            // 参数类型不匹配，需要转换
            if (funcType->get_param_type(i)->is_pointer_type()) {
                args.push_back(context.varLocation);
            }
            else {
                if (funcType->get_param_type(i) != var->get_type())
                    if (funcType->get_param_type(i)->is_integer_type())
                        var = builder->create_fptosi(var, INT32_T);
                    else
                        var = builder->create_sitofp(var, FLOAT_T);
                args.push_back(var);
            }
        }
        i = i + 1;
    }
    return builder->create_call(static_cast<Function*>(func), args);
}
