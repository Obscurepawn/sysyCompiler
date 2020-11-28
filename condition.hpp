#ifndef CONDITION_HPP
#define CONDITION_HPP
#include "ast.hpp"
#include "visitor.hpp"
#include "genMid.hpp"
#include "genArm.hpp"
#include "parser.hpp"
Condition* genCondition(NExpr *op);
class Condition {
public:
    // 是否已经打印
    bool has_flush;
    // Label标签
    NLabelStmt *cond_label;
    // 条件及语句
    string do_when;
    NExpr *do_expr;
    // 满足条件后执行操作的label及其情况
    Condition *then_cond;
    // 不满足条件后执行的label和情况
    Condition *end_cond;
    // 条件跳转
    Condition(NExpr *todo, string cond) {
        this->cond_label = new NLabelStmt();
        this->do_expr = todo;
        this->do_when = cond;
        this->then_cond = nullptr;
        this->end_cond = nullptr;
        this->has_flush = false;
    };
    // 无条件跳转
    Condition(NExpr *todo) {
        this->cond_label = new NLabelStmt();
        this->do_expr = todo;
        this->do_when = "";
        this->then_cond = nullptr;
        this->end_cond = nullptr;
        this->has_flush = false;
    };
    Condition() {
        this->do_when = "";
        this->do_expr = nullptr;
        this->then_cond = nullptr;
        this->end_cond = nullptr;
        this->cond_label = nullptr;
        this->has_flush = false;
    }
    void setThenDo(Condition *then) {
        Condition *p = this;
        while (p->then_cond)
            p = p->then_cond;
        p->then_cond = then;
    }
    void setEndDo(Condition *end) {
        Condition *p = this;
        while (p->end_cond) {
            p = p->end_cond;
        }
        p->end_cond = end;
    }
    string getLabelStr() {
        if (!this->cond_label)
            return "";
        return this->cond_label->getTag();
    }
    void flushCondition(Generator *g, NLabelStmt &end, NLabelStmt &begin);
    void flushCondition(MidGenerator *m, NLabelStmt &end, NLabelStmt &begin);
};
#endif