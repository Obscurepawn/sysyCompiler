#include "condition.hpp"
#include "genMid.hpp"
#include "midCode.hpp"
extern midCodeList midCode;
void Condition::flushCondition(Generator *g, NLabelStmt &end, NLabelStmt &begin)
{
    // 如果为空，或者已经打印过则不执行
    if (this->has_flush)
        return;
    this->has_flush = true;
    // 打印执行语句
    if (this->do_expr)
    {
        this->do_expr->accept(g);
    }

    // 如果满足条件，且下一个条件不为空，继续跳转判断
    if (this->then_cond)
    {
        g->Emit("b" + this->do_when, this->then_cond->getLabelStr());
    }
    else
    {
        // 如果then完了就说明条件全部符合，跳到下一段代码开始处
        g->Emit("b" + this->do_when, begin.getTag());
    }
    /**
         * 如果不满足条件
         * 1. 如果不满足时，仍然有操作，则跳转到下一个不满足时的判断操作
         * 2. 如果不满足时，无继续的操作，则直接跳转到end_label;
         * */
    if (this->end_cond)
    {
        g->Emit("b", this->end_cond->getLabelStr());
    }
    else
    {
        g->Emit("b", end.getTag());
    }
    if (this->then_cond && !this->then_cond->has_flush)
    {
        g->EmitLabel(this->then_cond->getLabelStr());
        this->then_cond->flushCondition(g, end, begin);
    }
    if (this->end_cond && !this->end_cond->has_flush)
    {
        g->EmitLabel(this->end_cond->getLabelStr());
        this->end_cond->flushCondition(g, end, begin);
    }
}

void Condition::flushCondition(MidGenerator *g, NLabelStmt &end, NLabelStmt &begin)
{
    // 如果为空，或者已经打印过则不执行
    if (this->has_flush)
        return;
    this->has_flush = true;
    // 打印执行语句
    if (this->do_expr)
    {
        this->do_expr->accept(g);
    }
    // 如果满足条件，且下一个条件不为空，继续跳转判断
    if (this->then_cond)
    {
        // g->Emit("b" + this->do_when, this->then_cond->getLabelStr());
        // 生成插入中间代码
        //p.s.本函数内生成的中间代码皆为无条件跳转
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = this->then_cond->getLabelStr();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);
    }
    else
    {
        // 如果then完了就说明条件全部符合，跳到下一段代码开始处
        // g->Emit("b" + this->do_when, begin.getTag());
        // 生成插入中间代码
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = begin.getTag();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);
    }
    /**
         * 如果不满足条件
         * 1. 如果不满足时，仍然有操作，则跳转到下一个不满足时的判断操作
         * 2. 如果不满足时，无继续的操作，则直接跳转到end_label;
         * */
    if (this->end_cond)
    {
        // g->Emit("b", this->end_cond->getLabelStr());
        // 生成插入中间代码
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = this->end_cond->getLabelStr();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);
    }
    else
    {
        //  g->Emit("b", end.getTag());
        // 生成插入中间代码
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = end.getTag();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);
    }
    if (this->then_cond && !this->then_cond->has_flush)
    {
        // g->EmitLabel(this->then_cond->getLabelStr());
        // 生成插入中间代码
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = this->then_cond->getLabelStr();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);

        this->then_cond->flushCondition(g, end, begin);
    }
    if (this->end_cond && !this->end_cond->has_flush)
    {
        // g->EmitLabel(this->end_cond->getLabelStr());
        // 生成插入中间代码
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = this->end_cond->getLabelStr();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);

        this->end_cond->flushCondition(g, end, begin);
    }
}

//  lt
Condition *genCondition(NExpr *op)
{
    // dynamic_cast可以将基类指针转换为继承类指针，如果非预期对象，返回null
    NBinaryOp *binary_op = dynamic_cast<NBinaryOp *>(op);
    // 如果不是binary那就直接改成binary，和0比较。
    if (!binary_op)
    {
        binary_op = new NBinaryOp(EQ, op, new NInt(0));
        return new Condition(binary_op, "ne");
    }
    Condition *cond = nullptr;
    switch (binary_op->op_)
    {
    case '<':
        return new Condition(op, "lt");
    case '>':
        return new Condition(op, "gt");
    case LE:
        return new Condition(op, "le");
    case GE:
        return new Condition(op, "ge");
    case EQ:
        return new Condition(op, "eq");
        ;
    case NE:
        return new Condition(op, "ne");
    // 这两个是我大哥...
    case OR:
        // 先设条件为左值
        cond = genCondition(binary_op->lhs_);
        // 如果左值满足则不需要查看右值,否则需要继续查看右值
        for (
            Condition *p = cond,
                      *tmp = genCondition(binary_op->rhs_);
            p; p = p->then_cond)
        {
            p->setEndDo(tmp);
        }
        break;
    case AND:
        // 先设条件为左值
        cond = genCondition(binary_op->lhs_);
        // 如果条件满足再查看右值，否则根据短路原则直接结束判断
        cond->setThenDo(genCondition(binary_op->rhs_));
        break;
    }
    return cond;
}