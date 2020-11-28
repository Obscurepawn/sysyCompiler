%{
    #include <iostream>
    #include <string>
    #include <algorithm>
    #include <unordered_map>
    #include "genArm.hpp"
    #include "hashMap.hpp"
    #include "parser.hpp"
    using namespace std;
    extern HashMap* symtbl;
    extern NBlock* prog_block;
    unordered_map<char, int> digit = {
        {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
        {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
        {'8', 8}, {'9', 9}, {'a', 10}, {'b', 11},
        {'c', 12}, {'d',13}, {'e', 14}, {'f', 15}
    };
    long long my_atoi(char* val) {
        int base = 10;
        string s = string(val);
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        bool is_neg = false;
        long long ret = 0;
        int i = 0;
        // 跳过非数字字符, 除了-号需要处理
        for(i = 0; i < s.length() && !isdigit(s[i]); i++) {
            if (s[i] == '-') {
                is_neg = !is_neg;
            }
        }
        if(s[i] == '0') {
            char nextc = i + 1 < s.length()? s[i + 1]: -1;
            base = nextc == 'x'? 16: 8;
        }
        while(i < s.length()) {
            ret = ret * base + digit[s[i]];
            i++;
        }
        return is_neg? -ret: ret;
    }
%}
%option yylineno
%option noyywrap

IDENT      [_a-zA-Z][a-zA-Z0-9_]*

DECIMAL_CONST    [1-9]*[0-9]+
OCTAL_CONST      0[0-7]*
HEX_CONST        0[xX][0-9a-fA-F]*
INTEGER_CONST {DECIMAL_CONST}|{OCTAL_CONST}|{HEX_CONST}

SPC [ \t\n]+
%%
{SPC} {/*do nothing for space*/}
"//".* {
    ;
}

"/*"    {
    int c;
    while( ( c = yyinput() ) != 0 ) {
        if(c == '\n') {
            ++ yylineno;
        } else if (c == '*') {
            c = yyinput();
            if( c == '/')
                break;
            else
                unput(c);
        }
    }
}

"int"  {
    yylval.type = new TInt();
    return INT;
}

"void" {
    yylval.type = new TVoid();
    return VOID;
}

"if"    {
    return IF;
}

"else" {
    return ELSE;
}

"while" {
    return WHILE;
}

"break" {
    return BREAK;
}

"continue" {
    return CONTINUE;
}

"return" {
    return RETURN;
}

"==" {
    yylval.token = EQ;
    return EQ;
}

"!=" {
    yylval.token = NE;
    return NE;
}

">=" {
    yylval.token = GE;
    return GE;
}

"<=" {
    yylval.token = LE;
    return LE;
}

"&&" {
    yylval.token =  AND;
    return AND;
}

"||" {
    yylval.token = OR;
    return OR;
}
"const" {
    return CONST;
}
{INTEGER_CONST} {
    yylval.expr = new NInt(my_atoi(yytext));
    return NUMBER;
}

{IDENT} {
    string name = string(yytext);
    yylval.ident = new NIdent(name);
    return IDENT;
}

. {
    yylval.token = yytext[0];
    return yytext[0];
}

<<EOF>> {
    return EOF;
}
%%