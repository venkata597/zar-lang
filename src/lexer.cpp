#include "../include/lexer.hpp"
#include "../include/error.hpp"

std::unordered_map<std::string,Zar::TokenTypes> Zar::keyword_lookup = {
    {"int",Zar::TokenTypes::INT},
    {"float",Zar::TokenTypes::FLOAT},
    {"long",Zar::TokenTypes::LONG},
    {"short",Zar::TokenTypes::SHORT},
    {"string",Zar::TokenTypes::STRING},
    {"char",Zar::TokenTypes::CHAR},
    {"import",Zar::TokenTypes::IMPORT},
    {"and",Zar::TokenTypes::AND},
    {"or",Zar::TokenTypes::OR},
    {"not",Zar::TokenTypes::NOT},
    {"if",Zar::TokenTypes::IF},
    {"else",Zar::TokenTypes::ELSE},
    {"fn",Zar::TokenTypes::FUNC},
    {"return",Zar::TokenTypes::RETURN},
    {"for",Zar::TokenTypes::FOR},
    {"while",Zar::TokenTypes::WHILE},
    {"true",Zar::TokenTypes::TRUE},
    {"false",Zar::TokenTypes::FALSE},
    {"let",Zar::TokenTypes::LET},
    {"var",Zar::TokenTypes::VAR},
    {"mut",Zar::TokenTypes::MUT},
    {"break",Zar::TokenTypes::BREAK},
    {"continue",Zar::TokenTypes::CONTINUE},
    {"struct",Zar::TokenTypes::STRUCT},
    {"extern",Zar::TokenTypes::EXTERN}
};

std::vector<Zar::TokenTypes> Zar::type_lookup = {
    TokenTypes::INT,
    TokenTypes::FLOAT,
    TokenTypes::SHORT,
    TokenTypes::LONG,
    TokenTypes::CHAR,
    TokenTypes::STRING,
    TokenTypes::STRUCT
};

std::string Zar::Lexer::_openFile(){
    Zar::Lexer::_file.open(path,std::ios::binary | std::ios::ate);
    std::streamsize file_size = _file.tellg();

    _file.seekg(0,std::ios::beg);

    std::string buffer(file_size,'\0');
    _file.read(&buffer[0],file_size);

    return buffer;
}

void Zar::Lexer::_split(char delim){
    std::string buf;
    for(auto c: _contents){
        if(c==delim){
            _line_cache.push_back(buf);
            buf.clear();
        }
        else{
            buf+=c;
        }
    }
    if(!buf.empty()){
        _line_cache.push_back(buf);
    }
    err_engine.set_source(_line_cache);
}

Zar::Token Zar::Lexer::_make_token(Zar::TokenTypes ty,std::string lex,int lc,int cc){
    Zar::Token tk;
    tk._type = ty;
    tk._lexeme = lex;
    tk._src_loc = {.file=_file_name,._line=lc,._column=cc};
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

char Zar::Lexer::_peek(){
    _advance();
    char next = _current;
    _put_back(next);
    return next;
}

void Zar::Lexer::_skip_whitespace(){
    while(_current == ' ' || _current == '\t' || _current == '\n' || _current == '\r'){
        if(_current == '\n'){_linec++;_columnc=0;}
        else{_columnc++;}
        _advance();
    }
}

std::string Zar::Lexer::_get_num(){
    std::string num;
    while(isdigit(_current) || _current == '.'){
        _columnc++;
        num+=_current;
        _advance();
    }
    _put_back(_current);
    return num;
}

std::string Zar::Lexer::_scan_ident(){
    std::string buf;
    while(isalpha(_current) || isalnum(_current) || _current == '_'){
        _columnc++;
        buf.push_back(_current);
        _advance();
    }
    _put_back(_current);
    return buf;
}

char Zar::Lexer::_scan_char(){
    char buf;
    _advance();
    buf = _current;
    _columnc++;
    _advance();
    if(_current == '\''){
        return buf;
    }
    else{
        err_engine.report_error(ERROR,"No closing \' for character declaration",{.file=_file_name,._line=_linec,._column=_columnc});
        return buf;
    }
}

std::string Zar::Lexer::_scan_string(){
    std::string buf;
    _advance();
    while(_current!='\"' && _current != ';'){
        _columnc++;
        buf.push_back(_current);
        _advance();
    }
    if(_current == '\"'){
        _columnc++;
        return buf;
    }
    else{
        err_engine.report_error(ERROR,"No closing \" for string declaration",{.file=_file_name,._line=_linec,._column=_columnc});
        return buf;
    }
}

bool Zar::Lexer::_is_keyword(std::string st){
    auto k_it = keyword_lookup.find(st);
    if(k_it!=keyword_lookup.end()){
        return true;
    }
    else{
        return false;
    }
}

Zar::TokenTypes Zar::Lexer::_get_keyword(std::string st){
    return keyword_lookup.at(st);
}

Zar::Token Zar::Lexer::_getToken(char _ch){
    switch(_ch){
        case '+':
            return _make_token(Zar::TokenTypes::PLUS,"+",(_linec),_columnc++);
        case '-':
            return _make_token(Zar::TokenTypes::MINUS,"-",(_linec),_columnc++);
        case '*':
            return _make_token(Zar::TokenTypes::MULT,"*",(_linec),_columnc++);
        case '/':
            return _make_token(Zar::TokenTypes::DIV,"/",(_linec),_columnc++);
        case ';':
            return _make_token(Zar::TokenTypes::SEMIC,";",(_linec),_columnc++);
        case ',':
            return _make_token(Zar::TokenTypes::COMMA,",",(_linec),_columnc++);
        case '(':
            return _make_token(Zar::TokenTypes::L_PAREN,"(",(_linec),_columnc++);
        case ')':
            return _make_token(Zar::TokenTypes::R_PAREN,")",(_linec),_columnc++);
        case '{':
            return _make_token(Zar::TokenTypes::L_CURLY,"{",(_linec),_columnc++);
        case '}':
            return _make_token(Zar::TokenTypes::R_CURLY,"}",(_linec),_columnc++);
        case ':':
            return _make_token(Zar::TokenTypes::COLON,":",(_linec),_columnc++);
        case '=':
            if(_peek() == '='){
                _advance();
                return _make_token(Zar::TokenTypes::EQ_EQ,"==",(_linec),_columnc++);
            }
            else{
                return _make_token(Zar::TokenTypes::EQ,"=",(_linec),_columnc++);
            }
        case '<':
            if(_peek() == '='){
                _advance();
                return _make_token(Zar::TokenTypes::LESS_EQ,"<=",(_linec),_columnc++);
            }
            else{
                return _make_token(Zar::TokenTypes::LESS,"<",(_linec),_columnc++);
            }
        case '>':
            if(_peek() == '='){
                _advance();
                return _make_token(Zar::TokenTypes::GREATER_EQ,">=",(_linec),_columnc++);
            }
            else{
                return _make_token(Zar::TokenTypes::GREATER,">",(_linec),_columnc++);
            }
        case '!':
            if(_peek() == '='){
                _advance();
                return _make_token(Zar::TokenTypes::BANG_EQ,"!=",(_linec),_columnc++);
            }
            else{
                return _make_token(Zar::TokenTypes::BANG,"!",(_linec),_columnc++);
            }
        case '\'':
            if(_peek()=='\''){
                _advance();
                return _make_token(Zar::TokenTypes::CHAR_LIT,"",(_linec),_columnc);
            }
            else{
                return _make_token(Zar::TokenTypes::CHAR_LIT,std::string(1,_scan_char()),(_linec),_columnc);
            }
        case '\"':
            if(_peek()=='\"'){
                _advance();
                return _make_token(Zar::TokenTypes::STRING_LIT,"",(_linec),_columnc);
            }
            else{
                return _make_token(Zar::TokenTypes::STRING_LIT,_scan_string(),(_linec),_columnc);
            }
        default:
            if(isdigit(_ch)){
                return _make_token(Zar::TokenTypes::NUMBER,_get_num(),(_linec),_columnc);
            }
            else if(isalpha(_ch) || _ch == '_'){
                std::string ident = _scan_ident();
                if(_is_keyword(ident)){
                    return _make_token(_get_keyword(ident),ident,(_linec),_columnc);
                }
                else{
                    return _make_token(Zar::TokenTypes::IDENTIFIER,ident,(_linec),_columnc);
                }
            }
            else if(_ch=='\0'){
                return _make_token(Zar::TokenTypes::_EOF,std::string(1,_ch),(_linec),_columnc++);
            }
            else{
                return _make_token(Zar::TokenTypes::Null,std::string(1,' '),(_linec),_columnc++);
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
    tokens_vect.push_back(_getToken(_current));
    return tokens_vect;
}

std::vector<std::string> Zar::Lexer::getLines(){
    return _line_cache;
}
