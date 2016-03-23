NAME=lua
CPPFLAGS=-g --std=c++11

SRC=main.cc node.cc vartable.cc

# Link & compile
parser: lex.yy.c grammar.tab.o main.cc node.cc
	g++ $(CPPFLAGS) -o $(NAME) $(SRC) grammar.tab.o lex.yy.c

# Grammar
grammar.tab.o: grammar.tab.cc
	g++ $(CPPFLAGS) -c grammar.tab.cc
grammar.tab.cc: grammar.yy
	bison grammar.yy

# Lexing
lex.yy.c: lex.ll grammar.tab.cc
	flex lex.ll

# Clean
clean:
	rm $(NAME) grammar.tab.* lex.yy.c* stack.hh
