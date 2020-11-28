  
%option noyywrap yylineno
%{
    #include <iostream>
    #include <fstream>
    #include <cstdio>
    #include <string>
    #include "codeOpt.h"
    std::string tokenValue = "";
    FlexLexer* lexer;
    std::ifstream in;
    #define SAVE_TOKEN tokenValue = yytext
%}

IDENT      [a-zA-Z][a-zA-Z0-9]*

DECIMAL_CONST    [1-9]*[0-9]+
OCTAL_CONST      0[0-7]*
HEX_CONST        0[xX][0-9a-fA-F]*
INTEGER_CONST {DECIMAL_CONST}|{OCTAL_CONST}|{HEX_CONST}
COMMENTS ("//".*)|("/*".*"*/")
SPC [ \t\n]+

%%
{SPC}           {/*do nothing for space*/}
{COMMENTS}      {/*do nothing for comments*/}


"int"           {SAVE_TOKEN;return INT;}

"void"          {SAVE_TOKEN;return VOID;}

"if"            {SAVE_TOKEN;return IF;}

"else"          {SAVE_TOKEN;return ELSE;}

"while"         {SAVE_TOKEN;return WHILE;}

"break"         {SAVE_TOKEN;return BREAK;}

"continue"      {SAVE_TOKEN;return CONTINUE;}

"return"        {SAVE_TOKEN;return RETURN;}

"=="            {SAVE_TOKEN;return EQ;}

"!="            {SAVE_TOKEN;return NE;}

">="            {SAVE_TOKEN;return GE;}

"<="            {SAVE_TOKEN;return LE;}

"&&"            {SAVE_TOKEN;return AND;}

"||"            {SAVE_TOKEN;return OR;}

"const"         {SAVE_TOKEN;return CONST;}

(("[".*?"]")|("[]"))+  {SAVE_TOKEN;return DIMENSION;}

"{"[^;^\n]*"}"    {SAVE_TOKEN;return ARRAY_VAL;}

{INTEGER_CONST} {SAVE_TOKEN;return NUMBER;}

{IDENT}         {SAVE_TOKEN;return IDENT;}

.               {tokenValue = yytext;return yytext[0];}

<<EOF>>         {return EOF;}
%%

int yyerror(char *text){
    fprintf(stderr,"error:%s\n",text);
    return 0;
}