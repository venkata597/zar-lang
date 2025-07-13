#include "../include/codegen.hpp"


// "true" in the register pair represents its free state
std::array<std::pair<std::string,bool>,5> Zar::Codegen::_regs = {
    x86_REGISTER("r8",true),
    x86_REGISTER("r9",true),
    x86_REGISTER("r10",true),
    x86_REGISTER("r11",true),
    x86_REGISTER("r12",true),
};

void Zar::Codegen::_make_asm_file(){
    outfile.open("cg.asm",std::ios::trunc);
}

int Zar::Codegen::_get_free_reg(){
    for(int ir = 0;ir<_regs.size();ir++){
        if(_regs[ir].second == true){
            _regs[ir].second = false;
            return ir;
        }
    }
    std::cout << "No Free Register" << std::endl;
    exit(1);
}

void Zar::Codegen::_free_all_regs(){
    for(int i = 0;i<_regs.size();i++){
        _regs[i].second = true;
    }
}

void Zar::Codegen::_free_reg(int nreg){
    _regs[nreg].second = true;
}

void Zar::Codegen::_pre_asm(){
    outfile << "section .text\n\tglobal _start\n\n_start:\n"; 
}

int Zar::Codegen::_gen_valreg(int val){
    int reg = _get_free_reg();
    outfile << "\tmov " << _regs[reg].first << ", " << val << '\n';
    return reg;
}

int Zar::Codegen::_genadd(int lreg,int rreg){
    outfile << "\tadd " << _regs[lreg].first << ", " << _regs[rreg].first << '\n';
    _free_reg(rreg);
    return lreg;
}

int Zar::Codegen::_gensub(int lreg,int rreg){
    outfile << "\tsub " << _regs[rreg].first << ", " << _regs[lreg].first << "\n";
    _free_reg(lreg);
    return rreg;
}

int Zar::Codegen::_genmult(int lreg,int rreg){
    outfile << "\tmov rax, " << _regs[lreg].first << '\n';
    outfile << "\tmul " << _regs[rreg].first << '\n';
    outfile << "\tmov " << _regs[rreg].first << ", rax" << '\n';
    _free_reg(lreg);
    return rreg;
}

int Zar::Codegen::_gendiv(int lreg,int rreg){
   outfile << "\tmov rax, " << _regs[lreg].first << '\n';
   outfile << "\tcqo" << '\n';
   outfile << "\tidiv " << _regs[rreg].first << '\n';
   outfile << "\tmov " << _regs[rreg].first << ", rax" << '\n';
   _free_reg(lreg);
   return rreg;
}

void Zar::Codegen::_post_asm(int reg){
    outfile << "\tmov rdi, " << _regs[reg].first << '\n';
    outfile << "\tmov rax, 60\n";
    outfile << "\tsyscall\n";
}

int Zar::Codegen::_code_gen(const Zar::exprNode* node){
    int lreg,rreg;
    if(node->left.get()) lreg = _code_gen(node->left.get());
    if(node->right.get()) rreg = _code_gen(node->right.get()); 

    switch(node->ntype){
        case Zar::NodeTypes::N_PLUS:
            return Zar::Codegen::_genadd(lreg,rreg);
        case Zar::NodeTypes::N_MINUS:
            return Zar::Codegen::_gensub(lreg,rreg);
        case Zar::NodeTypes::N_MULT:
            return Zar::Codegen::_genmult(lreg,rreg);
        case Zar::NodeTypes::N_DIV:
            return Zar::Codegen::_gendiv(lreg,rreg);
        case Zar::NodeTypes::N_INT:
            return Zar::Codegen::_gen_valreg(std::stoi(node->value));
        default:
            std::cout << "Unknown AST operator" << '\n';
            exit(1);
    }
}

void Zar::Codegen::genasm(){
    int r;
    _make_asm_file();
    _pre_asm();
    r = _code_gen(tree);
    _post_asm(r);
    outfile.close();
}