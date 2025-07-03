#pragma once

#include "namespace.hpp"
#include "lexer.hpp"
#include "parser.hpp"

namespace Zar{
    class PrettyPrinter{
    private:
        std::string _node_to_string(NodeTypes n);
        void _print_recur(const Zar::exprNode* node,int indent);
    public:
        Zar::exprNode* start;
        PrettyPrinter(Zar::exprNode* tr):
            start(tr){}  
        void print_ast();
};
}