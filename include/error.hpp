#pragma once
#include "lexer.hpp"
#include <vector>
#include <string>

#define ERR_RED   "\033[31m"
#define ERR_BOLD  "\033[1m"
#define ERR_RESET "\033[0m"


namespace Zar{

    struct SourceLocation;

    enum ErrorSeverity{
        ERROR,
        WARNING,
    };

    struct Error{
        ErrorSeverity severity;
        std::string message;
        SourceLocation srcloc;
    };

    class ErrorEngine{
    private:
        std::vector<Error> errors;
        std::vector<std::string> _line_storage;
    public:

        void report_error(ErrorSeverity es,std::string msg,SourceLocation slc);
        bool hasErrors();
        void print_errors();
        void set_source(std::vector<std::string> ls){
            _line_storage = ls;
        }
        ErrorEngine(){}
    };
}
