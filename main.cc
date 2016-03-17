#include <iostream>
#include <sstream>
#include <cstdio>
#include "grammar.tab.hh"

#include "globals.h"

bool debug_lex = false;
bool debug_grammar = false;

void yy::parser::error(const std::string& err){
    std::cout << "It's one of the bad ones... " << err << std::endl;
    exit(-1);
}

int main(int argc, char **argv){
	yy::parser parser;
	if (!parser.parse()){
		if (debug_grammar == true){
			std::stringstream ss;
			root.dump(ss);
			std::cout << ss.str();
		}
	}
    return 0;
}
