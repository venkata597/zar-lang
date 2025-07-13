#pragma once

#include "namespace.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>

namespace Zar{
    enum class TokenTypes{
        INT,
        PLUS,
        MINUS,
        MULT,
        DIV,
        _EOF,
        Null,
    };

    struct Token{
        TokenTypes _type;
        std::string _lexeme;
        int _line;
    };

    class Lexer{
        private:
            /*Lexer required vars*/
            std::string _contents{};
            int _track{};
            char _current;
            char _putback{};
            int _linec{0};
            std::ifstream _file;

            /*All lexer methods*/
            std::string _openFile();
            Token _make_token(TokenTypes ty,std::string lex,int lc);
            void _advance();
            void _put_back(char c);
            void _skip_whitespace();
            Token _getToken(char _ch);
            std::string _get_num();

        public:
            std::string path;
            Lexer(std::string pth){
                path = pth;
                _contents = _openFile();
            }
            std::vector<Zar::Token> scanFile(); 
    };
}