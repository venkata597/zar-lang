#include "../include/lexer.hpp"

std::string Zar::Lexer::_openFile(){
    Zar::Lexer::_file.open(path,std::ios::binary | std::ios::ate);
    std::streamsize file_size = _file.tellg();
    
    _file.seekg(0,std::ios::beg);

    std::string buffer(file_size,'\0');
    _file.read(&buffer[0],file_size);

    return buffer;
}

Zar::Token Zar::Lexer::_make_token(Zar::TokenTypes ty,std::string lex,int lc){
    Zar::Token tk;
    tk._type = ty;
    tk._lexeme = lex;
    tk._line = lc;
    return tk;
}

void Zar::Lexer::_advance(){
    if(_putback){
     _current = _putback;
     _putback = '\0';   
    }else if(_track<_contents.size()){
        _current = _contents.at(_track++);
    }
    else{
        _current = '\0';
    }
}

void Zar::Lexer::_put_back(char c){
    Zar::Lexer::_putback = c;
}

void Zar::Lexer::_skip_whitespace(){
    while(_current == ' ' || _current == '\t' || _current == '\n' || _current == '\r'){
        if(_current == '\n') _linec++;
        _advance();
    }
}

std::string Zar::Lexer::_get_num(){
    std::string num;
    while(isdigit(_current)){
        num+=_current;
        _advance();
    }
    _put_back(_current);
    return num;
}

Zar::Token Zar::Lexer::_getToken(char _ch){
    switch(_ch){
        case '+':
            return _make_token(Zar::TokenTypes::PLUS,"+",(_linec+1));
            break;
        case '-':
            return _make_token(Zar::TokenTypes::MINUS,"-",(_linec+1));
            break;
        case '*':
            return _make_token(Zar::TokenTypes::MULT,"*",(_linec+1));
            break;
        case '/':
            return _make_token(Zar::TokenTypes::DIV,"/",(_linec+1));
            break;
        default:
            if(isdigit(_ch)){
                return _make_token(Zar::TokenTypes::INT,_get_num(),(_linec+1));
                break;
            }
            else if(_ch=='\0'){
                return _make_token(Zar::TokenTypes::_EOF,std::string(1,_ch),(_linec+1));
                break;
            }
            else{
                return _make_token(Zar::TokenTypes::Null,std::string(1,' '),(_linec+1));
                break;
            }
    }
}

std::vector<Zar::Token> Zar::Lexer::scanFile(){
    std::vector<Zar::Token> tokens_vect;
    _advance();
    while(_current!='\0'){
        _skip_whitespace();
        tokens_vect.push_back(_getToken(_current));
        _advance();
    }
    return tokens_vect;
}