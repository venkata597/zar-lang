#include "../include/parser.hpp"
#include "../include/ast.hpp"
#include "../include/lexer.hpp"
#include "../include/error.hpp"
#include <memory>
#include <unordered_set>

std::vector<int> Zar::prec_table = {
        0,  // NUMBER
        10, // PLUS
        10, // MINUS
        20, // MULT
        20, // DIV
        30, // EQUAL TO
        30, // NOT EQUAL TO
        40, // LESS THAN
        40, // GREATER THAN
        40, // LESS THAN OR EQUAL TO
        40, // GREATER THAN OR EQUAL TO
};

std::unordered_map<Zar::TokenTypes,Zar::BinaryOp> Zar::Opref = {
    {Zar::TokenTypes::PLUS,Zar::BinaryOp::ADD},
    {Zar::TokenTypes::MINUS,Zar::BinaryOp::SUB},
    {Zar::TokenTypes::MULT,Zar::BinaryOp::MULT},
    {Zar::TokenTypes::DIV,Zar::BinaryOp::DIV},
    {Zar::TokenTypes::EQ_EQ,Zar::BinaryOp::EQ},
    {Zar::TokenTypes::BANG_EQ,Zar::BinaryOp::NEQ},
    {Zar::TokenTypes::GREATER,Zar::BinaryOp::GT},
    {Zar::TokenTypes::LESS,Zar::BinaryOp::LT},
    {Zar::TokenTypes::LESS_EQ,Zar::BinaryOp::LTE},
    {Zar::TokenTypes::GREATER_EQ,Zar::BinaryOp::GTE}
};

std::unordered_map<Zar::TokenTypes,Zar::DataType> Zar::type_ref = {
    {Zar::TokenTypes::INT,Zar::DataType::TYPE_INT},
    {Zar::TokenTypes::FLOAT,Zar::DataType::TYPE_FLOAT},
    {Zar::TokenTypes::CHAR,Zar::DataType::TYPE_CHAR},
    {Zar::TokenTypes::STRING,Zar::DataType::TYPE_STRING},
    {Zar::TokenTypes::LONG,Zar::DataType::TYPE_LONG},
    {Zar::TokenTypes::SHORT,Zar::DataType::TYPE_SHORT},
    {Zar::TokenTypes::VOID,Zar::DataType::TYPE_VOID},
};

void Zar::Parser::_advance(){
    if(_token_track+1<_tkns.size()){
        Zar::Parser::_Tok = _tkns[++_token_track];
    }
    else{
        _Tok = _tkns.back();
    }
}

void Zar::Parser::_sync(Zar::TokenTypes tkn){
    while(_Tok._type!=tkn && _Tok._type!=Zar::TokenTypes::_EOF){
        _advance();
    }
}

/*
    void Parser::_sync(std::array<TokenTypes> barr){
        std::unordered_set<TokenTypes> set(barr.begin(),barr.end());
        while(!set.count(_Tok._type) && _Tok._type!=TokenTypes::_EOF){
            _advance();
        }
    }
 */

Zar::Token Zar::Parser::_peek(int offset = 0){
    return _tkns[_token_track+offset];
}

bool Zar::Parser::_check(Zar::TokenTypes expected){
    return _peek()._type == expected;
}

Zar::ExprPtr Zar::Parser::_parse_primary(){
    switch(_Tok._type){
        case Zar::TokenTypes::NUMBER:{
            auto term = std::make_unique<literalNode>(_Tok._lexeme,ExprType::EXPR_LITRL);
            _advance();
            return std::move(term);
        }
        case Zar::TokenTypes::IDENTIFIER:{
            auto term = std::make_unique<literalNode>(_Tok._lexeme,ExprType::EXPR_IDEN_LITRL);
            _advance();
            return std::move(term);
        }
        default:
            err_engine.report_error(ERROR,"Unknown Expression kind found",_peek(-1)._src_loc);
            return nullptr;
    }
}

Zar::BinaryOp Zar::Parser::_get_optype(TokenTypes tktp){return Opref.at(tktp);}

Zar::DataType Zar::Parser::_get_datatype(TokenTypes tktp){return type_ref.at(tktp);}

int Zar::Parser::_get_precedence(const Zar::Token tk){
    int index = static_cast<int>(tk._type);
    if(index<0 || index>10){
        return -1;
    }
    return prec_table.at(index);
}

Zar::ExprPtr Zar::Parser::_parse_expression(int pre){
    ExprPtr tree,left,right;
    Zar::TokenTypes type;
    left = _parse_primary();

    while(_Tok._type!= Zar::TokenTypes::_EOF && _get_precedence(_Tok) > pre){
        type = _Tok._type;
        _advance();
        right = _parse_expression(prec_table[static_cast<int>(type)]);

        left = std::make_unique<binexprNode>(_get_optype(type),std::move(left),std::move(right),"\0");
        Zar::TokenTypes tktp = _Tok._type;
        if(tktp == Zar::TokenTypes::SEMIC){
            return std::move(left);
        }
    }
    return std::move(left);
}

Zar::ExprPtr Zar::Parser::_parseExpression(){
    ExprPtr expr = _parse_expression(0);
    return std::move(expr);
}


Zar::VarDeclPtr Zar::Parser::_var_decl(bool mut_flag){
    std::unique_ptr<Zar::VarDeclNode> var;
    std::string ident;
    if(_check(Zar::TokenTypes::IDENTIFIER)){
        ident = _Tok._lexeme;
        _advance();
    }
    else{
        err_engine.report_error(ERROR,"Expected an identifier",_peek(-1)._src_loc);
    }
    _check(Zar::TokenTypes::COLON) ? _advance() : err_engine.report_error(ERROR,"Expected at ':' ",_peek(-1)._src_loc);
    std::unordered_set<TokenTypes> set(type_lookup.begin(),type_lookup.end());
    Zar::DataType nt;
    if(set.count(_peek()._type)){
        nt = _get_datatype(_Tok._type);
    }
    else{
        err_engine.report_error(ERROR,"Expected a datatype",_peek(-1)._src_loc);
    }
    _advance();
    ExprPtr expression{nullptr};
    if(_check(Zar::TokenTypes::EQ)){
        _advance();
        expression = _parseExpression();
    }
    else{
        var = std::make_unique<Zar::VarDeclNode>(ident,nt,mut_flag,std::move(expression));
    }
    var = std::make_unique<Zar::VarDeclNode>(ident,nt,mut_flag,std::move(expression));
    return std::move(var);
}

Zar::AssignStmtPtr Zar::Parser::_assignment(){
    ExprPtr expression;
    std::string ident;
    if(_check(Zar::TokenTypes::IDENTIFIER)){
        ident = _Tok._lexeme;
        _advance();
    }
    else{
        err_engine.report_error(ERROR,"Expected an Identifier",_peek(-1)._src_loc);
    }
    _check(Zar::TokenTypes::EQ) ? _advance() : err_engine.report_error(ERROR,"Expected an '='",_peek(-1)._src_loc);
    expression = _parseExpression();
    std::unique_ptr<Zar::AssignStmtNode> asgn = std::make_unique<Zar::AssignStmtNode>(ident,std::move(expression));
    return std::move(asgn);
}

Zar::BlockPtr Zar::Parser::_parse_block(){
    std::vector<StmtPtr> bvect;
    while(!_check(Zar::TokenTypes::R_CURLY) && _Tok._type != TokenTypes::_EOF){
        auto astnd = _parse_item();
        if(astnd){
            bvect.push_back(std::move(astnd));
        }
        else{
            _advance();
        }
    }
    if(_Tok._type == TokenTypes::R_CURLY){
        _advance();
    }
    else{
        err_engine.report_error(ERROR,"Expected a '}' to close block" ,_peek(-1)._src_loc);
    }
    return std::make_unique<Block>(std::move(bvect));
}

Zar::IfStmtPtr Zar::Parser::_parse_if(){
    BlockPtr thenblock;
    if(_check(Zar::TokenTypes::L_PAREN)){
        _advance();
    }
    else{
        err_engine.report_error(ERROR,"Missing \"(\" after if statement in",_peek(-1)._src_loc);
        _advance();
    }
    ExprPtr cond = _parseExpression();
    if(!cond){
        err_engine.report_error(ERROR,"No Condition expression for 'if' ",_peek(-1)._src_loc);
        _sync(Zar::TokenTypes::R_PAREN);
    }
    if(_check(Zar::TokenTypes::R_PAREN)){
        _advance();
    }
    else{
        err_engine.report_error(ERROR,"Expected \")\" after expression in",_peek(-1)._src_loc);
        _sync(Zar::TokenTypes::L_CURLY);
    }
    if(_check(Zar::TokenTypes::L_CURLY)){
        _advance();
        thenblock = _parse_block();
    }
    else{
        err_engine.report_error(ERROR,"Expected '{' after conditional expression",_peek(-1)._src_loc);
        _sync(Zar::TokenTypes::R_CURLY);
    }
    std::vector<std::unique_ptr<Zar::IfStmtNode>> elseblocks;
    while(_check(Zar::TokenTypes::ELSE)){
        _advance();
        std::unique_ptr<Zar::IfStmtNode> elsenode;
        if(_check(Zar::TokenTypes::IF)){
            _advance();
            elsenode = _parse_if();
            elseblocks.push_back(std::move(elsenode));
        }
        else if(_check(Zar::TokenTypes::L_CURLY)){
            _advance();
            auto elseblock = _parse_block();
            elsenode = std::make_unique<Zar::IfStmtNode>(nullptr,std::move(elseblock));
            elseblocks.push_back(std::move(elsenode));
        }
        else{
            err_engine.report_error(ERROR,"Expected an 'if' or '{' after else",_peek(-1)._src_loc);
        }
    }
    std::unique_ptr<Zar::IfStmtNode> ifno = std::make_unique<Zar::IfStmtNode>(std::move(cond),std::move(thenblock));
    ifno->elseBlocks = std::move(elseblocks);
    return ifno;
}

Zar::LoopStmtPtr Zar::Parser::_parse_loop(){
    StmtPtr initialzer{nullptr};
    ExprPtr cond{nullptr};
    StmtPtr upexpr{nullptr};
    BlockPtr block;
    _check(Zar::TokenTypes::L_PAREN)? _advance() : err_engine.report_error(ERROR,"Missing \"(\" after loop statement in",_peek(-1)._src_loc);
    initialzer = std::move(_parse_item());
    cond = std::move(_parseExpression());
    _advance();
    upexpr = std::move(_statements());
    _check(Zar::TokenTypes::R_PAREN)? _advance() : err_engine.report_error(ERROR,"Expected \")\" after expression",_peek(-1)._src_loc);
    _check(Zar::TokenTypes::L_CURLY)? _advance() : err_engine.report_error(ERROR,"Expected '{' after conditional expression",_peek(-1)._src_loc);
    block = _parse_block();
    return std::make_unique<LoopStmtNode>(StmtType::STMT_FOR,std::move(initialzer),std::move(cond),std::move(upexpr),std::move(block));
};

Zar::ParamDeclPtr Zar::Parser::_parse_param(){
    std::string name;
    DataType tp;
   if(_check(Zar::TokenTypes::IDENTIFIER)){
       name = _Tok._lexeme;
       _advance();
   }else{
       err_engine.report_error(ERROR,"Expected an identifier" ,_peek(-1)._src_loc);
   }
    _check(Zar::TokenTypes::COLON) ? _advance() : err_engine.report_error(ERROR,"Expected a ':' ",_peek(-1)._src_loc);
    std::unordered_set<TokenTypes> set(type_lookup.begin(),type_lookup.end());
    if(set.count(_peek()._type)){
        tp = _get_datatype(_Tok._type);
    }
    else{
        err_engine.report_error(ERROR,"Expected a type after ':' ",_peek(-1)._src_loc);
    }
    _advance();
    return std::make_unique<ParamDeclNode>(name,tp);
}

Zar::FuncDeclPtr Zar::Parser::_parse_fn(){
    std::string fn_iden;
    if(_check(Zar::TokenTypes::IDENTIFIER)){
        fn_iden = _Tok._lexeme;
        _advance();
    }
    else{
        err_engine.report_error(ERROR,"Expected function name after 'fn' keyword",_peek(-1)._src_loc);
    }
    _check(Zar::TokenTypes::L_PAREN)? _advance() : err_engine.report_error(ERROR,"Missing \"(\" after function name ",_peek(-1)._src_loc);
    std::vector<ParamDeclPtr> params;
    while(_Tok._type!=TokenTypes::R_PAREN && _Tok._type!=TokenTypes::_EOF){
        auto param = _parse_param();
        params.push_back(std::move(param));
        if(_check(Zar::TokenTypes::R_PAREN)){
            continue;
        }
        else if(_check(Zar::TokenTypes::COMMA)){
            _advance();
            if(_check(Zar::TokenTypes::IDENTIFIER)){
                continue;
            }
            else{
                err_engine.report_error(ERROR,"Unnecessary ',' after a parameter",_peek(-1)._src_loc);
            }
        }
        else{
            _advance();
        }
    }
    _check(Zar::TokenTypes::R_PAREN) ? _advance() : err_engine.report_error(ERROR,"Missing \")\" after parameters declarations ",_peek(-1)._src_loc);
    _check(Zar::TokenTypes::COLON) ? _advance() : err_engine.report_error(ERROR,"Expected a ':'",_peek(-1)._src_loc);
    std::unordered_set<TokenTypes> set(type_lookup.begin(),type_lookup.end());
    Zar::DataType rt;
    if(set.count(_peek()._type)){
        rt = _get_datatype(_Tok._type);
    }
    else{
        err_engine.report_error(ERROR,"Expected a return type after ':' ",_peek(-1)._src_loc);
    }
    _advance();
    BlockPtr block;
    if(_check(Zar::TokenTypes::L_CURLY)){
        _advance();
        block = std::move(_parse_block());
    }
    return std::make_unique<FuncDeclNode>(fn_iden,rt,std::move(params),std::move(block));
}

Zar::StmtPtr Zar::Parser::_statements(){
    if(_Tok._type == Zar::TokenTypes::IDENTIFIER){
        auto asgn = _assignment();
        if(_check(Zar::TokenTypes::SEMIC)){
            _advance();
            return std::move(asgn);
        }
        else{
            err_engine.report_error(ERROR,"Expected a ';' ",_peek(-1)._src_loc);
        }
    }
    else if(_Tok._type == Zar::TokenTypes::IF){
        _advance();
        auto ifstmt = _parse_if();
        return std::move(ifstmt);
    }
    else if(_Tok._type == TokenTypes::LOOP){
        _advance();
        auto loopstmt = _parse_loop();
        return std::move(loopstmt);
    }
    return nullptr;
}

Zar::StmtPtr Zar::Parser::_parse_item(){
    switch(_Tok._type){
        case Zar::TokenTypes::IDENTIFIER:
        case Zar::TokenTypes::IF:
        case Zar::TokenTypes::LOOP:
            return _statements();
            break;
        case Zar::TokenTypes::LET:
        case Zar::TokenTypes::FUNC:
            return std::make_unique<DeclStmt>(std::move(_decls()));
            break;
        default:
            return nullptr;
    }
}

Zar::DeclPtr Zar::Parser::_decls(){
    if(_Tok._type == Zar::TokenTypes::LET){
        _advance();
        bool flg = false;
        if(_check(Zar::TokenTypes::MUT)){
            flg = true;
            _advance();
        }
        auto var = _var_decl(flg);
        if(_check(Zar::TokenTypes::SEMIC)){
            _advance();
            return std::move(var);
        }
        else{
            err_engine.report_error(ERROR,"Expected a ';' ",_peek(-1)._src_loc);
        }
    }
    else if(_Tok._type == Zar::TokenTypes::FUNC){
        _advance();
        auto fnptr = _parse_fn();
        return std::move(fnptr);
    }
    return nullptr;
}

Zar::TranslationUnit Zar::Parser::parse(){
    TranslationUnit tunit;
    _advance();
    while(_Tok._type!=Zar::TokenTypes::_EOF){
        auto node = _decls();
        if(!node){
            std::cout << "Null Node" << std::endl;
        }else{
            tunit.unit.push_back(std::move(node));
        }
    }
    return std::move(tunit);
}
