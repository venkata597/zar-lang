#include "../include/nprettyprinter.hpp"
#include "../include/ast.hpp"
#include <iostream>

std::string Zar::NPrettyPrinter::_toString(BinaryOp bop) const {
    switch (bop) {
            case BinaryOp::ADD:  return "ADD";
            case BinaryOp::SUB:  return "SUB";
            case BinaryOp::MULT: return "MULT";
            case BinaryOp::DIV:  return "DIV";
            case BinaryOp::EQ: return "  EQUALS";
            case BinaryOp::NEQ: return "  NOT_EQUALS";
            case BinaryOp::GT: return "  GREATER_THAN";
            case BinaryOp::LT: return "  LESS_THAN";
            case BinaryOp::GTE: return "  GREATER_THAN_EQUALS";
            case BinaryOp::LTE: return "  LESS_THAN_EQUALS";
            case BinaryOp::TERM: return "TERM";
            default:             return "UNKNOWN";
        }
}

std::string Zar::NPrettyPrinter::_type_to_str(Zar::DataType dt) const {
    switch(dt){
        case DataType::TYPE_INT : return "INT";
        case DataType::TYPE_FLOAT : return "FLOAT";
        case DataType::TYPE_LONG : return "LONG";
        case DataType::TYPE_SHORT : return "SHORT";
        case DataType::TYPE_CHAR : return "CHAR";
        case DataType::TYPE_STRING: return "STRING";
        case DataType::TYPE_USER_DEFINED : return "USER DEFINED";
        default : return "UNKNOWN";
    }
}

void Zar::NPrettyPrinter::_recur_print(const binexprNode* node) {
    if(!node) return;
    std::string prefix(_current_indent,' ');
    std::cout << prefix << _toString(node->Op) << '\n';
    _current_indent += 2;
    if(node->left) node->left->accept(*this);
    if(node->right) node->right->accept(*this);
    _current_indent -= 2;
}

void Zar::NPrettyPrinter::visit(const binexprNode* node) {
    _recur_print(node);
}

void Zar::NPrettyPrinter::visit(const literalNode* node) {
    std::cout << "Term: " << node->value << '\n';
}

void Zar::NPrettyPrinter::visit(const Block* node) {
    std::cout << " Block: " << '\n';
    for(int i = 0;i<node->block.size();i++){
        node->block[i]->accept(*this);
    }
}

void Zar::NPrettyPrinter::visit(const ExprStmt* node){
    node->expr->accept(*this);
}

void Zar::NPrettyPrinter::visit(const DeclStmt* node){
    node->decl->accept(*this);
}

void Zar::NPrettyPrinter::visit(const VarDeclNode* node) {
    node->mutble ? std::cout << "Mutable " : std::cout << "Immutable ";
    std::cout << _type_to_str(node->data_type) << '\n';
    node->expression->accept(*this);
    std::cout << "Identifier: " << node->iden << '\n';
}

void Zar::NPrettyPrinter::visit(const AssignStmtNode* node) {
    std::cout << "Identifier: " << node->iden << '\n';
    node->expression->accept(*this);
}

void Zar::NPrettyPrinter::visit(const IfStmtNode* node) {
    std::cout << "================" << '\n';
    if(node->condition==nullptr) std::cout << "Else block no condition";
    else{std::cout << "Condition: " << '\n';node->condition->accept(*this);}
    node->thenblock->accept(*this);
    if(node->elseBlocks.size()!=0){
        for(int i = 0;i<node->elseBlocks.size();i++){
            node->elseBlocks[i]->accept(*this);
        }
    }
    std::cout << "================" << '\n';
}

void Zar::NPrettyPrinter::visit(const Zar::LoopStmtNode* node){
    std::cout << "================" << '\n';
    if (node->initializer) {
        std::cout << "FOR LOOP: " << '\n';
        std::cout << "INITIALIZER: " << '\n';
        node->initializer->accept(*this);
    }
    else if(node->initializer == nullptr && node->condition!=nullptr){
        std::cout << "WHILE LOOP: " << '\n';
        std::cout << "CONDITION: " << '\n';
        node->condition->accept(*this);
    }
    if(node->update_expr){
        std::cout << "Update Expression: " << '\n';
        node->update_expr->accept(*this);
    }
    node->body->accept(*this);
}

void Zar::NPrettyPrinter::visit(const Zar::ParamDeclNode* node){
    std::cout << "  Parameter Identifier: " << node->name << '\n';
    std::cout << "  Parameter Type: " << _type_to_str(node->type) << '\n';
}

void Zar::NPrettyPrinter::visit(const Zar::FuncDeclNode* node){
    std::cout << "================" << '\n';
    std::cout << "Function name: " << node->name << '\n';
    std::cout << "Return Type: " << _type_to_str(node->return_type) << '\n';
    std::cout << "Parameters: " << '\n';
    for(int i = 0;i<node->params.size();i++){
        node->params[i]->accept(*this);
    }
    node->body->accept(*this);
    std::cout << "================" << '\n';
}

Zar::TranslationUnit Zar::NPrettyPrinter::print_ast() {
    for(int i = 0;i<ast.unit.size();i++){
        if(!ast.unit[i]){
            std::cout << "Null ptr detected" << std::endl;
        }
        else{
            ast.unit[i]->accept(*this);
        }
    }
    return std::move(ast);
}
