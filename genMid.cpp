#include "genMid.hpp"
#include "ast.hpp"
#include "condition.hpp"
#include "genArm.hpp"
#include "hashMap.hpp"
#include "midCode.hpp"
#include "parser.hpp"
#include <string>
#include <unordered_map>
#define MAX_LEVEL 100000
using namespace std;
bool isGenMid;
//8.14
midCodeList midCode;
//变量重命名 需要符号表的树形信息
//注意每次切换作用域时修改值
int symtbl_level_cnt[MAX_LEVEL];
int symtbl_level;

//当前值的变量名 用于genMid()或genIndex()
string cur_string_name;

//符号表指针 当前层的
//注意每次切换作用域时修改该指针！
HashMap *symtbl;

//临时变量编号 是全局的
//“世界上每产生一个临时变量 mid_tmpVarNo就会自增”
int mid_tmpVarNo = 0;

//循环层数 再while_stmt中用过 不知道干嘛的
int loop_level = 0;

//当前语句嵌套深度
int now_block_level = 0;
//提前跳转相关
NLabelStmt *pre_ret_label = nullptr;
string pre_data = "";
// 标记当前break, continue需要跳转的Label标签
stack<NLabelStmt *> code_begin_label; // loop 开始处，continue跳转位置
stack<NLabelStmt *> code_end_label;   // loop 结束处，break跳转位置

//check 0819
//不过这里面的比较运算是不是多余了？
//生成二元运算的中间代码 其结果存入临时变量 并记录到cur_string_name
void MidGenerator::visitBinaryOp(NBinaryOp *binary_op)
{
    //new一个中间代码结构
    NmidCode *newMidCode = new NmidCode();
    //被X数/左子树
    if (binary_op->lhs_->canGetValue())
    { //是VINT
        newMidCode->rVal1.type = VINT;
        //可以使用getValue()获取常量的值
        newMidCode->rVal1.val = binary_op->lhs_->getValue();
    }
    else
    { //是VIDENT
        newMidCode->rVal1.type = VIDENT;
        // 被X数变量名
        newMidCode->rVal1.ident = genMid(binary_op->lhs_);
    }
    //X数/右子树
    if (binary_op->rhs_->canGetValue())
    {
        newMidCode->rVal2.type = VINT;
        newMidCode->rVal2.val = binary_op->rhs_->getValue();
    }
    else
    {
        newMidCode->rVal2.type = VIDENT;
        // X数变量名
        newMidCode->rVal2.ident = genMid(binary_op->rhs_);
    }

    //二元运算符 有计算和比较两类
    string inst;
    switch (binary_op->op_)
    {
    case '+':
        inst = "calc";
        newMidCode->op = cal_op::ADD;
        break;
    case '-':
        inst = "calc";
        newMidCode->op = cal_op::SUB;
        break;
    case '*':
        inst = "calc";
        newMidCode->op = cal_op::MUL;
        break;
    case '/':
        inst = "calc";
        newMidCode->op = cal_op::DIV;
        break;
    case '%':
        inst = "calc";
        newMidCode->op = cal_op::MOD;
        break;
    case '<':
        inst = "cmp";
        newMidCode->cmp = cmp_op::L;
        break;
    case '>':
        inst = "cmp";
        newMidCode->cmp = cmp_op::G;
        break;
    case LE:
        inst = "cmp";
        newMidCode->cmp = cmp_op::LE;
        break;
    case GE:
        inst = "cmp";
        newMidCode->cmp = cmp_op::GE;
        break;
    case EQ:
        inst = "cmp";
        newMidCode->cmp = cmp_op::EQ;
        break;
    case NE:
        inst = "cmp";
        newMidCode->cmp = cmp_op::NE;
        break;
    default:
        break;
    }

    //根据这个表达式的用途 对中间代码的功能做细分
    if (inst == "calc")
    { //普通二元运算
        //临时变量名 运算的结果需要有个临时变量存储
        //变量名暂定为(“t%d”,临时变量编号) 和样例有冲突可更改
        newMidCode->lVal = "t_" + to_string(mid_tmpVarNo);
        mid_tmpVarNo++;
        //将单句midcode加入midcode列表
        midCode.push_back(*newMidCode);
        //表示当前存值变量名
        cur_string_name = newMidCode->lVal;
    }
    else
    { //条件跳转
        //not used
    }
}

//check 0819
//生成一元运算的中间代码 其结果存入临时变量 并记录到cur_string_name
void MidGenerator::visitUnaryOp(NUnaryOp *unary_op)
{
    //new一个中间代码结构
    NmidCode *newMidCode = new NmidCode();
    //操作数/根据AST规则 位于左子树
    if (unary_op->canGetValue())
    {
        newMidCode->rVal1.type = VINT;
        newMidCode->rVal1.val = unary_op->getValue();
    }
    else
    {
        newMidCode->rVal1.type = VIDENT;
        newMidCode->rVal1.ident = genMid(unary_op->uhs_);
    }
    //运算符 分别为按位取反和取反
    switch (unary_op->op_)
    {
    case '!':
        //mid code definition to do:逻辑运算符按位取反
        //我先随便写个 midcode.cpp里也修改了
        newMidCode->op = cal_op::NOT;
        break;
    case '-':
        newMidCode->op = cal_op::NEG;
        break;
    }
    //临时变量存值
    newMidCode->lVal = "t_" + to_string(mid_tmpVarNo);
    mid_tmpVarNo++;
    //放入midcodelist
    midCode.push_back(*newMidCode);
    //临时变量记录到cur_string_name
    cur_string_name = newMidCode->lVal;
}

//check 0819
//生成赋值语句的中间代码
void MidGenerator::visitAssign(NAssign *assign)
{
    //赋值构成规则：update 8.14
    // op = COPY 时, lVal = rVal1
    // op = COPY_AtoI 时，rVal2 = lVal[rVal1]
    // op = COPY_ItoA, 时，lVal[rVal1] = rVal2

    //注意 对于 a=b[c]而言 并不是采用第二条规则 而是拆成两部分
    //t1=b[c] a=t1
    //即第二条规则在本函数中并不会用到 而是在处理Nexpr时用到

    //new一个中间代码结构
    NmidCode *newMidCode = new NmidCode();
    //要赋的值
    NExpr *value = assign->assign_val_;

    //目的变量/左值 是一个标识符
    NIdent *id = symtbl->findVar(assign->id_->midname_);

    //原本全局变量和非全局变量分开处理
    //但是生成中间代码无需这样做
    //下面分op=COPY ItoA 两种情况讨论
    if (!id->type_->isArray())
    { //op=COPY左值是单个变量
        // op = COPY 时, lVal = rVal1
        newMidCode->op = cal_op::COPY;
        newMidCode->lVal = id->midname_;
        //注意这里对右边表达式的处理
        //由于右边表达式时NExpr类型的 缺少它是否是常数等信息
        //为了编程方便 统一为genMid返回的临时变量
        //即使是常数 也先存进临时变量里
        //todo:genMid时 对于常数结点 需要写一个临时变量赋值语句
        newMidCode->rVal1.type = VIDENT;
        newMidCode->rVal1.ident = genMid(value);
    }
    else
    { //op=ItoA左值是数组
        // op = COPY_ItoA, 时，lVal[rVal1] = rVal2
        newMidCode->op = cal_op::COPY_ItoA;
        newMidCode->lVal = id->midname_;

        //同上对右边表达式的处理 左值数组下标统一为临时变量
        //考虑到多维数组的存在 写一个genIndex函数来获得偏移量
        //genIndex()逻辑同genVarArrayIndex(NIdent* arr_id)
        newMidCode->rVal1.type = VIDENT;
        newMidCode->rVal1.ident = genIndex(id);

        newMidCode->rVal2.type = VIDENT;
        newMidCode->rVal2.ident = genMid(value);
    }
    //放进中间代码列表
    midCode.push_back(*newMidCode);
}

//check 0819
//生成if条件语句的中间代码
void MidGenerator::visitIfStmt(NIfStmt *if_stmt)
{
    //if下是新的作用域 符号表层数+1
    symtbl_level++;
    symtbl_level_cnt[symtbl_level]++;
    symtbl = new HashMap(symtbl);
    //block层数+1
    now_block_level++;

    //生成条件结构
    Condition *cond = genCondition(if_stmt->cond_);
    //lebel
    NLabelStmt if_label;
    NLabelStmt end_label;
    if (if_stmt->else_)
    {
        //if(){}else{}
        NLabelStmt else_label;
        //flushcondition(this,endlabel,beginlabel)
        //意为：
        //满足条件时 跳到beginlabel：iflabel
        //不满足条件时 跳到endlebel：elselabel
        cond->flushCondition(this, else_label, if_label);

        //处理else部分：
        //生成一个tag中间代码
        NmidCode *elseTag = new NmidCode();
        elseTag->tag = else_label.getTag();
        midCode.push_back(*elseTag);
        //生成else{}这里面的语句
        if_stmt->else_->accept(this);
        //生成一个无条件跳转
        //结束时 跳到endlabel
        //需要插入无条件跳转中间代码
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = end_label.getTag();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);

        //处理if部分：
        //生成一个tag中间代码
        NmidCode *ifTag = new NmidCode();
        ifTag->tag = if_label.getTag();
        midCode.push_back(*ifTag);
        //生成if(){}这里面的语句
        if_stmt->then_->accept(this);
        //生成一个到结束处的无条件跳转
        //这里直接用上面那个结构再插入一次就行
        midCode.push_back(*newMidCode);
    }
    else
    {
        //if(){}
        //flushcondition(this,endlabel,beginlabel)
        //意为：
        //满足条件时 跳到beginlabel：iflabel
        //不满足条件时 跳到endlebel：endlabel
        cond->flushCondition(this, end_label, if_label);

        //处理if(){}部分
        //生成一个tag中间代码
        NmidCode *ifTag = new NmidCode();
        ifTag->tag = if_label.getTag();
        midCode.push_back(*ifTag);
        //生成if(){}里面的语句
        if_stmt->then_->accept(this);
        //生成一个到结束的无条件跳转
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = end_label.getTag();
        newMidCode->op = cal_op::GOTO_U;
        midCode.push_back(*newMidCode);
    }

    //block层数-1
    now_block_level--;

    //退出作用域 符号表层数-1
    symtbl_level--;
    symtbl = symtbl->getFather();
    // symtbl->pop();
}

//check 0819
//生成while循环语句的中间代码
void MidGenerator::visitWhileStmt(NWhileStmt *while_stmt)
{
    loop_level++;
    //符号表深度++
    symtbl_level++;
    symtbl_level_cnt[symtbl_level]++;
    symtbl = new HashMap(symtbl);
    //循环体label
    NLabelStmt begin_label;
    //条件label
    NLabelStmt *cond_label = new NLabelStmt();
    code_begin_label.push(cond_label);
    //循环体结束label
    NLabelStmt *end_label = new NLabelStmt();
    code_end_label.push(end_label);
    //生成条件中间代码
    Condition *cond = genCondition(while_stmt->cond_);

    //无条件跳转到条件比较字段
    NmidCode *newMidCode = new NmidCode();
    newMidCode->to_tag = cond_label->getTag();
    newMidCode->op = cal_op::GOTO_U;
    midCode.push_back(*newMidCode);

    //生成循环体内中间代码
    //添加该段的label
    NmidCode *whileTag = new NmidCode();
    whileTag->tag = begin_label.getTag();
    midCode.push_back(*whileTag);
    //生成while体的代码
    while_stmt->do_->accept(this);
    //结束后无条件跳转到条件比较字段
    midCode.push_back(*newMidCode);

    //生成条件比较字段中间代码
    //添加该段的label
    NmidCode *condTag = new NmidCode();
    condTag->tag = cond_label->getTag();
    midCode.push_back(*condTag);
    //flushcondition(this,endlabel,beginlabel)
    //意为：
    //满足条件时 跳到beginlabel
    //不满足条件时 跳到endlebel
    cond->flushCondition(this, *end_label, begin_label);

    //结束lebel
    NmidCode *endTag = new NmidCode();
    endTag->tag = end_label->getTag();
    midCode.push_back(*endTag);

    //about 循环层数？
    code_begin_label.pop();
    code_end_label.pop();
    loop_level--;

    //符号表深度--
    symtbl_level--;
    symtbl = symtbl->getFather();
    // symtbl->pop();
}

//check 0819
//生成break或continue的跳转
//与之前循环层数有关
void MidGenerator::visitJumpStmt(NJmpStmt *jmp_stmt)
{
    //无条件跳转
    NmidCode *newMidCode = new NmidCode();
    newMidCode->op = cal_op::GOTO_U;
    switch (jmp_stmt->jmp_type_)
    {
    case AST_CONTINUE_JMP:
        newMidCode->to_tag = code_begin_label.top()->getTag();
        break;
    case AST_BREAK_JMP:
        newMidCode->to_tag = code_end_label.top()->getTag();
        break;
    }
    midCode.push_back(*newMidCode);
}

//0819 check
//处理return
void MidGenerator::visitRetStmt(NRetStmt *ret_stmt)
{
    // 针对提前return
    if (now_block_level > 2)
    {
        // 产生跳转标签
        if (!pre_ret_label)
        {
            pre_ret_label = new NLabelStmt();
        }
        if (ret_stmt->ret_val_)
        { //如果有返回值
            //处理返回值
            //生成返回值的中间代码 并得到一个变量 准备返回它
            string des = genMid(ret_stmt->ret_val_);
            //特别记录一下这里要返回的值?保留ml原来的架构 不乱改了
            pre_data = des;
            cur_string_name = pre_data; //needed?
        }
        //无条件跳转到pre_ret_label
        NmidCode *newMidCode = new NmidCode();
        newMidCode->to_tag = pre_ret_label->getTag();
        // newMidCode->op = cal_op::GOTO_U;
        newMidCode->op = cal_op::RETURN;
        midCode.push_back(*newMidCode);
        return;
    }
    // 不存在提前跳转
    if (!pre_ret_label)
    {
        if (ret_stmt->ret_val_)
        { //有返回值
            string des = genMid(ret_stmt->ret_val_);
            cur_string_name = des; //needed?
        }
    }
    else
    {
        // 存在提前跳转需要先打印Label，然后写ret的语句
        if (ret_stmt->ret_val_)
        {
            string des = genMid(ret_stmt->ret_val_);
            cur_string_name = des; //needed?
        }
        //打印label
        NmidCode *retTag = new NmidCode();
        //retTag->op = cal_op::RETURN;//only an EmitLabel(),not "retrun" type
        retTag->tag = pre_ret_label->getTag();
        midCode.push_back(*retTag);

        if (pre_data.length() > 0)
        {                               //有提前跳转 有返回值
            cur_string_name = pre_data; //needed?
            pre_data = "";
            pre_ret_label = nullptr;
        }
    }
}

//0819 check
//生成NExprStmt下中间代码的接口
void MidGenerator::visitExprStmt(NExprStmt *expr_stmt)
{
    //对应的是stmt -> exp;
    //依靠子结点的具体信息去生成
    expr_stmt->exp_->accept(this);
}

//0819 check
//遇到block时处理符号表等的嵌套问题
void MidGenerator::visitBlock(NBlock *block_node)
{
    //符号表深度
    symtbl_level++;
    symtbl_level_cnt[symtbl_level]++;
    symtbl = new HashMap(symtbl);

    // 嵌套深度 + 1
    now_block_level += 1;
    // 具有函数参数,将参数合并加入子符号表，同时清空参数符号表
    // gen block
    NStmt *stmt = nullptr;
    for (int i = 0; i < block_node->stmts_->size(); i++)
    { //travel every stmt in this block
        stmt = block_node->stmts_->at(i);
        stmt->accept(this);
    }
    // 跳出block，嵌套深度 -1
    now_block_level -= 1;

    //符号表深度
    symtbl_level--;
    symtbl = symtbl->getFather();
    // symtbl->pop();
}

//0820 check
//声明变量时 把变量添加到符号表 并赋予它在midcode中的名字
//这一步不会生成中间代码 仅维护符号表
//BUT, if there is assign while declaration, use assign->accpt(this)
void MidGenerator::visitVarDecl(NVarDecl *var_decl)
{
    Assignlist *assign_list = var_decl->assign_list_;
    if (!assign_list)
    {
        return;
    }
    NAssign *assign = nullptr;

    //如果为全局变量
    if (var_decl->is_global_)
    {
        int size = var_decl->assign_list_->size();
        for (int i = 0; i < size; i++)
        { //对于声明语句中每个变量
            assign = var_decl->assign_list_->at(i);
            //重命名 exp. g_11
            assign->id_->midname_ = "g_" + to_string(symtbl->size());
            symtbl->push(assign->id_);
            assign->accept(this);
        }
        return;
    }

    //如果为局部变量
    int assign_list_size = assign_list->size();
    for (int i = 0; i < assign_list_size; i++)
    { //对于每一个声明的变量
        assign = assign_list->at(i);
        //为该变量重命名 存储在该变量NIdent结点的 midname_ 变量中
        //exp. t_2_3_11
        assign->id_->midname_ = "a_" + to_string(symtbl_level) + "" + to_string(symtbl_level_cnt[symtbl_level]) + "_" + to_string(symtbl->size());
        //存入符号表
        symtbl->push(assign->id_);
        //赋值？
        assign->accept(this);
    }
}

//0820 check
//访问一个Ident结点 返回变量名即可(记录到cur_string_name)
// BUT, if arrry, return the value with a tmp var "t_x"
void MidGenerator::visitIdent(NIdent *ident)
{
    if (ident->type_->isArray())
    {
        // op = COPY_AtoI 时，rVal2 = lVal[rVal1]
        NmidCode *newMidCode = new NmidCode();
        newMidCode->op = cal_op::COPY_AtoI;
        newMidCode->rVal2.type = IDENT;
        newMidCode->rVal2.ident = "t_" + to_string(mid_tmpVarNo);
        mid_tmpVarNo++;
        newMidCode->rVal1.type = IDENT;
        newMidCode->rVal1.ident = genIndex(ident);
        newMidCode->lVal = ident->midname_;
        cur_string_name = newMidCode->rVal2.ident;
    }
    else
    {
        cur_string_name = ident->midname_;
    }
}

//0820 check
//函数定义
void MidGenerator::visitFuncDef(NFuncDef *func_def)
{
    //加入符号表
    symtbl->push(func_def);
    //生成一个label 与函数同名
    NmidCode *funTag = new NmidCode();
    funTag->tag = func_def->id_->name_.c_str();
    midCode.push_back(*funTag);
    // 加入参数
    NmidCode *funPara = new NmidCode();
    funPara->op = cal_op::FUNC_DEF;
    NIdent *id;
    for (int i = 0; i < func_def->params_->size(); i++)
    {
        rVal r;
        id = func_def->params_->at(i)->assign_list_->at(i)->id_;
        id->midname_ = "t_" + to_string(symtbl_level) + "" + to_string(symtbl_level_cnt[symtbl_level]) + "_" + to_string(symtbl->size());
        symtbl->push(id);
        r.ident = func_def->params_->at(i)->assign_list_->at(i)->id_->name_;
        r.type = VIDENT;
        funPara->paraList.push_back(r);
    }
    //生成函数体
    func_def->body_->accept(this);
}

//0820 check
void MidGenerator::visitIntValue(NInt *val)
{
    // may do nothing or return a digit?
    //这里是不用处理吧 生成中间代码时会先检查canGetValue() 然后调用getValue()
    //visit到这个的时候似乎不用管
}

//0820 check
void MidGenerator::visitFuncCall(NFuncCall *func_call)
{
    //改个名字
    if (func_call->id_->name_ == "stoptime")
    {
        func_call->id_->name_ = "_sysy_stoptime";
    }
    if (func_call->id_->name_ == "starttime")
    {
        func_call->id_->name_ = "_sysy_starttime";
    }

    //block、符号表等层数变化
    symtbl_level++;
    symtbl_level_cnt[symtbl_level]++;
    now_block_level++;
    symtbl = new HashMap(symtbl);

    //处理参数传递
    NmidCode *callMidCode = new NmidCode();
    if (func_call->arguments_)
    { //传入参数
        for (int i = 0; i < func_call->arguments_->size(); i++)
        {
            //计算表达式 并将结果放到midcode的参数列表中
            NExpr *e = func_call->arguments_->at(i);
            string des = genMid(e);
            //?为什么报错
            rVal p;
            p.ident = des;
            callMidCode->paraList.push_back(p);
        }
    }

    //跳转
    callMidCode->to_tag = func_call->id_->name_;
    callMidCode->op = cal_op::FUNC_CALL;
    midCode.push_back(*callMidCode);

    //block、符号表等层数变化
    symtbl_level--;
    now_block_level--;
    symtbl = symtbl->getFather();
    // symtbl->pop();
}

//0820 check
//1即是0 0即是1
void MidGenerator::visitEmptyStmt(NEmptyStmt *empty)
{
    // do nothing
}

//0820 check
//生成expr里的中间代码 并且返回最终结果所在的临时变量名
string MidGenerator::genMid(NExpr *expr)
{
    expr->accept(this);
    // 设一个全局变量来存当前产生值的名称 cur_string_name
    return cur_string_name;
}

//0820 check
//对于数组的特别处理 即使是多维数组 也通过该函数得到偏移量
//等效为一维函数
//偏移量存储在该函数返回的临时变量名中
string MidGenerator::genIndex(NIdent *arr_id)
{
    if (!arr_id->array_index_)
    {
        return "";
    }
    //在符号表中找到该数组
    NIdent *origin_id = symtbl->findVar(arr_id->name_);

    //处理每一维时要用到的 这里循环外直接处理第一维
    NExpr *e = arr_id->array_index_->at(0);
    string des = genMid(e); //偏移量
    //遍历数组的维数

    for (int i = 1; i < arr_id->array_index_->size(); i++)
    {
        //当前迭代出的偏移量*当前维的声明大小+当前维下标
        //当前维的声明大小:origin_id->array_index_->at(i);
        //插入乘法运算中间代码 des=des*origin_id->array_index->at(i)
        NmidCode *newMidCode = new NmidCode();
        newMidCode->op = cal_op::MUL;
        newMidCode->lVal = des;
        newMidCode->rVal1.type = IDENT;
        newMidCode->rVal1.ident = des;
        newMidCode->rVal2.type = IDENT;
        e = origin_id->array_index_->at(i);
        newMidCode->rVal2.ident = genMid(e);
        midCode.push_back(*newMidCode);

        //插入加上当前维下标的中间代码 des=des+arr_id->array_index_->at(i)
        newMidCode->op = cal_op::ADD;
        newMidCode->lVal = des;
        newMidCode->rVal1.type = IDENT;
        newMidCode->rVal1.ident = des;
        newMidCode->rVal2.type = IDENT;
        e = arr_id->array_index_->at(i);
        newMidCode->rVal2.ident = genMid(e);
        midCode.push_back(*newMidCode);
    }
    //没写错误处理

    return des;
}

void MidGenerator::genMidCode(NBlock *root)
{
    //梦开始的地方
    isGenMid = true;
    symtbl = new HashMap();
    root->accept(this);
    midCodeOutput(&midCode);
}