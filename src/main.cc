#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include "grammar.tab.hh"

#include "globals.h"
#include "vartable.h"

bool debug_lex = false;
bool debug_grammar = false;
bool debug_interpretation = false;
bool output_dotfile = false;
bool interpret = true;

void yy::parser::error(const std::string& err){
    std::cout << "It's one of the bad ones... " << err << std::endl;
    exit(-1);
}

void parse_flags(int argc, char** argv);
void print_help();
void setup();

enum INPUT_MODES {
	INPUT_STDIN,
	INPUT_FILE,
};

int mode = INPUT_STDIN;
char* filename = NULL;

VarTable* vartable;

int main(int argc, char** argv){
	parse_flags(argc, argv);
	setup();
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
        else {
            std::ofstream outfile("parse.dot");
			root.dumps_dot(outfile);
            outfile.close();
        }
		if (interpret == true){
			root.interpret();
		}
	}
    return 0;
}

void setup(){
	if (interpret == true){
		vartable = new VarTable();
	}
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
				case 'l':
					debug_lex = true;
					break;
				case 'g':
					debug_grammar = true;
					break;
				case 'i':
					debug_interpretation = true;
					break;
				case 'd':
					debug_lex = true;
					debug_grammar = true;
					debug_interpretation = true;
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
	std::endl << "  -s : silent parse" <<
	std::endl << "  -l : debug lex" <<
	std::endl << "  -g : debug grammar" <<
	std::endl << "  -i : debug interpretation" <<
	std::endl << "  -d : debug all" <<
	std::endl;
}
