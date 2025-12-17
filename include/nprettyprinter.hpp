#pragma once

#include "ast.hpp"
#include "parser.hpp"
#include "visitor.hpp"
#include <string>
#include <unordered_map>

namespace Zar{
class NPrettyPrinter: public ExprVisitor,public StmtVisitor,public DeclVisitor{
    private:
        TranslationUnit ast;
        std::unordered_map<ExprType,std::string> _expr_lookup = {
            {Zar::ExprType::EXPR_BIN, "Binary Expression"},
            {Zar::ExprType::EXPR_LITRL, "Literal Expression"},
            {Zar::ExprType::EXPR_UNARY,"Unary Expression"},
            {Zar::ExprType::EXPR_CALL,"Call Expression"}
        };

        std::string _toString(BinaryOp bop) const ;
        std::string _type_to_str(Zar::DataType dt) const ;

        // Expression printing
        int _current_indent = 0;
        void _recur_print(const binexprNode* node);
    public:
        NPrettyPrinter(TranslationUnit tree):
            ast(std::move(tree)){}

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

        TranslationUnit print_ast();
};
}
