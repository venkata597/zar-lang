#pragma once

#include "lexer.hpp"
#include "ast.hpp"
#include <vector>
#include <unordered_map>

namespace Zar{

    class ErrorEngine;

    extern std::vector<int> prec_table;
    extern std::unordered_map<Zar::TokenTypes,Zar::BinaryOp> Opref;
    extern std::unordered_map<Zar::TokenTypes,Zar::DataType> type_ref;

    typedef struct Scope{
        Scope* parent;
        std::unordered_map<std::string,Symbol> table;
    }Scope;


    typedef struct{
        std::vector<DeclPtr> unit;
    }TranslationUnit;

    class Parser{
    private:
        std::vector<Zar::Token> _tkns;
        Zar::Token _Tok;
        int _token_track{-1};
        std::vector<Symbol> _Glob_Sym_Tbl;

        ErrorEngine& err_engine;

        void _advance();
        void _sync(TokenTypes tkn);
        Token _peek(int offset);
        bool _check(TokenTypes expected);
        void _report_error(std::string msg);
        ExprPtr _parse_primary();
        ExprPtr _parse_expression(int pre);
        BinaryOp _get_optype(TokenTypes tktp);
        DataType _get_datatype(TokenTypes tktp);
        int _get_precedence(const Zar::Token tk);

        // Parsing Functions
        ExprPtr _parseExpression();
        VarDeclPtr _var_decl(bool mut_flag);
        AssignStmtPtr _assignment();
        IfStmtPtr _parse_if();
        LoopStmtPtr _parse_loop(bool wloop);
        ParamDeclPtr _parse_param();
        FuncDeclPtr _parse_fn();
        BlockStmtPtr _blockstmts();
        BlockPtr _parse_block();
        StmtPtr _statements();
        DeclPtr _decls();
        StmtPtr _parse_item();

    public:
        Parser(std::vector<Zar::Token> tk_vct,ErrorEngine& e):
            _tkns(tk_vct),err_engine(e){}

        TranslationUnit parse();
    };
}
