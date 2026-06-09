#pragma once

#include "ast.hpp"
namespace Zar{

    class astNode;
    class Expr;
    class binexprNode;
    class Stmt;
    class BlockStmtNode;
    class VarStmtNode;
    class AssignStmtNode;
    class IfStmtNode;

    class ExprVisitor{
    public:
        virtual ~ExprVisitor() = default;

        virtual void visit(const binexprNode* node) = 0;
        virtual void visit(const literalNode* node) = 0;
    };

    class StmtVisitor{
    public:
        virtual ~StmtVisitor() = default;

        virtual void visit(const ExprStmt* node) = 0;
        virtual void visit(const DeclStmt* node) = 0;

        virtual void visit(const AssignStmtNode* node) = 0;
        virtual void visit(const IfStmtNode* node) = 0;
        virtual void visit(const LoopStmtNode* node) = 0;
        virtual void visit(const Block* node) = 0;
    };

    class DeclVisitor{
    public:
        virtual ~DeclVisitor() = default;

        virtual void visit(const VarDeclNode* node) = 0;
        virtual void visit(const FuncDeclNode* node) = 0;
        virtual void visit(const ParamDeclNode* node) = 0;
        virtual void visit(const ExternFuncDeclNode* node) = 0;
    };

}
