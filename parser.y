%{
    #include <vector>
    #include <string>
    #include "genArm.hpp"
    #include "type.hpp"
    #include "hashMap.hpp"
    extern int yylex();
    extern char* yytext;
    extern int yylineno;
    extern int genArrayIndex(NIdent* ident);
    extern bool to_debug;
    extern NBlock* prog_block;
    int loop_depth = 0;
    // 语法分析时符号表
    HashMap* parse_time_symtbl = new HashMap();
    // 循环内声明表
    vector<NVarDecl*> loop_decls;
    void yyerror(const char* s) {
        printf("Error %s: %s at line %d\n", s, yytext, yylineno);
        exit(1);
    }
%}
%union{
    NBlock*                         block;
    NStmt*                          stmt;
    NExpr*                          expr;
    Assignlist*                     assign_list;
    Varlist*                        var_list;
    Exprlist*                       expr_list;
    NAssign*                        assign;
    Type*                           type;
    NIdent*                         ident;
    NArray*                         array;
    NVarDecl*                       vardecl;
    NFuncDef*                       func_def;
    int                             token;
}
%token NUMBER IDENT
%token BREAK CONTINUE
%token EQ NE GE LE
%token IF ELSE WHILE RETURN
%token AND OR
%token CONST INT VOID

%precedence LOW_PREC
%precedence ELSE
%left ',' '['

%%
program: comp_unit {
    prog_block = $<block>1;
}
;
comp_unit: comp_unit decl {
            $<block>1->append($<stmt>2);
            $<block>$ = $<block>1;
            $<vardecl>2->is_global_ = true;
        }
        | comp_unit func_def {
            $<block>1->append($<stmt>2);
            $<block>$ = $<block>1;
        }
        | %empty {
            $<block>$ = new NBlock();
        }
;

decl: const_decl    {$<stmt>$ = $<stmt>1;}
    | var_decl      {$<stmt>$ = $<stmt>1;}
;

const_decl: CONST INT const_def_list ';' {
                $<type>2->is_const_ = true;
                NVarDecl* decl = new NVarDecl($<type>2, $<assign_list>3);
                if(loop_depth > 0) {
                    loop_decls.push_back(decl);
                    decl->is_in_loop_ = true;
                }
                $<stmt>$ = decl;
            }
;

const_def_list: const_def_list ',' const_def {
                    $<assign_list>1->push_back($<assign>3);
                    $<assign_list>$ = $<assign_list>1;
                }
                | const_def {
                    $<assign_list>$ = new vector<NAssign*>();
                    $<assign_list>$->push_back($<assign>1);
                }
;

const_def: declarator_const '=' const_init_val {
                parse_time_symtbl->push($<ident>1);
                $<assign>$ = new NAssign($<ident>1, $<expr>3);
                // 设置ident的值,类型
                $<ident>1->setValue($<expr>3);
            }
;

declarator_const: declarator_const'['const_exp']' {
                    $<ident>$ = $<ident>1;
                    int len = $<expr>3->getValue();
                    TArray* t = dynamic_cast<TArray*>($<ident>$->type_.get());
                    if(t) {
                        TArray* tail = t->getTail();
                        TArray* new_type = new TArray(tail->base_type_, new NInt(len));
                        tail->base_type_ = shared_ptr<Type>(new_type);
                    } else {
                        $<ident>$->setType(new TArray($<ident>$->type_, new NInt(len)));
                    }
                }
                | IDENT {
                    $<ident>$ = $<ident>1;
                    $<ident>$->setType(new TInt());
                }
;

const_init_val: const_exp {
                $<expr>$ = $<expr>1;
            }
            | '{''}'    {
                $<expr>$ = new NArray();
            }
            | '{'const_init_val_list'}' {
                $<expr>$ = $<array>2;
            }
;

const_init_val_list: const_init_val_list ',' const_init_val {
                $<array>1->append($<expr>3);
                $<array>$ = $<array>1;
            }
            | const_init_val {
                $<array>$ = new NArray();
                $<array>$->append($<expr>1);
            }
;

var_decl: INT var_def_list';' {
                NVarDecl* decl = new NVarDecl($<type>1, $<assign_list>2);
                if(loop_depth > 0) {
                    loop_decls.push_back(decl);
                    decl->is_in_loop_ = true;
                }
                $<stmt>$ = decl;
            }
;

var_def_list: var_def_list ',' var_def {
                $<assign_list>1->push_back($<assign>3);
                $<assign_list>$ = $<assign_list>1;
            }
            | var_def {
                $<assign_list>$ = new vector<NAssign*>();
                $<assign_list>$->push_back($<assign>1);
            }
;

var_def: declarator_const {
            parse_time_symtbl->push($<ident>1);
            $<assign>$ = new NAssign($<ident>1);     
        }
        | declarator_const '=' init_val {
            parse_time_symtbl->push($<ident>1);   
            $<assign>$ = new NAssign($<ident>1, $<expr>3);
            $<ident>1->setValue($<expr>3);
        }
;

init_val: exp {
            $<expr>$ = $<expr>1;    
        }
        | '{'init_val_list'}' {
            $<expr>$ = $<array>2;
        }
        | '{''}' {
            $<expr>$ = new NArray();
        }
;

init_val_list: init_val_list ',' init_val {
            $<array>1->append($<expr>3);
            $<array>$ = $<array>1;
        }
        | init_val {
            $<array>$ = new NArray();
            $<array>$->append($<expr>1);
        }
;

func_def: INT IDENT '('func_fparams')' block {
            $<stmt>$ = new NFuncDef($<type>1, $<ident>2, $<var_list>4, $<block>6);
        }
        | INT IDENT '('')' block {
           $<stmt>$ = new NFuncDef($<type>1, $<ident>2, $<block>5);
        }
        | VOID IDENT '('func_fparams')' block {
            $<stmt>$ = new NFuncDef($<type>1, $<ident>2, $<var_list>4, $<block>6);
        }
        | VOID IDENT '('')' block {
            $<stmt>$ = new NFuncDef($<type>1, $<ident>2, $<block>5);
        }
;
func_fparams: func_fparams ',' func_fparam {
            $<var_list>1->push_back($<vardecl>3);
            $<var_list>$ = $<var_list>1;
        }
        | func_fparam {
            $<var_list>$ = new vector<NVarDecl*>();
            $<var_list>$->push_back($<vardecl>1);
        }

func_fparam: INT declarator_func {
            $<vardecl>$ = new NVarDecl($<type>1, $<assign_list>2);
        }
;

declarator_func: declarator_func '['exp']' {
                $<assign_list>$ = $<assign_list>1;
                /* set array type */
                NIdent* id = $<assign_list>1->back()->id_;
                int len = $<expr>3->getValue();
                TArray* t = dynamic_cast<TArray*>(id->type_.get());
                if(t) {
                    TArray* tail = t->getTail();
                    TArray* new_type = new TArray(tail->base_type_, new NInt(len));
                    tail->base_type_ = shared_ptr<Type>(new_type);
                } else {
                    id->setType(new TArray(id->type_, new NInt(len)));
                }
                int depth = id->type_->getDepth();
            }
            | declarator_func '['']' {
                $<assign_list>$ = $<assign_list>1;
                NIdent* id = $<assign_list>1->back()->id_;
                /* 暂时还没管省略维度的参数类型，假设长度为1 */
                id->setType(new TArray(id->type_, new NInt(1)));
                int depth = id->type_->getDepth();
            }
            | IDENT %prec LOW_PREC {
                $<assign_list>$ = new vector<NAssign*>();
                NAssign* a = new NAssign($<ident>1);
                /*set int type*/
                $<ident>1->setType(new TInt());
                $<assign_list>$->push_back(a);
            }
;
block: '{' {
            parse_time_symtbl = new HashMap(parse_time_symtbl);
        }
        block_item_list 
        '}' {
            $<block>$ = $<block>3;
            parse_time_symtbl = parse_time_symtbl->getFather();
            parse_time_symtbl->pop();
        }
;
block_item_list: block_item_list block_item {
                    $<block>1->append($<stmt>2);
                    $<block>$ = $<block>1;
                }
                | block_item {
                    $<block>$ = new NBlock();
                    $<block>$->append($<stmt>1);
                }
;
block_item: decl {
            $<stmt>$ = $<stmt>1;
            }
            | stmt {
            $<stmt>$ = $<stmt>1;
            }
;
stmt: lval '=' exp';' {
        $<stmt>$ = new NAssign($<ident>1, $<expr>3);
        NIdent* origin_id = parse_time_symtbl->findVar($<ident>1->name_.c_str());
        if(origin_id) {
            if(origin_id->type_->isArray()) {
                origin_id->value_ = $<expr>3;
            } else {
                origin_id->value_ = $<expr>3;
            }
        }
    }
    | exp';' {
        /*doubt*/
        $<stmt>$ = new NExprStmt($<expr>1);
    }
    | block {
        $<stmt>$ = $<stmt>1;
    }
    | if_stmt {
        $<stmt>$ = $<stmt>1;
    }
    | while_stmt {
        $<stmt>$ = $<stmt>1;
    }
    | jmp_stmt {
        $<stmt>$ = $<stmt>1;
    }
    | ret_stmt {
        $<stmt>$ = $<stmt>1;
    }
    | ';' {
        $<stmt>$ = new NEmptyStmt(); 
    }
;

if_stmt: IF '(' cond ')' stmt %prec LOW_PREC {
            $<stmt>$ = new NIfStmt($<expr>3, $<block>5, nullptr);
        }
        | IF '(' cond ')' stmt ELSE stmt {
            $<stmt>$ = new NIfStmt($<expr>3, $<block>5, $<block>7);
        }
;
while_stmt: WHILE '(' cond ')' {
                loop_depth ++;
            } stmt {
                NWhileStmt* while_stmt = new NWhileStmt($<expr>3, $<block>6);
                loop_depth --;
                if(loop_depth == 0 && loop_decls.size() > 0) {
                    NBlock* opt_while_stmt = new NBlock();
                    for(int i = 0; i < loop_decls.size(); i++) {
                        opt_while_stmt->append(loop_decls.at(i));
                    }
                    opt_while_stmt->append(while_stmt);
                    loop_decls.resize(0);
                    $<stmt>$ = opt_while_stmt;
                } else {
                    $<stmt>$ = while_stmt;
                }
            }
;

jmp_stmt: BREAK';' {
            $<stmt>$ = new NJmpStmt(AST_BREAK_JMP);
        }
        | CONTINUE';' {
            $<stmt>$ = new NJmpStmt(AST_CONTINUE_JMP);
        }
;

ret_stmt: RETURN exp';' {
            $<stmt>$ = new NRetStmt($<expr>2);
        }
        | RETURN';' {
            $<stmt>$ = new NRetStmt();
        }
;
exp: add_exp {$<expr>$ = $<expr>1;}
;
cond: l_or_exp  {$<expr>$ = $<expr>1;}
;

lval: lval '['exp']' {
        $<ident>1->appendArray($<expr>3);
        $<ident>$ = $<ident>1;
    }
    | IDENT {
        $<ident>$ = $<ident>1;
    }
;

primary_exp: '(' exp ')' {
        $<expr>$ = $<expr>2;
    }
    | lval {
        $<ident>$ = $<ident>1;
    }
    | NUMBER {
        $<expr>$ = $<expr>1;
    }
;
unary_exp: primary_exp {
        $<expr>$ = $<expr>1;
    }
    | IDENT '('')'  {
        $<expr>$ = new NFuncCall($<ident>1, nullptr);
    }
    | IDENT '('func_rparams')' {
        $<expr>$ = new NFuncCall($<ident>1, $<expr_list>3);
    }
    | '+' unary_exp {
        $<expr>$ = new NUnaryOp('+', $<expr>2);
    }
    | '-' unary_exp {
        $<expr>$ = new NUnaryOp('-', $<expr>2);
    }
    | '!' unary_exp {
        $<expr>$ = new NUnaryOp('!', $<expr>2);
    }
;
func_rparams: func_rparams ',' exp {
                $<expr_list>1->push_back($<expr>3);
                $<expr_list>$ = $<expr_list>1;
            }
            | exp {
                $<expr_list>$ = new vector<NExpr*>();
                $<expr_list>$->push_back($<expr>1);
            }
;

mul_exp: mul_exp '*' unary_exp {
        $<expr>$ = new NBinaryOp('*', $<expr>1, $<expr>3);
    }
    | mul_exp '/' unary_exp {
        $<expr>$ = new NBinaryOp('/', $<expr>1, $<expr>3);
    }
    | mul_exp '%' unary_exp {
        $<expr>$ = new NBinaryOp('%', $<expr>1, $<expr>3);
    }
    | unary_exp {
        $<expr>$ = $<expr>1;
    }
;
add_exp: add_exp '+' mul_exp {
        $<expr>$ = new NBinaryOp('+', $<expr>1, $<expr>3);
    }
    | add_exp '-' mul_exp {
        $<expr>$ = new NBinaryOp('-', $<expr>1, $<expr>3);
    }
    | mul_exp {
        $<expr>$ = $<expr>1;
    }
;

rel_exp: add_exp
    | rel_exp rel_op add_exp {
        $<expr>$ = new NBinaryOp($<token>2, $<expr>1, $<expr>3);
    }
;
eq_exp: rel_exp {
        $<expr>$ = $<expr>1;
    }
    | eq_exp eq_op rel_exp {
        $<expr>$ = new NBinaryOp($<token>2, $<expr>1, $<expr>3);
    }
;

l_and_exp: eq_exp {
        $<expr>$ = $<expr>1;
    }
    | l_and_exp AND eq_exp {
        $<expr>$ = new NBinaryOp($<token>2, $<expr>1, $<expr>3);
    }
;

l_or_exp: l_and_exp {
        $<expr>$ = $<expr>1;
    }
    | l_or_exp OR l_and_exp {
        $<expr>$ = new NBinaryOp($<token>2, $<expr>1, $<expr>3);
    }
;

rel_op: LE {
        $<token>$ = $<token>1;
    }
    | GE    {
        $<token>$ = $<token>1;
    }
    | '<' {
        $<token>$ = $<token>1;
    }
    | '>' {
        $<token>$ = $<token>1;
    }
;
eq_op: EQ {
        $<token>$ = $<token>1;
    }
    | NE {
        $<token>$ = $<token>1;
    }
;
const_exp: add_exp {
    $<expr>$ = $<expr>1;
}
%%