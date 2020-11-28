#include "genArm.hpp"
#include "ast.hpp"
#include "type.hpp"
#include "hashMap.hpp"
class Location;
extern HashMap *parse_time_symtbl;
void NExpr::accept(Visitor* v) {
    this->accept(v);
}

void NStmt::accept(Visitor *v) {
    this->accept(v);
}

void NIdent::accept(Visitor* v) {
    v->visitIdent(this);
}

void NInt::accept(Visitor* v) {
    v->visitIntValue(this);
}
void NBinaryOp::accept(Visitor* v) {
    v->visitBinaryOp(this);
}

void NUnaryOp::accept(Visitor* v) {
    v->visitUnaryOp(this);
}

void NFuncCall::accept(Visitor* v) {
    v->visitFuncCall(this);
}

void NBlock::accept(Visitor* v) {
    v->visitBlock(this);
}

void NAssign::accept(Visitor* v) {
    v->visitAssign(this);
}

void NVarDecl::accept(Visitor* v) {
    v->visitVarDecl(this);
}

void NFuncDef::accept(Visitor* v) {
    v->visitFuncDef(this);

}

void NIfStmt::accept(Visitor* v) {
    v->visitIfStmt(this);
}

void NWhileStmt::accept(Visitor* v) {
    v->visitWhileStmt(this);
}

void NJmpStmt::accept(Visitor* v) {
    v->visitJumpStmt(this);
}

void NRetStmt::accept(Visitor* v) {
    v->visitRetStmt(this);
}

void NEmptyStmt::accept(Visitor* v) {
    v->visitEmptyStmt(this);
}

void NExprStmt::accept(Visitor* v) {
    v->visitExprStmt(this);
}
// 计算数组线性化后下标对应偏移
int genArrayIndex(NIdent *arr_id) {
    // 查询符号表中原始数据的类型
    NIdent *origin_id = parse_time_symtbl->findVar(arr_id->name_);
    // 对于数组变量
    int index = 0;
    // 计算下标位置
    // 获得类型,因为需要知道数组本身的维度大小才能定位元素
    Type *tp = origin_id->type_.get();
    // 必须有类型
    // assert(tp);
    // 按照行优先获得下标,目前还是不支持变量下标
    NExpr *e = nullptr;
    for (int i = 0; i < arr_id->array_index_->size(); i++)
    {
        e = arr_id->array_index_->at(i);
        // 转换为数组类型，成功则为数组，不然就是int
        TArray *arr_tp = dynamic_cast<TArray *>(tp);
        if (arr_tp && arr_tp->base_type_->isArray()) {
            index += arr_tp->len_->getValue() * e->getValue();
            tp = arr_tp->base_type_.get();
            continue;
        }
        index += e->getValue();
    }
    // 返回索引
    return index;
}
int NIdent::getValue() {
    NIdent *origin_id = parse_time_symtbl->findVar(this->name_);
    if(!origin_id || !origin_id->value_) {
        return 0;
    }
    if (origin_id->type_->isArray()) {
        NArray *arr_val = dynamic_cast<NArray *>(origin_id->value_);
        if(arr_val) {
            vector<int> vals = arr_val->toVector();
            int index = genArrayIndex(this);
            if(index < vals.size()) {
                return vals.at(index);
            }
        }
        return 0;
    }
    return origin_id->value_->getValue();
}

bool NIdent::canGetValue() {
    // NIdent* origin_id = parse_time_symtbl->findVar(this->name_);
    // if(!origin_id || !origin_id->value_) {
    //     return false;
    // }
    // // 对于数组元素
    // if(origin_id->type_->isArray()) {
    //     NArray* arr = dynamic_cast<NArray*>(origin_id->value_);
    //     if(!arr) {
    //         return false;
    //     }
    //     vector<int> vec = arr->toVector();
    //     int index = genArrayIndex(this);
    //     return index < vec.size();
    // }
    // return true;
    return false;
}

// print ast
void NInt::print() {
    printf("%lld", this->value_);
}

void NArray::print() {
    printf("{");
    if (this->value_.size() != 0)
        this->value_[0]->print();
    for (int i = 1; i < this->value_.size(); i++) {
        printf(",");
        this->value_[i]->print();
    }
    printf("}");
}

void NIdent::print() {
    printf("Ident: %s", this->name_.c_str());
}

void NBinaryOp::print() {
    printf("BinaryOp: %s", parseOp().c_str());
    printf("\t");
    this->lhs_->print();
    printf("\t");
    this->rhs_->print();
    printf("\n");
}

void NUnaryOp::print() {
    printf("UnaryOp: %c\n", this->op_);
    printf("\t");
    this->uhs_->print();
    printf("\n");
}

void NFuncCall::print() {
    printf("FuncCall\n");
    printf("\t");
    this->id_->print();
    printf("\tArgs\n");
    if(this->arguments_ != nullptr) {
        for(int i = 0;i < this->arguments_->size(); i++) {
            printf("\t");
            this->arguments_->at(i)->print();
            printf("\n");
        }
    }
    printf("\n");
}

void NBlock::print() {
    printf("Block\n");
    for (int i = 0; i < this->stmts_->size(); i++) {
        printf("\t");
        this->stmts_->at(i)->print();
    }
    printf("\n");
}

void NAssign::print() {
    printf("Assign\n");
    printf("\t");
    this->id_->print();
    if (this->assign_val_ != nullptr) {
        printf("\tValue: ");
        this->assign_val_->print();
        printf("\n");
    }
}

void NVarDecl::print() {
    string type;
    if (this->type_->isInt()) {
        type = "int";
    }
    else if (this->type_->isVoid()) {
        type = "void";
    }
    else {
        type = "array";
    }
    printf("VarDecl: %s\n", type.c_str());
    for (int i = 0; i < this->assign_list_->size(); i++) {
        printf("\t");
        this->assign_list_->at(i)->print();
    }
    printf("\n");
}

void NFuncDef::print() {
    printf("FuncDef\n");
    printf("\t");
    printf("%s\n", this->id_->name_.c_str());
    string type;
    if (this->ret_type_->isInt()) {
        type = "int";
    } else if (this->ret_type_->isVoid()) {
        type = "void";
    } else {
        type = "unknown";
    }
    printf("\tRetType: %s\n", type.c_str());
    if (this->params_ != nullptr) {
        printf("\tParams\n");
        for (int i = 0; i < this->params_->size(); i++) {
            printf("\t");
            this->params_->at(i)->print();
        }
    }
    printf("\tBody\n\t");
    this->body_->print();
    printf("\n");
}

void NIfStmt::print() {
    printf("IfStmt\n");
    printf("\tCond\n\t");
    this->cond_->print();
    printf("\tThen\n\t");
    this->then_->print();
    if (this->else_ != nullptr) {
        printf("\tElse\n\t");
        this->else_->print();
    }
    printf("\n");
}

void NWhileStmt::print() {
    printf("WhileStmt\n");
    printf("\tCond\n\t");
    this->cond_->print();
    printf("\tDo\n\t");
    this->do_->print();
    printf("\n");
}

void NJmpStmt::print() {
    printf("JmpStmt\n");
    switch (this->jmp_type_) {
        case AST_BREAK_JMP:
            printf("break\n");
            break;
        case AST_CONTINUE_JMP:
            printf("continue\n");
            break;
        default:
            printf("unknown\n");
    }
}

void NRetStmt::print() {
    printf("RetStmt\n");
    if (this->ret_val_ != nullptr) {
        printf("\tRetVal\n\t");
        this->ret_val_->print();
    }
}

void NEmptyStmt::print() {
    printf("EmptyStmt\n");
    printf("\n");
}

void NExprStmt::print() {
    printf("ExprStmt\n\t");
    this->exp_->print();
    printf("\n");
}