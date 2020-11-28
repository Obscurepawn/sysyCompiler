#ifndef GEN_MID_HPP
#define GEN_MID_HPP
#include <iostream>
#include <fstream>
#include <assert.h>
#include "ast.hpp"
#include "visitor.hpp"
#include "hashMap.hpp"
using namespace std;



class MidGenerator: public Visitor {
public:
    ~MidGenerator(){};
    void genMidCode(NBlock* root);
    virtual void visit(Node* node) {
        node->accept(this);
    }
    // Expr
    void visitBinaryOp(NBinaryOp *binary_op);//80%
    void visitIdent(NIdent *ident);//useless?
    void visitUnaryOp(NUnaryOp *unary_op);//100%
    void visitFuncCall(NFuncCall *func_call);
    // Stmt
    void visitBlock(NBlock *block_node);//useless?
    void visitAssign(NAssign *assign);//60% 需要跟进版本
    void visitVarDecl(NVarDecl *var_decl);//100% 为所有非全局变量名记录临时变量名 并且为符号表添加功能以满足需求
    void visitFuncDef(NFuncDef *func_def);//0%
    void visitIfStmt(NIfStmt *if_stmt);//20%
    void visitWhileStmt(NWhileStmt *while_stmt);//50%
    void visitJumpStmt(NJmpStmt *jmp_stmt);//100%
    void visitRetStmt(NRetStmt *ret_stmt);//10%没搞懂规则
    void visitEmptyStmt(NEmptyStmt *empty_stmt);//100%
    void visitExprStmt(NExprStmt *expr_stmt);//100%
    void visitIntValue(NInt* int_val);//


    string genMid(NExpr *expr);//100%
    string genIndex(NIdent *arr_id);//0%
    // Condition *genCondition(NExpr *binary_op);//0%
private:
};

#endif