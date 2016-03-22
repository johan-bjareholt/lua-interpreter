#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include "grammar.tab.hh"

#include "globals.h"

bool debug_lex = false;
bool debug_grammar = false;
bool output_dotfile = false;
bool interpret = true;

void yy::parser::error(const std::string& err){
    std::cout << "It's one of the bad ones... " << err << std::endl;
    exit(-1);
}

void parse_flags(int argc, char** argv);
void print_help();

enum INPUT_MODES {
	INPUT_STDIN,
	INPUT_FILE,
};

int mode = INPUT_STDIN;
char* filename = NULL;

int main(int argc, char** argv){
	parse_flags(argc, argv);
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
		if (interpret == true){
			root.interpret();
		}
	}
    return 0;
}


void parse_flags(int argc, char** argv){
	for (int i=1; i<argc; i++){
		int len = strlen(argv[i]);
		if (len >= 2 && argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'e':
					output_dotfile = true;
					interpret = false;
					break;
				case 's':
					output_dotfile = false;
					debug_lex = false;
					debug_grammar = false;
					interpret = false;
					break;
				case 'i':
					break;
				case 'd':
					debug_lex = true;
					debug_grammar = true;
					break;
				case 'h':
					print_help();
					exit(0);
				INVALID_FLAG:
				default:
					std::cout << "Invalid flag" << std::endl
							  << "use the -h flag for help" << std::endl;
					exit(0);
					break;
			}
		}
		else {
			if (filename == NULL){
				filename = argv[i];
				mode = INPUT_FILE;
				set_input_file(filename);
			}
			else {
				std::cout << "Cannot have two files as input, exiting" << std::endl;
				exit(0);
			}
		}
	}
}

void print_help(){
	std::cout << "Name: lua-interpreter" <<
	std::endl << "Author: Johan Bjäreholt" <<
	std::endl << "Synopsis: lua [flag] [inputfile]" <<
	std::endl << "Flags:" <<
	std::endl << "  -e : output dotformat" <<
	std::endl << "  -i : interpret code (default)" <<
	std::endl << "  -s : silent" <<
	std::endl << "  -d : debug lex and grammar" <<
	std::endl;
}
