#include "../include/ast.hpp"

std::string Zar::PrettyPrinter::_node_to_string(Zar::NodeTypes n){
    switch(n){
        case Zar::NodeTypes::N_PLUS:
            return "ADD";
            break;
        case Zar::NodeTypes::N_MINUS:
            return "MINUS";
            break;
        case Zar::NodeTypes::N_MULT:
            return "MULT";
            break;
        case Zar::NodeTypes::N_DIV:
            return "DIV";
            break;
        case Zar::NodeTypes::N_INT:
            return "INT_LIT";
            break;
    }
}


void Zar::PrettyPrinter::_print_recur(const Zar::exprNode* node,int indent){
    if(!node) return;
    for(int i = 0;i<indent;i++) std::cout << " ";
    std::cout << _node_to_string(node->ntype);
    if(node->ntype == Zar::NodeTypes::N_INT) std::cout << " : " << node->value;
    std::cout << '\n';

    _print_recur(node->left.get(),indent+1);
    _print_recur(node->right.get(),indent+1);
}

void Zar::PrettyPrinter::print_ast(){
    _print_recur(start,0);
}  