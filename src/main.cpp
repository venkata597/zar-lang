#include <iostream>
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/nprettyprinter.hpp"
#include "../include/error.hpp"
#include "../include/codegen.hpp"

int main(int argc,char** argv){
    if(argc<2){
        std::cout << "Check once again" << std::endl;
        return 1;
    }
    Zar::ErrorEngine ee;
    Zar::Lexer lexer(argv[1],ee);
    std::vector<Zar::Token> tkns = lexer.scanFile();
    Zar::Parser parser(tkns,ee);
    auto ast = parser.parse();
    Zar::NPrettyPrinter pp(std::move(ast));
    ast = pp.print_ast();
    if(ee.hasErrors()){
        ee.print_errors();
    }
    Zar::CodeGen cg(std::move(ast));
    cg.generate();
}
