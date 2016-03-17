NAME=lua

# Link & compile
parser: lex.yy.c grammar.tab.o main.cc node.cc
	g++ -g -o $(NAME) grammar.tab.o lex.yy.c main.cc node.cc

# Grammar
grannar.tab.o: grammar.tab.cc
	g++ -g -c grammar.tab.cc
grammar.tab.cc: grammar.yy
	bison grammar.yy

# Lexing
lex.yy.c: lex.ll grammar.tab.cc
	flex lex.ll

# Clean
clean:
	rm $(NAME) grammar.tab.* lex.yy.c* stack.hh
