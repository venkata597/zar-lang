#include "../include/error.hpp"

void Zar::ErrorEngine::report_error(ErrorSeverity es,std::string msg,SourceLocation slc){
    errors.push_back({.severity=es,.message=msg,.srcloc=slc});
}

bool Zar::ErrorEngine::hasErrors(){
    bool eflag = false;
    for(auto error: errors){
        if(error.severity==ERROR) eflag = true;
        else continue;
    }
    return eflag;
}

void Zar::ErrorEngine::print_errors(){
    for(auto error: errors){
        std::cout << error.srcloc.file << ":" << error.srcloc._line << ":" << error.srcloc._column << " ";
        switch(error.severity){
            case ERROR:
                std::cout << BOLD << RED << "Error: " << RESET << error.message;
                break;
            case WARNING:
                std::cout << "Warning: " << error.message;
        }
        std::cout << std::endl;
        std::cout << _line_storage.at(error.srcloc._line-1) << std::endl;
        std::string spaces(error.srcloc._column,'-');
        std::cout << spaces;
        std::cout << "^";
        std::cout << std::endl;
    }
}
