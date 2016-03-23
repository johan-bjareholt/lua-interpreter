#pragma once
#include "node.h"
#include "vartable.h"

extern void set_input_file(char*);

extern unsigned int total;
extern Node root;
extern int linenr;

extern bool debug_lex;
extern bool debug_grammar;
extern bool debug_interpretation;

extern VarTable* vartable;
