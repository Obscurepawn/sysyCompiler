#ifndef ASH_HPP
#define ASH_HPP
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <assert.h>
#include <ctype.h>
#include "type.hpp"
using namespace std;

#define SYS_INT 0x1     // int
#define SYS_VOID 0x2    // void
#define SYS_PTR 0x4     //for array

#define AST_BREAK_JMP   0x02
#define AST_CONTINUE_JMP 0x03

#define TYPE_INT 0x4
#define TYPE_VOID 0x8

// Location
class Location;
// Visitor
class Visitor;
class Generator;
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

typedef vector<NExpr*>   Exprlist;
typedef vector<NStmt*>   Stmtlist;
typedef vector<NAssign*> Assignlist;
typedef vector<NVarDecl*> Varlist;


class Node {
public:
    virtual ~Node(){};
    // visit mode
    virtual void accept(Visitor* visitor) = 0;
    // print ast
    virtual void print() = 0;
};

class NExpr: public Node {
public:
    Location* loc_;
    virtual bool canGetValue() = 0;
    virtual int getValue() = 0;
    void accept(Visitor *v);
    void print() {
        printf("Expr\n");
    }
};

class NStmt: public Node {
public:
    void print() {
        printf("Stmt\n");
    }
    void accept(Visitor *v);
};

// 表示INT类型的值而非类型，比如1,2,3,0xff,077等
class NInt: public NExpr {
public:
   NInt(long long value): value_(value){
       this->loc_ = nullptr;
   };
   void print();
    bool canGetValue() {
        return true;
    }
    int getValue() {
        return this->value_;
    }
    void accept(Visitor* v);
private:
    long long value_;
};

// 表示数组型的值而非类型,比如{1,2,3,4,5}, {1,{1,2,3},{4,5}}
class NArray: public NExpr {
public:
    Exprlist    value_;
    NArray(){
        this->loc_ = nullptr;
    };
    void append(NExpr* value) {
        value_.push_back(value);
    }
    bool canGetValue() {
        // if(this->value_.size() == 0) {
        //     return false;
        // }
        // for(int i = 0; i < this->value_.size(); i++) {
        //     if(!this->value_.at(i)) {
        //         return false;
        //     }
        //     if(!this->value_.at(i)->canGetValue()) {
        //         return false;
        //     }
        // }
        // return true;
        return false;
    }
    void print();
    // Todo 如何实现多维数组的线性化？？？？？
    vector<int> toVector() {
        vector<int> v;
        vector<int> tmp;
        for(int i = 0; i < this->value_.size(); i++) {
            // 转化成array类
            NArray* arr = dynamic_cast<NArray*>(this->value_.at(i));
            if(!arr) {
                // 转换失败，说明不是子数组，直接获取值
                if(this->value_.at(i) && this->value_.at(i)->canGetValue()) {
                    v.push_back(this->value_.at(i)->getValue());
                }
            } else {
                // 深度优先，转换成vector
                tmp = arr->toVector();
                for(int i = 0; i < tmp.size(); i++) {
                    v.push_back(tmp.at(i));
                }
                // 清空tmp
                tmp.resize(0);
            }
        }
        return v;
    }
    int getValue() {
        // wait to fix
        return 0;
    }
};

// identifiers
class NIdent: public NExpr {
    friend class Generator;
    friend class MidGenerator;
public:
    string name_;
    // 中间代码替换后临时名称
    string midname_;
    shared_ptr<Type> type_;
    NExpr* value_;
    bool is_addr;
    /**
     *  below is used to represent array_size
     *  for example, if we have an a[1][2]
     *  then the array_index_ of a is {Nexpr(1), NExpr(2)};
     *  for a[][2];
     *  then the array_index_ of a maybe {nullptr, NExpr(2)};
     * */
    Exprlist* array_index_;

    NIdent(string name) {
        this->name_ = name;
        this->is_addr = false;
        this->array_index_ = nullptr;
        this->type_ = nullptr;
        this->value_ = nullptr;
        this->loc_ = nullptr;
    };
    // for example if an ident a appendArray(2) it's a[2]
    void appendArray(NExpr* exp) {
        if(!this->array_index_) {
            this->array_index_ = new Exprlist();
        }
        this->array_index_->push_back(exp);
    }
    // set the type of the ident
    void setType(Type* tp) {
        this->type_ = shared_ptr<Type>(tp);
    }

    void setValue(NExpr* value) {
        this->value_ = value;
    }
    // print ast
    void print();

    bool canGetValue();
    int getValue();
    void accept(Visitor* v);
};


class NBinaryOp: public NExpr {
    friend class Generator;
    friend class MidGenerator;
private:
    enum yytokentype {
        NUMBER = 258,
        IDENT = 259,
        BREAK = 260,
        CONTINUE = 261,
        EQ = 262,
        NE = 263,
        GE = 264,
        LE = 265,
        IF = 266,
        ELSE = 267,
        WHILE = 268,
        RETURN = 269,
        AND = 270,
        OR = 271,
        CONST = 272,
        INT = 273,
        VOID = 274,
        LOW_PREC = 275
    };
    string parseOp() {
        switch (this->op_) {
            case LE: return "<=";
            case GE: return ">=";
            case EQ: return "==";
            case NE: return "!=";
            case OR: return "||";
            case AND: return "&&";
            default: return string(1, this->op_);
        }
    }


public:
    int op_;
    NExpr*  lhs_;
    NExpr*  rhs_;
    NBinaryOp(int op, NExpr* lhs, NExpr* rhs) {
        this->op_ = op;
        this->lhs_ = lhs;
        this->rhs_ = rhs;
    };
    void print();
    bool canGetValue() {
        return lhs_->canGetValue() && rhs_->canGetValue();
    }
    int getValue() {
#define L this->lhs_->getValue()
#define R this->rhs_->getValue()
        switch(this->op_) {
            case '+': return L + R;
            case '-': return L - R;
            case '*': return L * R;
            case '/': return L / R;
            case '%': return L % R;
            case '<': return L < R;
            case '>': return L > R;
            case LE: return L <= R;
            case GE: return L >= R;
            case EQ: return L == R;
            case NE: return L != R;
            case OR: return L || R;
            case AND: return L && R;
        }
        return 0;
#undef L
#undef R
    }
    void accept(Visitor* v);
};

class NUnaryOp: public NExpr {
    friend class Generator;
    friend class MidGenerator;
public:
    int op_;
    NExpr* uhs_;
    NUnaryOp(int op, NExpr* uhs) {
        this->op_ = op;
        this->uhs_ = uhs;
        this->loc_ = nullptr;
    };

    void print();

    bool canGetValue() {
        return this->uhs_ && uhs_->canGetValue();
    }
    int getValue() {
        if(!this->uhs_) {
            return 0;
        }
        switch(this->op_) {
            case '!': return !this->uhs_->getValue();
            case '-': return -this->uhs_->getValue();
        }
        return 0;
    }
    void accept(Visitor *v);
};
// 函数调用 func(0,1);
class NFuncCall: public NExpr {
    friend class Generator;
    friend class MidGenerator;
public:
    NIdent* id_;
    Exprlist* arguments_;

    NFuncCall(NIdent* id, Exprlist* args) {
        this->id_ = id;
        this->arguments_ = args;
        this->loc_ = nullptr;
    }

    NFuncCall(NIdent* id) {
        this->id_ = id;
        this->arguments_ = nullptr;
        this->loc_ = nullptr;
    }

    void print();
    bool canGetValue() {
        return false;
    }
    int getValue() {
        return 0;
    }
    void accept(Visitor *v);
};

class NBlock: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    Stmtlist* stmts_;
    NBlock(Stmtlist* stmts) {
        this->stmts_ = stmts;
    }
    NBlock() {
        this->stmts_ = new Stmtlist();
    };
    void append(NStmt* stmt) {
        if(!this->stmts_) {
            this->stmts_ = new Stmtlist();
        }
        this->stmts_->push_back(stmt);
    }
    
    void print();
    void accept(Visitor *v);
};
// Assign like a = 3, b[5] = {1,2,3,4,5};
class NAssign : public NStmt {
    friend class Generator;
    friend class MidGenerator;
private:
    bool is_init_;
public:
    NIdent* id_;
    NExpr* assign_val_;
    // 是否初始化
    NAssign(NIdent *id, NExpr *val) {
        this->id_ = id;
        this->assign_val_ = val;
        this->is_init_ = true;
    }

    NAssign(NIdent *id) {
        this->id_ = id;
        this->assign_val_ = nullptr;
        this->is_init_ = false;
    }

    void print();

    bool isInit() {
        return this->is_init_;
    }
    void accept(Visitor *v);
};

class NVarDecl: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    Type*       type_;
    Assignlist* assign_list_;
    // 是否全局变量
    bool is_global_;
    // 是否在循环内
    bool is_in_loop_;
    NVarDecl(Type* type, Assignlist* list) {
        this->type_ = type;
        this->assign_list_ = list;
        this->is_global_ = false;
    }
    void print();
    void accept(Visitor *v);
};

class NFuncDef: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    Type* ret_type_;
    NIdent* id_;
    Varlist* params_;
    NBlock* body_;
    NFuncDef(Type* ret_type, NIdent* id, Varlist* params, NBlock* body) {
        this->ret_type_ = ret_type;
        this->id_ = id;
        this->params_ = params;
        this->body_ = body;
    }
    NFuncDef(Type *ret_type, NIdent *id, NBlock *body) {
        this->ret_type_ = ret_type;
        this->id_ = id;
        this->params_ = new Varlist();
        this->body_ = body;
    }
    NFuncDef(Type *ret_type, NIdent *id) {
        this->ret_type_ = ret_type;
        this->id_ = id;
        this->params_ = new Varlist();
        this->body_ = nullptr;
    }
    void print();
    void accept(Visitor *v);
};

class NIfStmt: public NStmt {
public:
    NExpr* cond_;
    NBlock* then_;
    NBlock* else_;
    NIfStmt(NExpr* cond, NBlock* then, NBlock* els) {
        this->cond_ = cond;
        this->then_ = then;
        this->else_ = els;
    }
    NIfStmt(NExpr *cond, NBlock *then) {
        this->cond_ = cond;
        this->then_ = then;
        this->else_ = nullptr;
    }

    void print();
    void accept(Visitor *v);
};

class NWhileStmt: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    NExpr* cond_;
    NBlock* do_;
    NWhileStmt(NExpr* cond, NBlock* do_loop) {
        this->cond_ = cond;
        this->do_ = do_loop;
    }
    void print();
    void accept(Visitor *v);
};

class NJmpStmt: public NStmt {
    int jmp_type_;
    friend class Generator;
    friend class MidGenerator;
public:
    NJmpStmt(int jmp_type) {
        this->jmp_type_ = jmp_type;
    }
    void print();
    void accept(Visitor *v);
};

class NRetStmt: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    NExpr* ret_val_;
    NRetStmt(NExpr* ret_val) {
        this->ret_val_ = ret_val;
    }
    NRetStmt() {
        this->ret_val_ = nullptr;
    }

    void print();
    void accept(Visitor *v);
};
// empty stmt like ';'
class NEmptyStmt: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    void print();
    void accept(Visitor* v);
};

// expr stmt like a + 1;
class NExprStmt: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    NExpr* exp_;
    NExprStmt(NExpr* exp) {
        this->exp_ = exp;
    }
    void print();
    void accept(Visitor *v);
};

class NLabelStmt: public NStmt {
    friend class Generator;
    friend class MidGenerator;
public:
    bool has_print;
    ~NLabelStmt(){
        this->has_print = false;
    };
    string getTag() {
        return ".L" + to_string(this->tag_);
    }
    NLabelStmt(): tag_(setTag()), has_print(false){};
private:
    int setTag() {
        static int tag = 0;
        return tag++;
    }
    int tag_;
};

#endif  // AST_HPP