#include <iostream>
#include <sstream>
#include <cstdio>
#include "grammar.tab.hh"

#include "globals.h"

bool debug_lex = false;
bool debug_grammar = false;
bool output_dotfile = true;

void yy::parser::error(const std::string& err){
    std::cout << "It's one of the bad ones... " << err << std::endl;
    exit(-1);
}

int main(int argc, char **argv){
	yy::parser parser;
	if (!parser.parse()){
		std::stringstream ss;
		if (debug_grammar == true){
			root.dumps_str(ss);
			std::cout << ss.str();
			ss.clear();
		}
		if (output_dotfile == true){
			root.dumps_dot(ss);
			std::cout << ss.str();
			ss.clear();
		}
	}
    return 0;
}
