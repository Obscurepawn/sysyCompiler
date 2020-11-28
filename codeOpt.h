#ifndef CODE_OPT_H
#define CODE_OPT_H
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
enum token {
    INT = 258,
    VOID,
    IF,
    ELSE,
    WHILE,
    BREAK,
    CONTINUE,
    RETURN,
    EQ,
    NE,
    LE,
    GE,
    AND,
    OR,
    CONST,
    NUMBER,
    IDENT,
    DIMENSION,
    ARRAY_VAL,
    SPACE,
};
int preOpt(std::string file_name);
#endif // CODE_OPT_H