#include "../include/codegen.hpp"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <system_error>

void Zar::CodeGen::_make_module(std::string name){
    auto module = std::make_unique<Module>(name,context);
    _current_module = std::move(module);
}

void Zar::CodeGen::_pushScope(){
    std::unordered_map<std::string,CG_Symbol> scope;
    symtabstack.push_back(scope);
}

void Zar::CodeGen::_popScope(){
    if(!symtabstack.empty()){
        symtabstack.pop_back();
    }
    else{
        std::cerr << "popScope on empty Symbol Table stack" << std::endl;
        std::exit(1);
    }
}

void Zar::CodeGen::_addSymbol(std::string name,CG_Symbol symbol){
    auto& cscope = symtabstack.back();
    if(cscope.find(name)!=cscope.end()){
        std::cerr << "Redeclaration of Symbol in the same scope" << std::endl;
        std::exit(1);
    }
    else{
        cscope.emplace(name,symbol);
    }
}

Zar::CodeGen::CG_Symbol Zar::CodeGen::_lookup(std::string name){
    CG_Symbol Symbol(nullptr);
    for(int it = symtabstack.size()-1;it>=0;it--){
        if(symtabstack.at(it).find(name)!=symtabstack.at(it).end()){
            Symbol = symtabstack.at(it).at(name);
            break;
        }
    }
    return Symbol;
}

Value* Zar::CodeGen::_getRValue(Value* v,Type* t){
    if(v->getType()->isPointerTy())
        return builder.CreateLoad(t,v,"l_"+v->getName());
    return v;
}

Value* Zar::CodeGen::_genAdd(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateAdd(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genSub(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateSub(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genMul(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateMul(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genDiv(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateSDiv(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genEq(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateICmpEQ(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genNEq(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateICmpNE(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genGT(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateICmpUGT(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genLT(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateICmpULT(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genGEq(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateICmpUGE(lhs,rhs,rn);
}
Value* Zar::CodeGen::_genLEq(Value* lhs,Value* rhs,std::string rn){
    return builder.CreateICmpULE(lhs,rhs,rn);
}


void Zar::CodeGen::visit(const literalNode* node){
    if(node->type==ExprType::EXPR_IDEN_LITRL){
        auto sym = _lookup(node->value);
        _lastVal = _getRValue(sym.val,sym.typ);
    }
    else{
        _lastVal = ConstantInt::get(builder.getInt32Ty(),std::stoi(node->value));
    }
}

void Zar::CodeGen::visit(const binexprNode* node){
    node->left->accept(*this);
    Value* lhs = _lastVal;
    node->right->accept(*this);
    Value* rhs = _lastVal;

    switch(node->Op){
        case Zar::BinaryOp::ADD:
            _lastVal = _genAdd(lhs,rhs,"addtemp");
            break;
        case Zar::BinaryOp::SUB:
            _lastVal = _genSub(lhs,rhs,"subtemp");
            break;
        case Zar::BinaryOp::MULT:
            _lastVal = _genMul(lhs,rhs,"multemp");
            break;
        case Zar::BinaryOp::DIV:
            _lastVal = _genDiv(lhs,rhs,"divtemp");
            break;
        case Zar::BinaryOp::EQ:
            _lastVal = _genEq(lhs,rhs,"eqtemp");
            break;
        case Zar::BinaryOp::NEQ:
            _lastVal = _genNEq(lhs,rhs,"neqtemp");
            break;
        case Zar::BinaryOp::GT:
            _lastVal = _genGT(lhs,rhs,"gttemp");
            break;
        case Zar::BinaryOp::LT:
            _lastVal = _genLT(lhs,rhs,"lttemp");
            break;
        case Zar::BinaryOp::GTE:
            _lastVal = _genGEq(lhs,rhs,"geqtemp");
            break;
        case Zar::BinaryOp::LTE:
            _lastVal = _genLEq(lhs,rhs,"leqtemp");
            break;
        default:
            std::cout << "Unknown Node type encountered" << std::endl;
            exit(1);
    }
}

void Zar::CodeGen::visit(const Block* node){
    _pushScope();
    for(int i = 0;i<node->block.size();i++){
        if(node->block.at(i)){node->block.at(i)->accept(*this);}
        else{continue;}
    }
    _popScope();
}

void Zar::CodeGen::visit(const ExprStmt* node){
    node->expr->accept(*this);
}

void Zar::CodeGen::visit(const DeclStmt* node){
    node->decl->accept(*this);
}

void Zar::CodeGen::visit(const VarDeclNode* node){
    if(_lookup(node->iden).val==nullptr){
        Value* var = builder.CreateAlloca(types.at(node->data_type),nullptr,node->iden);
        if(node->expression == nullptr){
            _addSymbol(node->iden,CG_Symbol(var,types.at(node->data_type),node->mutble));
        }
        else{
            node->expression->accept(*this);
            Value* s_val = _lastVal;
            builder.CreateStore(s_val,var);
            _addSymbol(node->iden,CG_Symbol(var,types.at(node->data_type),node->mutble));
        }
    }
    else{
        std::cerr << "Redeclaration of the variable with same identifier in this scope" << std::endl;
        std::exit(1);
    }
}

void Zar::CodeGen::visit(const AssignStmtNode* node){
    auto sym = _lookup(node->iden);
    if(sym.val!=nullptr){
        if(sym.isMut == false){
            std::cerr << "Assigning a value to a immutable variable" << std::endl;
            std::exit(1);
        }
        else{
            node->expression->accept(*this);
            Value* s_val = _lastVal;
            builder.CreateStore(s_val,sym.val);
        }
    }
}

void Zar::CodeGen::visit(const IfStmtNode* node){
    node->condition->accept(*this);
    Value* cond = _lastVal;
    BasicBlock* thenblock = BasicBlock::Create(context,"then",_current_function);
    BasicBlock* elseblock = BasicBlock::Create(context,"else",_current_function);
    BasicBlock* mergeblock = BasicBlock::Create(context,"merge",_current_function);
    builder.CreateCondBr(cond,thenblock,elseblock);
    builder.SetInsertPoint(thenblock);
    node->thenblock->accept(*this);
    builder.CreateBr(mergeblock);
    if(node->elseBlocks.size()==0){
        builder.CreateBr(mergeblock);
    }
    else if(node->elseBlocks.size()==1){
        builder.SetInsertPoint(elseblock);
        if(node->elseBlocks[0]->condition==nullptr){
            node->elseBlocks[0]->thenblock->accept(*this);
        }
        else{
            node->elseBlocks[0]->accept(*this);
        }
        builder.CreateBr(mergeblock);
    }
    else{
        builder.SetInsertPoint(elseblock);
        for(int i = 0;i<node->elseBlocks.size();i++){
            node->elseBlocks[i]->accept(*this);
        }
        builder.CreateBr(mergeblock);
    }
    builder.SetInsertPoint(mergeblock);
}

void Zar::CodeGen::visit(const LoopStmtNode* node){
    if(node->initializer)
        node->initializer->accept(*this);
    BasicBlock* entryblock = &_current_function->getEntryBlock();
    BasicBlock* loopcond = BasicBlock::Create(context,"loop.cond",_current_function);
    BasicBlock* loopbody = BasicBlock::Create(context,"loop.body",_current_function);
    BasicBlock* loopend = BasicBlock::Create(context,"loop.end",_current_function);
    builder.CreateBr(loopcond);
    builder.SetInsertPoint(loopcond);
    auto lphi = builder.CreatePHI(builder.getInt32Ty(),2,"i_phi");
    lphi->addIncoming(_lastVal,entryblock);
    node->condition->accept(*this);
    Value* cond = _lastVal;
    builder.CreateCondBr(cond,loopbody,loopend);

    builder.SetInsertPoint(loopbody);
    node->body->accept(*this);
    node->update_expr->accept(*this);
    Value* nextval = _lastVal;
    lphi->addIncoming(nextval,loopbody);
    builder.CreateBr(loopcond);

    builder.SetInsertPoint(loopend);
}

void Zar::CodeGen::visit(const Zar::ParamDeclNode* node){
    _c_params_list.push_back(types.at(node->type));
}

void Zar::CodeGen::visit(const Zar::FuncDeclNode* node){
    for(int i = 0;i<node->params.size();i++){
        node->params.at(i)->accept(*this);
    }
    Type* rttype = types.at(node->return_type);
    FunctionType* type = FunctionType::get(rttype,_c_params_list,false);
    Function* fn = Function::Create(
        type,
        Function::ExternalLinkage,
        node->name,
        _current_module.get()
    );
    _c_params_list.clear();
    _current_function = fn;
    BasicBlock* entry = BasicBlock::Create(context,"entry",_current_function);
    builder.SetInsertPoint(entry);
    node->body->accept(*this);
    if(node->return_type == DataType::TYPE_VOID){
        builder.CreateRetVoid();
    }
    else{
        builder.CreateRet(ConstantInt::get(builder.getInt32Ty(),0));
    }
}

void Zar::CodeGen::_pre_gen(){
    _make_module("out");
}

void Zar::CodeGen::_post_gen(){
    std::error_code EC;
    raw_fd_ostream dest("out.ll",EC,sys::fs::OF_None);
    if(EC){
        errs() << "Could Not Open File" << EC.message() << '\n';
        return;
    }
    _current_module->print(dest,nullptr);
    dest.flush();
}

void Zar::CodeGen::generate(){
    _pre_gen();
    for(int i = 0;i<tree.unit.size();i++){
        tree.unit.at(i)->accept(*this);
    }
    _post_gen();
}
