#include "../include/parser.hpp"

std::vector<int> Zar::prec_table = {
        0,  // INT
        10, // PLUS
        10, // MINUS
        20, // MULT
        20, // DIV
        0,  // EOF
    };

void Zar::Parser::_updateToken(){
    if(_token_track < _tkns.size())
        Zar::Parser::_Tok = _tkns[_token_track++];
}

std::unique_ptr<Zar::exprNode> Zar::Parser::_parseTerm(){
    std::unique_ptr<Zar::exprNode> term;
    switch(_Tok._type){
        case Zar::TokenTypes::INT:
            term = std::make_unique<exprNode>(Zar::NodeTypes::N_INT,_Tok._lexeme);
            _updateToken();
            return std::move(term);
        default:
            std::cerr << "Syntax error at Line: " << _Tok._line << '\n';
            std::exit(1);
    }
}

Zar::NodeTypes Zar::Parser::_get_nodetype(TokenTypes tktp){
    switch(tktp){
        case Zar::TokenTypes::PLUS:
            return Zar::NodeTypes::N_PLUS;
            break;
        case Zar::TokenTypes::MINUS:
            return Zar::NodeTypes::N_MINUS;
            break;
        case Zar::TokenTypes::MULT:
            return Zar::NodeTypes::N_MULT;
            break;
        case Zar::TokenTypes::DIV:  
            return Zar::NodeTypes::N_DIV;
            break;
    }
}

int Zar::Parser::_get_precedence(const Zar::Token tk){
    int index = static_cast<int>(tk._type);
    if(index<0 || index>5){
        std::cerr << "Encountered unknown token!!" << '\n';
        std::exit(1);
    }
    return prec_table.at(index);
}

std::unique_ptr<Zar::exprNode> Zar::Parser::_buildTree(int pre){
    std::unique_ptr<exprNode> tree,left,right;
    Zar::TokenTypes type;
    left = _parseTerm();
    
    while(_get_precedence(_Tok) > pre){
        type = _Tok._type;
        _updateToken();
        right = _buildTree(prec_table[static_cast<int>(type)]);

        left = std::make_unique<exprNode>(_get_nodetype(type),std::move(left),std::move(right),"\0");
        Zar::TokenTypes tktp = _Tok._type;
        if(tktp == Zar::TokenTypes::_EOF)
            return std::move(left);
    }
    return std::move(left);
}


std::unique_ptr<Zar::exprNode> Zar::Parser::get_ast(){
    _updateToken();
    std::unique_ptr<Zar::exprNode> ast = _buildTree(0);
    return std::move(ast);
}

