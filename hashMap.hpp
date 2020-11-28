#ifndef HASHMAP_HPP
#define HASHMAP_HPP
#include <map>
#include <stack>
#include <assert.h>
using namespace std;
class NFuncdef;
class NIdent;
extern bool isGenMid;
class HashMap {
private:
    // 指向父级符号表
    HashMap*    father_hash_;
    stack<HashMap*>        son_hash_;
    map<string, NIdent*>    var_map_;
    map<string, NFuncDef*>  func_map_;
public:
    void push(NIdent* ident) {
        // wait to fix conflict
        if(isGenMid) {
            printf("push %s\n", ident->midname_.c_str());
            this->var_map_.insert(make_pair(ident->midname_, ident));
        }
        this->var_map_.insert(make_pair(ident->name_, ident));
    }
    void push(NFuncDef* func) {
        // wait to fix conflict
        this->func_map_.insert(make_pair(func->id_->name_, func));
    }
    ~HashMap(){};
    // Constructors
    HashMap(): father_hash_(nullptr) {};
    HashMap(HashMap* father): father_hash_(father){
        this->father_hash_->son_hash_.push(this);
    };
    void pop() {
        if(this->son_hash_.size() > 0) {
            //this->son_hash_.back()->~HashMap();
            this->son_hash_.pop();
        }
    }
    NIdent* findVar(string name) {
        // search var
        if(this->var_map_.find(name) != this->var_map_.end()) {
            return this->var_map_.at(name);
        }
        // if(this->var_map_.find(name) != this->var_map_.end()) {
        //     return this->var_map_[name];
        // }
        // search in father hashtable
        NIdent* target;
        for(HashMap* p = this->father_hash_; p != nullptr; p = p->father_hash_) {
            target = p->findVar(name);
            if(target) {
                return target;
            }
        }
        return nullptr;
    }
    NFuncDef* findFunc(string name) {
       // search var
       if(this->func_map_.count(name)) {
           return this->func_map_.at(name);
       }
        // if(this->func_map_.find(name) != this->func_map_.end()) {
        //     return this->func_map_[name];
        // }
        // search in father hashtable
        NFuncDef* target;
        for(HashMap* p = this->father_hash_; p != nullptr; p = p->father_hash_) {
            target = p->findFunc(name);
            if(target != nullptr)
                return target;
        }
        return nullptr;
    }
    void append(HashMap* h) {
        if(!h)
            return;
        // 合并变量表
        if(!h->var_map_.empty()) {
            for(auto e: h->var_map_) {
                this->var_map_[e.first] = e.second;
            }
        }
        // 合并函数表
        if(!h->func_map_.empty()) {
            for (auto e : h->func_map_) {
                this->func_map_[e.first] = e.second;
            }
        }   
    }
    // 获得符号表大小
    int size() {
        return this->var_map_.size() + this->func_map_.size();
    }
    // 清空符号表
    void clear() {
        this->var_map_.clear();
        this->func_map_.clear();
    }

    HashMap* getFather() {
        return this->father_hash_;
    }
};
#endif