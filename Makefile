#decls
SRC := ast.cpp lexer.cpp parser.cpp main.cpp genArm.cpp
CC = clang++
CFLAGS = -std=c++11
STD_CFLAGS = -std=c++17

parser:parser.y
	bison -d -o parser.cpp parser.y

lexer:lexer.l
	flex -o lexer.cpp lexer.l

lexer.cpp: lexer.l
	$(MAKE) parser

parser.cpp: parser.y
	$(MAKE) lexer
	
compiler:parser.y lexer.l ${SRC} *.hpp
	$(MAKE) parser
	$(MAKE) lexer
	$(CC) $(CFLAGS) ${SRC} -o compiler

std-compiler:
	clang++ -std=c++17 -O2 -lm *.cpp -o compiler
	
.PHONY:parser lexer clean

test:compiler test.cpp ast.cpp
	./compiler < test.cpp >ast.txt	
clean:
	rm parser.cpp lexer.cpp parser.hpp