#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Zar{

    class Visitor;
    class ExprVisitor;
    class StmtVisitor;
    class DeclVisitor;

    struct Symbol;
    class astNode;
    class Expr;
    class binexprNode;
    class literalNode;
    class Stmt;
    class BlockStmtNode;
    class VarStmtNode;
    class AssignStmtNode;
    class IfStmtNode;
    class LoopStmtNode;
    class Decl;
    class VarDeclNode;
    class FuncDeclNode;
    class ExternFuncDeclNode;
    class ParamDeclNode;
    class ExprStmt;
    class DeclStmt;

    class Block;

    using astPtr = std::unique_ptr<astNode>;
    using binexprPtr = std::unique_ptr<binexprNode>;
    using BlockStmtPtr = std::unique_ptr<BlockStmtNode>;
    using VarStmtPtr = std::unique_ptr<VarStmtNode>;
    using AssignStmtPtr = std::unique_ptr<AssignStmtNode>;
    using IfStmtPtr = std::unique_ptr<IfStmtNode>;
    using LoopStmtPtr = std::unique_ptr<LoopStmtNode>;
    using ExprPtr = std::unique_ptr<Expr>;
    using StmtPtr = std::unique_ptr<Stmt>;
    using DeclPtr = std::unique_ptr<Decl>;
    using VarDeclPtr = std::unique_ptr<VarDeclNode>;
    using FuncDeclPtr = std::unique_ptr<FuncDeclNode>;
    using ExternFuncDeclPtr = std::unique_ptr<ExternFuncDeclNode>;
    using ParamDeclPtr = std::unique_ptr<ParamDeclNode>;
    using ExprStmtPtr = std::unique_ptr<ExprStmt>;
    using DeclStmtPtr = std::unique_ptr<DeclStmt>;

    using BlockPtr = std::unique_ptr<Block>;

    enum class BinaryOp{
      ADD,
      SUB,
      MULT,
      DIV,
      EQ,
      NEQ,
      GT,
      LT,
      GTE,
      LTE,
      TERM,
    };

    enum class ExprType{
        EXPR_LITRL,
        EXPR_IDEN_LITRL,
        EXPR_BIN,
        EXPR_CALL,
        EXPR_UNARY,
    };

    enum class StmtType{
        STMT_EXPR,
        STMT_DECL,
        STMT_BLOCK,
        STMT_VARDECL,
        STMT_ASSIGN,
        STMT_CONDITIONAL,
        STMT_FOR,
        STMT_WHILE,
    };

    enum class DeclType{
        DECL_VAR,
        DECL_PARAM,
        DECL_FUNC,
        DECL_EXTERN,
    };

    enum class DataType{
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_LONG,
        TYPE_SHORT,
        TYPE_CHAR,
        TYPE_STRING,
        TYPE_VOID,
        TYPE_USER_DEFINED,
    };

    struct Symbol{
        std::string identifier;
        DataType type;
        Expr* value;
        bool isMut;
        Symbol(std::string i,DataType t,Expr* v,bool fl){
            identifier = i;
            type = t;
            value = v;
            isMut = fl;
        }
    };

    class astNode{
    public:
        virtual ~astNode() = default;
    };

    class Expr: public astNode{
    public:
        ExprType type;
        Expr(ExprType ty):
            type(ty) {}
        virtual ~Expr() = default;
        virtual void accept(ExprVisitor& v) = 0;
    };

    class Stmt: public astNode{
    public:
        StmtType type;
        Stmt(StmtType ty):
            type(ty) {}
        virtual ~Stmt() = default;
        virtual void accept(StmtVisitor& v) = 0;
    };

    class Decl: public astNode{
    public:
        DeclType type;
        Decl(DeclType dt):
            type(dt) {}
        virtual ~Decl() = default;
        virtual void accept(DeclVisitor& v) = 0;
    };

    class Block: public Stmt{
    public:
        std::vector<StmtPtr> block;
        Block(std::vector<StmtPtr> bvect):
            Stmt(StmtType::STMT_BLOCK),block(std::move(bvect)) {}
        void accept(StmtVisitor& v) override;
    };

    class binexprNode: public Expr{
    public:
        BinaryOp Op;
        ExprPtr left;
        ExprPtr right;
        std::string value;

        binexprNode(BinaryOp opr,ExprPtr l,ExprPtr r,std::string v):
            Expr(ExprType::EXPR_BIN),Op(opr),left(std::move(l)),value(v),right(std::move(r)){}

        binexprNode(BinaryOp opr,std::string v):
            Expr(ExprType::EXPR_BIN),Op(opr),value(v),left(nullptr),right(nullptr) {}

        binexprNode(BinaryOp opr,ExprPtr l,std::string v):
            Expr(ExprType::EXPR_BIN),Op(opr),left(std::move(l)),right(nullptr),value(v) {}

        void accept(ExprVisitor& v) override;
    };

    class literalNode: public Expr{
    public:
        std::string value;
        literalNode(std::string v,ExprType typ):
            Expr(typ),value(std::move(v)) {}
        void accept(ExprVisitor& v) override;
    };

    class ExprStmt: public Stmt{
    public:
        ExprPtr expr;
        ExprStmt(ExprPtr e):
            Stmt(StmtType::STMT_EXPR),expr(std::move(e)){}
        void accept(StmtVisitor& v) override;
    };

    class DeclStmt: public Stmt{
    public:
        DeclPtr decl;
        DeclStmt(DeclPtr d):
            Stmt(StmtType::STMT_DECL),decl(std::move(d)){}
        void accept(StmtVisitor& v) override;
    };

    class VarDeclNode: public Decl{
    public:
        std::string iden;
        ExprPtr expression;
        Zar::DataType data_type;
        bool mutble;
        VarDeclNode(std::string ident,DataType dt,bool m,ExprPtr expr = nullptr):
            Decl(DeclType::DECL_VAR),iden(ident),data_type(dt),expression(std::move(expr)),mutble(m){}
        void accept(DeclVisitor& v) override;
    };

    class AssignStmtNode: public Stmt{
    public:
        std::string iden;
        ExprPtr expression;
        AssignStmtNode(std::string ident,ExprPtr expr):
            Stmt(StmtType::STMT_ASSIGN),iden(ident),expression(std::move(expr)) {}

        void accept(StmtVisitor& v) override;
    };

    class IfStmtNode: public Stmt{
    public:
        ExprPtr condition;
        BlockPtr thenblock;
        std::vector<IfStmtPtr> elseBlocks;
        IfStmtNode(ExprPtr cond,BlockPtr tblock):
            Stmt(StmtType::STMT_CONDITIONAL),condition(std::move(cond)),thenblock(std::move(tblock)){}

        void accept(StmtVisitor& v) override;
    };

    class LoopStmtNode: public Stmt{
    public:
        StmtPtr initializer;
        ExprPtr condition;
        StmtPtr update_expr;
        BlockPtr body;
        LoopStmtNode(StmtType ltype,StmtPtr init,ExprPtr cond,StmtPtr uexp,BlockPtr block):
            Stmt(ltype),initializer(std::move(init)),condition(std::move(cond)),update_expr(std::move(uexp)),body(std::move(block)) {}
        void accept(StmtVisitor& v) override;
    };


    class FuncDeclNode: public Decl{
    public:
        std::string name;
        DataType return_type;
        BlockPtr body;
        std::vector<ParamDeclPtr> params;
        FuncDeclNode(std::string nm,DataType rt,std::vector<ParamDeclPtr> pvect,BlockPtr fbdy):
            Decl(DeclType::DECL_FUNC),name(nm),return_type(rt),params(std::move(pvect)),body(std::move(fbdy)) {}
        void accept(DeclVisitor& v) override;
    };

    class ExternFuncDeclNode: public Decl{
    public:
        std::string name;
        DataType return_type;
        std::vector<ParamDeclPtr> params;
        // No Body for extern function declaration
        ExternFuncDeclNode(std::string nme,DataType rt,std::vector<ParamDeclPtr> pvect):
            Decl(DeclType::DECL_EXTERN),name(nme),return_type(rt),params(std::move(pvect)) {}
        void accept(DeclVisitor& v) override;
    };

    class ParamDeclNode: public Decl{
    public:
        std::string name;
        DataType type;
        ParamDeclNode(std::string n,DataType tp):
            Decl(DeclType::DECL_PARAM),name(n),type(tp) {}
        void accept(DeclVisitor& v) override;
    };
}
