#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>

namespace Zar{

    class ErrorEngine;

    enum class TokenTypes{
        // ARITHMETIC OPERATIONS
        PLUS = 1,
        MINUS = 2,
        MULT = 3,
        DIV = 4,
        EQ_EQ,
        BANG_EQ,
        LESS,
        GREATER,
        LESS_EQ,
        GREATER_EQ,
        _EOF,
        Null,
        SEMIC,
        COLON,

        L_PAREN,
        R_PAREN,
        L_CURLY,
        R_CURLY,
        COMMA,
        DOT,
        BANG,
        EQ,
        // Keywords
        VOID,
        INT,
        FLOAT,
        LONG,
        SHORT,
        STRING,
        CHAR,
        STRUCT,
        IMPORT,
        AND,
        OR,
        NOT,
        IF,
        ELSE,
        FUNC,
        RETURN,
        LOOP,
        TRUE,
        FALSE,
        LET,
        VAR,
        MUT,
        BREAK,
        CONTINUE,
        EXTERN,
        // Literals
        IDENTIFIER,
        CHAR_LIT,
        STRING_LIT,
        NUMBER = 0,
    };

    struct SourceLocation{
        std::string file;
        int _line;
        int _column;
    };

    struct Token{
        TokenTypes _type;
        std::string _lexeme;
        SourceLocation _src_loc;
    };

    extern std::unordered_map<std::string,TokenTypes> keyword_lookup;
    extern std::vector<TokenTypes> type_lookup;

    class Lexer{
        private:
            std::string _file_name;
            /*Lexer required vars*/
            std::string _contents{};
            std::vector<std::string> _line_cache;
            int _track{};
            char _current;
            char _putback{};
            int _linec{1};
            int _columnc{1};
            std::ifstream _file;

            ErrorEngine& err_engine;

            /*All lexer methods*/
            std::string _openFile();
            void _split(char delim);
            Token _make_token(TokenTypes ty,std::string lex,int lc,int cc);
            void _advance();
            void _put_back(char c);
            char _peek();
            void _skip_whitespace();
            Token _getToken(char _ch);
            std::string _get_num();
            std::string _scan_ident();
            char _scan_char();
            std::string _scan_string();
            bool _is_keyword(std::string st);
            TokenTypes _get_keyword(std::string st);

        public:
            std::string path;
            Lexer(std::string pth,ErrorEngine& e): err_engine(e){
                _file_name = pth;
                path = pth;
                _contents = _openFile();
                _split('\n');
            }
            std::vector<Zar::Token> scanFile();
            std::vector<std::string> getLines();
    };
}
