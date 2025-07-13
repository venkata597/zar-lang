#pragma once

#include "namespace.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <array>
#include <utility>
#include <fstream>

#define x86_REGISTER(x,y) std::pair<std::string,bool>(x,y)

namespace Zar{
    class Codegen{
    private:
        const exprNode* tree;
        static std::array<std::pair<std::string,bool>,5> _regs;
        std::ofstream outfile;
        void _make_asm_file();
        int _get_free_reg();
        void _free_all_regs();
        void _free_reg(int nreg);
        void _pre_asm();
        int _gen_valreg(int val);
        int _genadd(int lreg,int rreg);
        int _gensub(int lreg,int rreg);
        int _genmult(int lreg,int rreg);
        int _gendiv(int lreg,int rreg);
        void _post_asm(int reg);
        int _code_gen(const exprNode* node);
    public:
        Codegen(const exprNode* e)
            :tree(e) {}

        void genasm();
    };
}