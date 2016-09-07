NAME=lua
CPPFLAGS=-g --std=c++11 -I./src -I./build

SRC=src/main.cc src/node.cc src/vartable.cc src/interpretation.cc

# Link & compile
parser: build/lex.yy.c build/grammar.tab.o src/main.cc src/node.cc
	g++ $(CPPFLAGS) -o $(NAME) $(SRC) build/grammar.tab.o build/lex.yy.c

# Grammar
build/grammar.tab.o: build/grammar.tab.cc prepare
	g++ $(CPPFLAGS) -c build/grammar.tab.cc -o $@
build/grammar.tab.cc: src/grammar.yy prepare
	bison src/grammar.yy -o $@

# Lexing
build/lex.yy.c: src/lex.ll build/grammar.tab.cc prepare
	flex -o $@ src/lex.ll

# Clean
.PHONY: clean
clean:
	rm -f $(NAME)
	rm -f parse.dot
	rm -rf ./build

# Prepare
.PHONY: prepare
prepare:
	mkdir -p build
