#pragma once

#include "../include/parser.hpp"
#include "../include/visitor.hpp"
#include "ast.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <vector>

using namespace llvm;


namespace Zar{
    class CodeGen: public ExprVisitor,public StmtVisitor,public DeclVisitor{
    private:
        LLVMContext context;
        std::unique_ptr<Module> _current_module;
        IRBuilder<> builder;
        void _make_module(std::string name);

        Function* _current_function;

        TranslationUnit tree;

        class CG_Symbol{
        public:
            Value* val;
            Type* typ;
            bool isMut;
            CG_Symbol(Value* v):
                val(v) {}
            CG_Symbol(Value* v,Type*t,bool m):
                val(v),typ(t),isMut(m) {}
        };


        // Symbol table stack
        std::vector<std::unordered_map<std::string,CG_Symbol>> symtabstack;

        // Symbol table helpers
        void _pushScope();
        void _popScope();
        void _addSymbol(std::string name,CG_Symbol symbol);
        CG_Symbol _lookup(std::string name);

        std::unordered_map<Zar::DataType,IntegerType*> types = {
            {DataType::TYPE_INT,builder.getInt32Ty()},
        };

        // Expression CodeGen helpers
        Value* _lastVal = nullptr;
        Value* _getRValue(Value* v,Type* t);
        Value* _genAdd(Value* lhs,Value* rhs,std::string rn);
        Value* _genSub(Value* lhs,Value* rhs,std::string rn);
        Value* _genMul(Value* lhs,Value* rhs,std::string rn);
        Value* _genDiv(Value* lhs,Value* rhs,std::string rn);
        Value* _genEq(Value* lhs,Value* rhs,std::string rn);
        Value* _genNEq(Value* lhs,Value* rhs,std::string rn);
        Value* _genGT(Value* lhs,Value* rhs,std::string rn);
        Value* _genLT(Value* lhs,Value* rhs,std::string rn);
        Value* _genGEq(Value* lhs,Value* rhs,std::string rn);
        Value* _genLEq(Value* lhs,Value* rhs,std::string rn);

        // Conditional Helpers
        std::vector<int> _ifstack;
        int _if_count{0};
        void _enterif();
        void _exitif();
        std::string _get_cond_name(std::string bnme);

        void _pre_gen();
        void _post_gen();

    public:
        void visit(const binexprNode* node) override;
        void visit(const literalNode* node) override;
        void visit(const ExprStmt* node) override;
        void visit(const DeclStmt* node) override;
        void visit(const Block* node) override;
        void visit(const AssignStmtNode* node) override;
        void visit(const IfStmtNode* node) override;
        void visit(const LoopStmtNode* node) override;
        void visit(const VarDeclNode* node) override;
        void visit(const FuncDeclNode* node) override;
        void visit(const ParamDeclNode* node) override;


        CodeGen(TranslationUnit Tree):
            tree(std::move(Tree)),builder(context) {}

        void generate();
    };
}
