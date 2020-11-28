#ifndef TYPE_HPP
#define TYPE_HPP
#include "ast.hpp"
using namespace std;
class NExpr;
class Type {
public:
    unsigned int width_;
    unsigned int align_;
    bool is_const_;
    virtual ~Type(){};
    virtual bool isInt() = 0;
    virtual bool isVoid() = 0;
    virtual bool isArray() = 0;
    virtual int getWidth() = 0;
    virtual int getLen() = 0;
    virtual int getDepth() = 0;
};

// Int 类型
class TInt : public Type {
public:
    TInt() {
        this->align_ = 4;
        this->width_ = 4;
        this->is_const_ = false;
    }
    ~TInt(){};
    bool isInt() {
        return true;
    }
    bool isVoid() {
        return false;
    }
    bool isArray() {
        return false;
    }
    int getWidth() {
        return this->width_;
    }
    int getLen() {
        return 1;
    }
    int getDepth() {
        return 0;
    }
};

class TVoid : public Type {
public:
    TVoid() {
        this->align_ = 1;
        this->width_ = 1;
        this->is_const_ = false;
    }
    ~TVoid(){};
    bool isInt() {
        return false;
    }
    bool isVoid() {
        return true;
    }
    bool isArray() {
        return false;
    }
    int getWidth() {
        return this->width_;
    }
    int getLen() {
        return 1;
    }
    int getDepth() {
        return 0;
    }
};

class TArray : public Type {
public:
    shared_ptr<Type> base_type_;
    NExpr* len_;
    TArray(shared_ptr<Type> base_type, NExpr *len) {
        this->is_const_ = false;
        this->base_type_ = base_type;
        this->len_ = len;
        this->width_ = 4;
        this->align_ = 4;
    }
    bool isInt() {
        return false;
    }
    bool isVoid() {
        return false;
    }
    bool isArray() {
        return true;
    }
    int getWidth();
    TArray *getTail() {
        TArray *ret = nullptr;
        Type *t = this;
        while (t->isArray()) {
            ret = dynamic_cast<TArray *>(t);
            t = ret->base_type_.get();
        }
        return ret;
    }
    int getDepth() {
        return this->base_type_->getDepth() + 1;
    }
    int getLen();
};
#endif