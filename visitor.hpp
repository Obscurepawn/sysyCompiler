// Code Generator and visitor
#ifndef VISITOR_HPP
#define VISITOR_HPP
#include "ast.hpp"
class Visitor {
public:
    virtual ~Visitor(){};
    // Expr
    virtual void visitBinaryOp(NBinaryOp *binary_op) = 0;
    virtual void visitUnaryOp(NUnaryOp *unary_op) = 0;
    virtual void visitIdent(NIdent *ident) = 0;
    virtual void visitFuncCall(NFuncCall *func_call) = 0;
    // Stmt
    virtual void visitBlock(NBlock *block_node) = 0;
    virtual void visitAssign(NAssign *assign) = 0;
    virtual void visitVarDecl(NVarDecl *var_decl) = 0;
    virtual void visitFuncDef(NFuncDef *func_def) = 0;
    virtual void visitIfStmt(NIfStmt *if_stmt) = 0;
    virtual void visitWhileStmt(NWhileStmt *while_stmt) = 0;
    virtual void visitJumpStmt(NJmpStmt *jmp_stmt) = 0;
    virtual void visitRetStmt(NRetStmt *ret_stmt) = 0;
    virtual void visitEmptyStmt(NEmptyStmt *empty_stmt) = 0;
    virtual void visitExprStmt(NExprStmt *expr_stmt) = 0;
    virtual void visitIntValue(NInt *int_val) = 0;
};

#endif // VISITOR_HPP_