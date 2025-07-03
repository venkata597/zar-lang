#pragma once

#include "namespace.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>

namespace Zar{
    enum class NodeTypes{
        N_INT,
        N_PLUS,
        N_MINUS,
        N_MULT,
        N_DIV,
    };

    extern std::vector<int> prec_table;

    class astNode{
    public:
        NodeTypes ntype;
        astNode(NodeTypes n): ntype(n) {}
        virtual ~astNode() = default;
    };

    class exprNode : public astNode{
    public:
        std::unique_ptr<exprNode> left;
        std::unique_ptr<exprNode> right;
        std::string value;

        exprNode(NodeTypes nt,std::unique_ptr<exprNode> l,std::unique_ptr<exprNode> r,std::string v):
            astNode(nt),left(std::move(l)),value(v),right(std::move(r)){}
        
        exprNode(NodeTypes nt,std::string v):
            astNode(nt),value(v),left(nullptr),right(nullptr) {}
        
        exprNode(NodeTypes nt,std::unique_ptr<exprNode> l,std::string v):
            astNode(nt),left(std::move(l)),right(nullptr),value(v) {}
    };
    
    class Parser{
    private:
        /*Private Variables*/
        std::vector<Zar::Token> _tkns;
        Zar::Token _Tok;
        int _token_track{};

        /*Private Methods*/
        void _updateToken();
        std::unique_ptr<exprNode> _parseTerm();
        std::unique_ptr<exprNode> _buildTree(int pre);
        NodeTypes _get_nodetype(TokenTypes tktp);
        int _get_precedence(const Zar::Token tk);
    public:
        Parser(std::vector<Zar::Token> tk_vct):
            _tkns(tk_vct){}
        std::unique_ptr<exprNode> get_ast();
    };
}
