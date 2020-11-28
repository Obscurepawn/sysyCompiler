#ifndef GEN_ARM_HPP
#define GEN_ARM_HPP
#include <iostream>
#include <fstream>
#include <assert.h>
#include "visitor.hpp"
#include "ast.hpp"
using namespace std;

// 记录位置
class Location;
// 记录条件
class Condition;
// AST
class Node;
// Stmt
class NStmt;
class NAssign;
class NVarDecl;
class NFuncDef;
class NIfStmt;
class NWhileStmt;
class NJmpStmt;
class NRetStmt;
class NEmptyStmt;
class NExprStmt;
class NLabelStmt;
// Expr
class NExpr;
class NInt;
class NArray;
class NIdent;
class NBinaryOp;
class NUnaryOp;
class NFuncCall;

extern int cur_fp_pos;

// Rodata 记录只读变量，字符串，数组等
class Rodata {
private:
    int tag_;
    int setTag() {
        static int i = 0;
        return i++;
    }
public:
    NExpr* val_;
    NLabelStmt* labl_;
    Rodata() {
        this->tag_ = setTag();
        this->labl_ = new NLabelStmt();
        this->val_ = nullptr;
    };
    Rodata(NExpr* values) {
        this->tag_ = setTag();
        this->val_ = values;
        this->labl_ = new NLabelStmt();
    }
    ~Rodata(){};
    string getLabel() {
        return this->labl_->getTag();
    }
    string getTag() {
        return ".LC" + to_string(this->tag_);
    }
};
// Location,记录变量位置
class Location {
private:
    // 标记
    bool isInMemory_;
    bool isInReg_;
    bool isInLabel_;
    bool isInRodata_;
public:
    // 在寄存器
    string reg_;
    // 在栈空间
    int sp_pos_;
    // 在某label下
    NLabelStmt* label_;
    Rodata* rodata_;
    Location() {
        this->label_ = nullptr;
        this->isInMemory_ = false;
        this->isInReg_ = false;
        this->isInLabel_ = false;
        this->isInRodata_ = false;
        this->rodata_ = nullptr;
    }
    Location(int sp_pos) {
        this->setMemLoc(sp_pos);
        this->isInMemory_ = true;
        this->isInRodata_ = false;
        this->isInReg_ = false;
        this->isInLabel_ = false;
        this->label_ = nullptr;
        this->rodata_ = nullptr;
    }
    Location(NLabelStmt* label) {
        this->label_ =  label;
        this->isInLabel_ = true;
        this->isInRodata_ = false;
        this->isInReg_ = false;
        this->isInMemory_ = false;
        this->rodata_ = nullptr;
    }
    Location(Rodata* rodata) {
        this->rodata_ = rodata;
        this->isInRodata_ = true;
        this->isInMemory_ = false;
        this->isInReg_ = false;
        this->isInLabel_ = false;
        this->label_ = nullptr;
    }
    Location(string reg) {
        this->reg_ = reg;
        this->isInRodata_ = false;
        this->isInMemory_ = false;
        this->isInReg_ = true;
        this->isInLabel_ = false;
        this->label_ = nullptr;
    }
    string getLocation() {
        // assert(isInMemory_ || isInReg_ || isInLabel_ || isInRodata_);
        if(isInMemory_) {
            // 在栈空间中
            int off = this->sp_pos_ - cur_fp_pos;
            if(off == 0)
                return "[fp]";
            return "[fp, #" + to_string(off) + "]";
        } else if(isInLabel_) {
            // 在Label下
            assert(this->label_);
            return this->label_->getTag();
        }
        return this->reg_;
    }
    string getLocation(int index) {
        // assert(isInMemory_);
        int off = this->sp_pos_ - cur_fp_pos + index * 4;
        if(off == 0)
            return "[fp]";
        return "[fp, #" + to_string(off) + "]";
    }
    int getMemOff() {
        return this->sp_pos_ - cur_fp_pos;
    }
    // setters
    void setRegLoc(string reg_name) {
        this->reg_ = reg_name;
        this->isInReg_ = true;
    };

    void setMemLoc(int sp_pos) {
        this->sp_pos_ = sp_pos;
        this->isInMemory_ = true;
    }
    void setLabel(NLabelStmt* label) {
        this->label_ = label;
        this->isInLabel_ = true;
    }
    void setRodata(Rodata* rodata) {
        this->rodata_ = rodata;
        this->isInRodata_ = true;
    }
    // juders
    bool isInMemory() {
        return this->isInMemory_;
    }
    bool isInReg() {
        return this->isInReg_;
    }
    bool isInLabel() {
        return this->isInLabel_;
    }
    bool isInRodata() {
        return this->isInRodata_;
    }
};

// 代码生成器大哥
class Generator: public Visitor {
public:
    ~Generator(){};
    Generator(string file_name) {
        this->setOutFile(file_name);
    }
    void genArmCode(NBlock* root);
    virtual void visit(Node* node) {
        node->accept(this);
    }
    // Expr
    virtual void visitBinaryOp(NBinaryOp *binary_op);
    virtual void visitIdent(NIdent *ident);
    virtual void visitUnaryOp(NUnaryOp *unary_op);
    virtual void visitFuncCall(NFuncCall *func_call);
    // Stmt
    virtual void visitBlock(NBlock *block_node);
    virtual void visitAssign(NAssign *assign);
    virtual void visitVarDecl(NVarDecl *var_decl);
    virtual void visitFuncDef(NFuncDef *func_def);
    virtual void visitIfStmt(NIfStmt *if_stmt);
    virtual void visitWhileStmt(NWhileStmt *while_stmt);
    virtual void visitJumpStmt(NJmpStmt *jmp_stmt);
    virtual void visitRetStmt(NRetStmt *ret_stmt);
    virtual void visitEmptyStmt(NEmptyStmt *empty_stmt);
    virtual void visitExprStmt(NExprStmt *expr_stmt);
    virtual void visitIntValue(NInt* int_val);
    // 设置输出文件
    void setOutFile(string file_name_) {
        this->out_file_name_ = file_name_;
        this->out_file_ptr_ = fopen(file_name_.c_str(), "wb");
        if(this->out_file_ptr_ == nullptr) {
            fprintf(stderr, "Fail to write file %s\n", file_name_.c_str());
            exit(EXIT_FAILURE);
        }
    }
    // 输出汇编代码到目标文件
    void Emit(string inst);
    // example: bx lr
    void Emit(string op_, string des);
    // example: ldr r0, [r1, #4]
    void Emit(string op_, string des, string src);
    void Emit(string op_, string des, int off);
    // example: ldr r0, [r1, #-4]!
    void Emit(string op_, string des, string src, bool src_change);
    // example: ldr r0, r1, #4
    void Emit(string op_, string des, string src, string second_op);
    void Emit(string op_, string des, string src, int second_op); 
    // emit label like .L1 .L2 main:
    void EmitLabel(string label_);
    // 代码生成
    void genGlobalDecl(NVarDecl *var_decl);
    string genExpr(NExpr* expr);
    // 数组操作
    vector<int> genArrayValue(NExpr *array);
    // 获得下标
    int genVarArrayIndex(NIdent *id);
    // 数组载入内存
    void loadArrayIntoMemory(NAssign* assign);
    // Exprlist线性化
    Exprlist linify(NArray *list);
    // 提出循环中的所有变量声明
    vector<NVarDecl*> getAllVarDeclInLoop(NWhileStmt *while_stmt);
private:
    string out_file_name_;
    FILE *out_file_ptr_;
};


#endif