#include <iostream>
#include <cstdio>
#include "grammar.tab.hh"

extern unsigned int total;
extern Node root;

void yy::parser::error(const std::string& err){
    std::cout << "It's one of the bad ones... " << err << std::endl;
    exit(-1);
}

int main(int argc, char **argv){
    yy::parser parser;
    if (!parser.parse())
        root.dump();
    return 0;
}
