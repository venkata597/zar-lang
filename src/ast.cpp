#include "../include/ast.hpp"
#include "../include/visitor.hpp"

void Zar::binexprNode::accept(Zar::ExprVisitor& v) {
    v.visit(this);
}

void Zar::literalNode::accept(Zar::ExprVisitor& v) {
    v.visit(this);
}

void Zar::ExprStmt::accept(Zar::StmtVisitor& v){
    v.visit(this);
}

void Zar::DeclStmt::accept(Zar::StmtVisitor& v){
    v.visit(this);
}

void Zar::Block::accept(Zar::StmtVisitor& v){
    v.visit(this);
}

void Zar::AssignStmtNode::accept(Zar::StmtVisitor& v) {
    v.visit(this);
}

void Zar::IfStmtNode::accept(Zar::StmtVisitor& v) {
    v.visit(this);
}

void Zar::LoopStmtNode::accept(Zar::StmtVisitor& v){
    v.visit(this);
}

void Zar::VarDeclNode::accept(Zar::DeclVisitor& v){
    v.visit(this);
}

void Zar::FuncDeclNode::accept(Zar::DeclVisitor& v){
    v.visit(this);
}

void Zar::ParamDeclNode::accept(Zar::DeclVisitor& v){
    v.visit(this);
}
