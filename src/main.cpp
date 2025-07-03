#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/ast.hpp"

int main(int argc,char** argv){
    if(argc<2){
        std::cout << "Check once again" << std::endl;
        return 1;
    }
    Zar::Lexer lexer(argv[1]);
    std::vector<Zar::Token> tkns = lexer.scanFile();
    for(auto i:tkns){
        std::cout << static_cast<int>(i._type) << " " << "Lexeme: " << i._lexeme << " Line: " 
            << i._line << '\n';
    }

    Zar::Parser parser(tkns);
    std::unique_ptr<Zar::exprNode> ast = parser.get_ast();
    Zar::PrettyPrinter printer(ast.get());
    printer.print_ast();
}