//
//      ┏┛ ┻━━━━━┛ ┻┓
//      ┃　　　　　　 ┃
//      ┃　　　━　　　┃
//      ┃　┳┛　  ┗┳　┃
//      ┃　　　　　　 ┃
//      ┃　　　┻　　　┃
//      ┃　　　　　　 ┃
//      ┗━┓　　　┏━━━┛
//        ┃　　　┃   神兽保佑
//        ┃　　　┃   代码无BUG!
//        ┃　　　┗━━━━━━━━━┓
//        ┃　　　　　　　    ┣┓
//        ┃　　　　         ┏┛
//        ┗━┓ ┓ ┏━━━┳ ┓ ┏━┛
//          ┃ ┫ ┫   ┃ ┫ ┫
//          ┗━┻━┛   ┗━┻━┛

#include "genArm.hpp"
#include "condition.hpp"
#include "hashMap.hpp"
#include "parser.hpp"
#include "reg.hpp"
#include "type.hpp"
#include <stack>
#include <string>
#include <unordered_map>
using namespace std;
// 存函数所用的全局变量
vector<NIdent *> globl_data;
// loop level
static int loop_level = 0;
// 查找当前变量是否在全局变量表中
bool findGloblData(NIdent *id)
{
    if (!id)
    {
        return true;
    }
    for (int i = 0; i < globl_data.size(); i++)
    {
        if (globl_data.at(i)->name_ == id->name_)
            return true;
    }
    return false;
}
reg reg_manager;
// 表示当前存值的寄存器
string cur_str_reg = "";
// 存入函数所用的Rodata，用于栈空间中的数组元素
vector<Rodata *> rodatas;
// 常量符号表
unordered_map<string, int> const_data_map;
unordered_map<string, vector<int>> const_arr_map;
// 两位大哥模拟sp,fp位置，完成栈空间地址分配
int cur_sp_pos = 0;
int cur_fp_pos = 0;
// 模拟状态寄存器大哥，用于表明跳转条件
string cur_state = "";
// 标注当前语句嵌套深度，针对提前return
static int now_block_level = 0;
// 处理提前return所需标签
static NLabelStmt *pre_ret_label = nullptr;
static string pre_data = "";
// 标记当前break, continue需要跳转的Label标签
static stack<NLabelStmt *> code_begin_label; // loop 开始处，continue跳转位置
static stack<NLabelStmt *> code_end_label;   // loop 结束处，break跳转位置
// 寄存器类
struct Register
{
    // 你的名字
    string name;
    // 是否被占用
    bool is_used;
    Register(string reg_name)
    {
        this->name = reg_name;
        this->is_used = false;
    }
};
// 符号表
static HashMap *symtbl = nullptr;
// 函数参数表
HashMap *func_param_tbl = nullptr;
// 条件类,用于条件,特别是 && || 等复合条件
extern bool to_debug;
// 判断传递地址
bool isPassAddr(NIdent *id)
{
    if (!id->array_index_)
    {
        return true;
    }
    NIdent *origin_id = symtbl->findVar(id->name_);
    if (!origin_id->type_->isArray())
    {
        return false;
    }
    Type *tp = origin_id->type_.get();
    NExpr *index_expr = nullptr;
    int type_deep = 0;
    while (tp)
    {
        TArray *arr_tp = dynamic_cast<TArray *>(tp);
        if (!arr_tp)
        {
            break;
        }
        tp = arr_tp->base_type_.get();
        type_deep++;
    }
    int exp_deep = 0;
    for (int i = 0; i < id->array_index_->size(); i++)
    {
        index_expr = id->array_index_->at(i);
        if (!index_expr)
        {
            return true;
        }
        exp_deep++;
    }
    return exp_deep != type_deep;
}
// 寄存器们
static Register regs[16] = {
    Register("r0"), Register("r1"), Register("r2"), Register("r3"),
    Register("r4"), Register("r5"), Register("r6"), Register("r7"),
    Register("r8"), Register("r9"), Register("r10"), Register("r11"),
    Register("r12"), Register("sp"), Register("lr"), Register("pc")
};
string getRegName(regName r)
{
    if (r == regName::NOT_FND)
    {
        fprintf(stderr, "Error: illegal regName\n");
        exit(EXIT_FAILURE);
    }
    return regs[(int)r].name;
}
//  输出代码函数
void Generator::Emit(string inst)
{
    fprintf(this->out_file_ptr_, "\t%s\n", inst.c_str());
    //fflush(this->out_file_ptr_);
}
// example: bx lr
void Generator::Emit(string op_, string des)
{
    fprintf(this->out_file_ptr_, "\t%s\t%s\n", op_.c_str(), des.c_str());
    //fflush(this->out_file_ptr_);
}
// example: ldr r0, [r1, #4]
void Generator::Emit(string op_, string des, string src)
{
    fprintf(
        this->out_file_ptr_, "\t%s\t%s, %s\n",
        op_.c_str(),
        des.c_str(),
        src.c_str());
    //fflush(this->out_file_ptr_);
}
// example: ldr r0, [r1, #-4]!
void Generator::Emit(string op_, string des, string src, bool is_in_memory)
{
    if (is_in_memory)
    {
        src = '[' + src + ']';
    }
    Emit(op_, des, src);
}
void Generator::Emit(string op_, string des, int digit)
{
    string src = "#" + to_string(digit);
    Emit(op_, des, src);
}
// example: ldr r0, r1, r2
void Generator::Emit(string op_, string des, string src, string second_op)
{
    fprintf(
        this->out_file_ptr_, "\t%s\t%s, %s, %s\n",
        op_.c_str(),
        des.c_str(),
        src.c_str(),
        second_op.c_str());
    //fflush(this->out_file_ptr_);
}
// example: sub sp, sp, #12
void Generator::Emit(string op_, string des, string src, int second_op)
{
    string op2 = "#" + to_string(second_op);
    Emit(op_, des, src, op2);
}

void Generator::EmitLabel(string label_)
{
    fprintf(this->out_file_ptr_, "%s:\n", label_.c_str());
    fflush(this->out_file_ptr_);
}

static int getTempReg()
{
    for (int i = 8; i <= 10; i++)
    {
        if (regs[i].is_used == false)
        {
            regs[i].is_used = true;
            return i;
        }
    }
    return -1;
}

static int getReg()
{
    for (int i = 4; i <= 7; i++)
    {
        if (regs[i].is_used == false)
        {
            regs[i].is_used = true;
            return i;
        }
    }
    return getTempReg();
}

int getRegIndex(string reg_name)
{
    for (int i = 0; i < 16; i++)
    {
        if (regs[i].name == reg_name)
        {
            return i;
        }
    }
    return -1;
}
/***********
 *  Expr   *
 **********/
void Generator::genArmCode(NBlock *root_block)
{
    Emit(".arch armv7-a");
    Emit(".eabi_attribute 28, 1");
    Emit(".eabi_attribute 20, 1");
    Emit(".eabi_attribute 21, 1");
    Emit(".eabi_attribute 23, 3");
    Emit(".eabi_attribute 24, 1");
    Emit(".eabi_attribute 25, 1");
    Emit(".eabi_attribute 26, 2");
    Emit(".eabi_attribute 30, 6");
    Emit(".eabi_attribute 34, 1");
    Emit(".eabi_attribute 18, 4");
    // init symtbl
    symtbl = new HashMap();
    func_param_tbl = new HashMap();
    // let's gen code
    root_block->accept(this);
}

void Generator::visitBinaryOp(NBinaryOp *binary_op)
{
    //todo unary_op->uhs()->getName():得到变量名
    //todo:emit需要‘=’
    //e.g. ldr r0,=var1;取地址到r0

    //Emit("ldr",reg0,binary_op->lhs_->getName());
    //Emit("ldr",reg1,binary_op->rhs_->getName());

    //String reg1=getReg();
    //todo:emit需要‘[]’
    //e.g. ldr r1,[r0];从地址r0取值到r1
    //另外，不知道这里取值能不能 不多申请一个寄存器，而是就用存地址的寄存器 ,没试过
    //但是我下面就用不多申请一个寄存器的写法了，如果不对再改
    //Emit("ldr",reg0,reg0);
    //Emit("ldr",reg1,reg1);

    // 对于div和mod特殊处理, 需要调用函数
    if (binary_op->op_ == '/')
    {
        Exprlist *exp_list = new Exprlist();
        exp_list->push_back(binary_op->lhs_);
        exp_list->push_back(binary_op->rhs_);
        NIdent *func_id = new NIdent("__aeabi_idiv");
        NFuncCall *gcc_func = new NFuncCall(func_id, exp_list);
        gcc_func->accept(this);
        return;
    }

    if (binary_op->op_ == '%')
    {
        Exprlist *exp_list = new Exprlist();
        exp_list->push_back(binary_op->lhs_);
        exp_list->push_back(binary_op->rhs_);
        NIdent *func_id = new NIdent("__aeabi_idivmod");
        NFuncCall *gcc_func = new NFuncCall(func_id, exp_list);
        gcc_func->accept(this);
        return;
    }
    // 被X数地址
    string r1 = genExpr(binary_op->lhs_);
    // X数地址
    string r2 = genExpr(binary_op->rhs_);
    string inst = "nop";
    switch (binary_op->op_)
    {
    case '+':
        inst = "add";
        break;
    case '-':
        inst = "sub";
        break;
    case '*':
        inst = "mul";
        break;
    //下面几种比较大小，这个节点的运算会返回1或0两种值代表true或false
    case '<':
        inst = "cmp";
        cur_state = "lt";
        break;
    case '>':
        inst = "cmp";
        cur_state = "gt";
        break;
    case LE:
        inst = "cmp";
        cur_state = "le";
        break;
    case GE:
        inst = "cmp";
        cur_state = "ge";
        break;
    case EQ:
        inst = "cmp";
        cur_state = "eq";
        break;
    case NE:
        inst = "cmp";
        cur_state = "ne";
        break;
    }
    // 对于一般操作指令
    if (inst != "cmp")
    {
        Emit(inst, r1, r1, r2);
        //new Emit(inst, getRegName(r1), getRegName(r1), getRegName(r2));
        // 释放源寄存器用于之后的计算，暂时保留r4
        //new regs[getRegIndex(r2)].is_used = false;
        reg_manager.freeReg(getRegIndex(r2));
    }
    else
    {
        Emit(inst, r1, r2);
        //new Emit(inst, getRegName(r1), getRegName(r2));
        // 经过cmp设置标识状态后没用了，直接free掉
        //new regs[getRegIndex(r1)].is_used = false;
        reg_manager.freeReg(getRegIndex(r1));
        //new regs[getRegIndex(r2)].is_used = false;
        reg_manager.freeReg(getRegIndex(r2));
    }
    // 表示当前存值寄存器
    cur_str_reg = r1;
    //cur_str_reg = getRegName(r1);
}
// 处理标识符
void Generator::visitIdent(NIdent *ident)
{
    NIdent *id = symtbl->findVar(ident->name_);
    //new
    regName r = reg_manager.serchIdent(id->name_);
    if (r != regName::NOT_FND)
    {
        printf("LRU: %s\n", ident->name_.c_str());
        cur_str_reg = getRegName(r);
        return;
    }
    //end new
    // ident值的归宿
    int i;
    // 对于栈空间中的变量
    if (id->loc_->isInMemory())
    {
        //  对于栈空间中非数组变量
        if (!id->type_->isArray())
        {
            //new i = getReg();
            i = (int)reg_manager.getReg();
            // if (to_debug) {
            //     int tmp = getTempReg();
            //     Emit("ldr", regs[tmp].name, "=#" + to_string(id->loc_->getMemOff()));
            //     Emit("add", regs[tmp].name, "fp", regs[tmp].name);
            //     Emit("ldr", regs[i].name, regs[tmp].name, true);
            //     regs[tmp].is_used = false;
            // } else {
            Emit("ldr", regs[i].name, id->loc_->getLocation());
            reg_manager.regWrite((regName)i, regType::IDENT, id->name_);
            // }
            cur_str_reg = regs[i].name;
            printf("<%s, %d, %s>\n", id->name_.c_str(), i, cur_str_reg.c_str());
            return;
        }
        //new i = getReg();
        i = (int)reg_manager.getReg();
        // 取得地址
        if (id->is_addr)
        {
            // if(to_debug) {
            //     // 如果是数组参数，则传递为地址，应当ldr地址而不是直接在栈空间取
            //     int tmp = getTempReg();
            //     Emit("ldr", regs[tmp].name, "=#" + to_string(id->loc_->getMemOff()));
            //     Emit("add", regs[tmp].name, "fp", regs[tmp].name);
            //     Emit("ldr", regs[i].name, regs[tmp].name, true);
            //     regs[tmp].is_used = false;
            // } else {
            Emit("ldr", regs[i].name, id->loc_->getLocation());
            // }
        }
        else
        {
            // 如果不是指针参数，则直接在栈空间中加载地址
            //new int temp = getReg();
            int temp = (int)reg_manager.getReg();
            Emit("ldr", regs[temp].name, "=#" + to_string(id->loc_->getMemOff()));
            Emit("add", regs[i].name, "fp", regs[temp].name);
            //new regs[temp].is_used = false;
            reg_manager.freeReg(temp);
        }
        // 加上偏移量得到元素地址
        int index_reg = genVarArrayIndex(ident);
        if (index_reg != -1)
        {
            Emit("add", regs[i].name, regs[i].name, regs[index_reg].name);
            //new regs[index_reg].is_used = false;
            reg_manager.freeReg(index_reg);
        }
        // 提取元素
        if (!isPassAddr(ident))
        {
            Emit("ldr", regs[i].name, regs[i].name, true);
        }
        // 表示当前存值寄存器
        cur_str_reg = regs[i].name;
        return;
    }
    // 如果在寄存器
    if (id->loc_->isInReg())
    {
        //new i = getReg();
        i = (int)reg_manager.getReg();
        Emit("mov", regs[i].name, id->loc_->getLocation());
        // 表示当前存值寄存器
        cur_str_reg = regs[i].name;
        reg_manager.regWrite((regName)i, regType::IDENT, id->name_);
        return;
    }
    // 对于全局变量
    // 如果没加，则加入函数所用的全局变量vector
    if (!findGloblData(id))
    {
        globl_data.push_back(id);
    }
    // 对于非数组变量,直接读取值
    if (!id->type_->isArray())
    {
        // 读入全局变量所在地址
        //new i = getReg();
        i = (int)reg_manager.getReg();
        Emit("ldr", regs[i].name, id->loc_->getLocation());
        // 从地址读取全局的值
        Emit("ldr", regs[i].name, regs[i].name, true);
        // 表示当前存值寄存器
        cur_str_reg = regs[i].name;
        reg_manager.regWrite((regName)i, regType::IDENT, id->name_);
        return;
    }
    // 加载数组首部地址
    //new i = getReg();
    i = (int)reg_manager.getReg();
    Emit("ldr", regs[i].name, id->loc_->getLocation());
    // 根据下标获得所取元素在数组的偏移
    int index_reg = genVarArrayIndex(ident);
    if (index_reg != -1)
    {
        Emit("add", regs[i].name, regs[i].name, regs[index_reg].name);
        //new
        regs[index_reg].is_used = false;
        reg_manager.freeReg(index_reg);
    }
    // 取出元素
    if (!isPassAddr(ident))
    {
        Emit("ldr", regs[i].name, regs[i].name, true);
    }
    // 表示当前存值寄存器
    cur_str_reg = regs[i].name;
}

// 将常量值存入r4~r6，计算使用
void Generator::visitIntValue(NInt *int_val)
{
    //new int i = getReg();
    regName r = reg_manager.getReg();
    //new Emit("ldr", regs[i].name, "=#" + to_string(int_val->getValue()));
    Emit("ldr", getRegName(r), "=#" + to_string(int_val->getValue()));
    // 表示当前存值寄存器
    //new cur_str_reg = regs[i].name;
    cur_str_reg = getRegName(r);
}

// NArray线性化
vector<int> Generator::genArrayValue(NExpr *array)
{
    // 转换为NArray继承类指针
    NArray *array_expr = dynamic_cast<NArray *>(array);
    // DEBUG, 断言不为空
    if (!array_expr)
    {
        return vector<int>();
    }
    // 将数组线性化成vector<int>返回
    return array_expr->toVector();
}

int Generator::genVarArrayIndex(NIdent *arr_id)
{
    if (!arr_id->array_index_)
    {
        return -1;
    }
    // 符号表中原始变量
    NIdent *origin_id = symtbl->findVar(arr_id->name_);
    Type *tp = origin_id->type_.get();
    // 寄存器保存偏移量
    int r = -1;
    string des;
    NExpr *e = nullptr;
    for (int i = 0; i < arr_id->array_index_->size(); i++)
    {
        e = arr_id->array_index_->at(i);
        if (!e)
        {
            return -1;
        }
        TArray *arr_tp = dynamic_cast<TArray *>(tp);
        des = genExpr(e);
        // 多位数组乘以宽度在加
        if (arr_tp && arr_tp->base_type_->isArray())
        {
            //new int temp = getReg();
            int temp = (int)reg_manager.getReg();
            Emit("ldr", regs[temp].name, "=#" + to_string(arr_tp->getLen()));
            Emit("mul", des, des, regs[temp].name);
            //new regs[temp].is_used = false;
            reg_manager.freeReg((regName)temp);
            if (r == -1)
            {
                //new r = getTempReg();
                r = (int)reg_manager.getReg();
                Emit("mov", regs[r].name, des);
            }
            else
            {
                Emit("add", regs[r].name, regs[r].name, des);
            }
            //new regs[getRegIndex(des)].is_used = false;
            reg_manager.freeReg(getRegIndex(des));
            tp = arr_tp->base_type_.get();
            continue;
        }
        if (r == -1)
        {
            //new r = getTempReg();
            r = (int)reg_manager.getReg();
            Emit("mov", regs[r].name, des);
        }
        else
        {
            Emit("add", regs[r].name, regs[r].name, des);
        }
        //new regs[getRegIndex(des)].is_used = false;
        reg_manager.freeReg(getRegIndex(des));
    }
    Emit("lsl", regs[r].name, regs[r].name, 2);
    return r;
}

// 加载初始化数组到当前栈空间
void Generator::loadArrayIntoMemory(NAssign *assign)
{
    // 断定搬运进入栈空间的数组必须初始化
    // assert(assign->isInit());
    // 使用下述寄存器进行搬运操作
    //new int loader = getTempReg();
    int loader = (int)reg_manager.getReg();
    //new int saver = getTempReg();
    int saver = (int)reg_manager.getReg();
    // 加载数组地址
    Emit("ldr", regs[loader].name, assign->id_->loc_->rodata_->getLabel());
    // 加载栈空间地址
    Emit("mov", regs[saver].name, "sp");
    // 得到线性化后数组值
    vector<int> array_val = genArrayValue(assign->assign_val_);
    // 寄存器组
    string des;
    for (int i = array_val.size(); i > 0;)
    {
        // 判断所需需寄存器
        if (i >= 4)
        {
            des = "{r0, r1 , r2, r3}";
            i -= 4;
        }
        else if (i == 3)
        {
            des = "{r0, r1, r2}";
            i -= 3;
        }
        else if (i == 2)
        {
            des = "{r0, r1}";
            i -= 2;
        }
        else
        {
            des = "{r0}";
            i -= 1;
        }
        // 有数组所在空间载入寄存器
        Emit("ldmia", regs[loader].name + "!", des);
        // 有寄存器载入栈空间
        Emit("stmia", regs[saver].name + "!", des);
    }
    // 用完寄存器就扔掉
    //new regs[loader].is_used = false;
    //new regs[saver].is_used = false;
    reg_manager.freeReg(loader);
    reg_manager.freeReg(saver);
}
// lt
void Generator::visitUnaryOp(NUnaryOp *unary_op)
{
    //todo getReg():分配一个寄存器用来存值
    //String reg0=getReg();

    //todo unary_op->uhs()->getName():得到变量名
    //todo:emit需要‘=’
    //e.g. ldr r0,=var1;取地址到r0
    //Emit("ldr",reg0,unary_op->uhs_->getName());

    //String reg1=getReg();
    //todo:emit需要‘[]’
    //e.g. ldr r1,[r0];取值到r1
    //Emit("ldr",reg1,reg0);
    string r1 = genExpr(unary_op->uhs_);
    switch (unary_op->op_)
    {
    case '!':
        //Emit("mvn",reg1,reg1);
        //传送按位取反的值
        // 比较
        Emit("cmp", r1, 0);
        Emit("moveq", r1, 1);
        Emit("movne", r1, 0);
        break;
    case '-':
        //Emit("mvn",reg1,reg1);
        //Emit("add",reg1,reg1,#1);
        //取反+1
        Emit("rsb", r1, r1, 0);
        break;
    }
    // 表示当前存值寄存器
    cur_str_reg = r1;
    return;
    //todo:emit需要'[]'
    //e.g. str r1,[r0];保存变量值
    //todo:我应该把运算的值存到哪里？是否应该在符号表中存放一个临时变量tmpvar？
    //Emit("ldr",regtmp,tmpvar);
    //Emit("str",reg1,regtmp);
}

// ml
// Todo 如何防止调用清空数据
void Generator::visitFuncCall(NFuncCall *func_call)
{
    if (func_call->id_->name_ == "stoptime")
    {
        func_call->id_->name_ = "_sysy_stoptime";
    }
    if (func_call->id_->name_ == "starttime")
    {
        func_call->id_->name_ = "_sysy_starttime";
    }
    // reg_manager.regClear();
    vector<pair<int, int>> buf;
    // 如果传递参数
    if (func_call->arguments_)
    {
        // 存入参数: r0~r4 外加栈空间
        for (int i = 0; i < func_call->arguments_->size(); i++)
        {
            // 计算表达式并返回地址
            NExpr *e = func_call->arguments_->at(i);
            // 如果计算不出值则需要先在汇编中体现计算过程，然后将值所在寄存器移动进入参数寄存器
            string src = genExpr(e);
            // mov the value into r0...r3
            if (i < 4)
            {
                if (regs[i].is_used)
                {
                    //new int buf_src = getReg();
                    int buf_src = (int)reg_manager.getReg();
                    Emit("mov", regs[buf_src].name, regs[i].name);
                    buf.push_back({i, buf_src});
                    regs[i].is_used = false;
                    reg_manager.freeReg(i);
                }
                Emit("mov", regs[i].name, src);
                regs[i].is_used = true;
            }
            else
            {
                // 超过4参数，存在内存中
                cur_sp_pos -= 4;
                Emit("sub", "sp", "sp", 4);
                Emit("str", src, "sp", true);
            }
            regs[getRegIndex(src)].is_used = false;
            reg_manager.freeReg(getRegIndex(src));
        }
    }
    // 还就那个调用函数
    Emit("bl", func_call->id_->name_);
    // 函数调用结束后，释放参数寄存器
    for (int i = 0; i < 4; i++)
    {
        regs[i].is_used = false;
    }
    NFuncDef *func = symtbl->findFunc(func_call->id_->name_);
    // 针对库函数
    if (
        func_call->id_->name_ == "putint" || func_call->id_->name_ == "putchar" || func_call->id_->name_ == "putarray" || func_call->id_->name_ == "putch" || func_call->id_->name_ == "_sysy_stoptime" || func_call->id_->name_ == "_sysy_starttime" || func_call->id_->name_ == "memset")
    {
        ;
    }
    else if (!func || func->ret_type_->isInt())
    {
        // 获取寄存器，存储结果
        //new int i = getReg();
        int i = (int)reg_manager.getReg();
        if (func_call->id_->name_ == "__aeabi_idivmod")
        {
            Emit("mov", regs[i].name, "r1");
            regs[1].is_used = false;
        }
        else
        {
            Emit("mov", regs[i].name, "r0");
            regs[0].is_used = false;
        }
        for (int i = 0; i < buf.size(); i++)
        {
            Emit("mov", regs[buf.at(i).first].name, regs[buf.at(i).second].name);
            //new regs[buf.at(i).second].is_used = false;
            reg_manager.freeReg(buf.at(i).second);
            regs[buf.at(i).first].is_used = true;
        }
        buf.resize(0);
        // 表示当前存值寄存器
        cur_str_reg = regs[i].name;
    }
}

/***********
 *  Stmt   *
 **********/
void Generator::visitBlock(NBlock *block_node)
{
    // 创建该block下的子符号表
    symtbl = new HashMap(symtbl);
    // 嵌套深度 + 1
    now_block_level += 1;
    // 具有函数参数,将参数合并加入子符号表，同时清空参数符号表
    if (func_param_tbl->size() > 0) {
        symtbl->append(func_param_tbl);
        func_param_tbl->clear();
    }
    // gen block
    NStmt *stmt = nullptr;
    for (int i = 0; i < block_node->stmts_->size(); i++)
    {
        stmt = block_node->stmts_->at(i);
        stmt->accept(this);
    }
    // 跳出block，嵌套深度 -1
    now_block_level -= 1;
    // 访问结束后回到父符号表,同时启动自爆程序
    symtbl = symtbl->getFather();
    symtbl->pop();
}
// zyj
void Generator::visitAssign(NAssign *assign)
{
    // 给变量的值
    NExpr *value = assign->assign_val_;
    // 并没给出值的情况暂时不执行操作;
    if (!value)
    {
        return;
    }
    // 在符号表查找ident
    NIdent *id = symtbl->findVar(assign->id_->name_);
    // 查不到符号就假装报错
    if (!id)
    {
        fprintf(stderr, "Error: undefined symbol %s\n", assign->id_->name_.c_str());
        exit(EXIT_FAILURE);
    }
    // 数据地址
    string src = genExpr(assign->assign_val_);
    int r = (int)reg_manager.serchIdent(id->name_);
    // 如果ident还在寄存器，更新之
    if ((regName)r != regName::NOT_FND) {
        reg_manager.freeReg(r);
        reg_manager.regWrite((regName)getRegIndex(src), regType::IDENT, id->name_);
        // Emit("mov", regs[r].name, src);
    }
    // ident存储地址
    string des;
    bool str_to_mem = false;
    // 若指定ident在栈空间中，直接将值存入栈空间对应位置
    if (id->loc_->isInMemory())
    {
        if (id->type_->isArray())
        {
            // 获得下标对应偏移量,此处不用符号表的id，因为下标不同
            int index_reg = genVarArrayIndex(assign->id_);
            //new int r = getReg();
            int r = (int)reg_manager.getReg();
            // 获得对应元素地址 = 首元素地址 + 偏移量
            if (id->is_addr)
            {
                // if (to_debug) {
                //     int tmp = getTempReg();
                //     Emit("ldr", regs[tmp].name, "=#" + to_string(id->loc_->getMemOff()));
                //     Emit("add", regs[tmp].name, "fp", regs[tmp].name);
                //     Emit("ldr", regs[r].name, regs[tmp].name, true);
                //     regs[tmp].is_used = false;
                // } else {
                Emit("ldr", regs[r].name, id->loc_->getLocation());
                // }
            }
            else
            {
                //new int temp = getReg();
                int temp = (int)reg_manager.getReg();
                Emit("ldr", regs[temp].name, "=#" + to_string(id->loc_->getMemOff()));
                Emit("add", regs[r].name, "fp", regs[temp].name);
                //new regs[temp].is_used = false;
                reg_manager.freeReg((regName)temp);
            }
            // 加上偏移量
            Emit("add", regs[r].name, regs[r].name, regs[index_reg].name);
            // 加载数组元素地址
            des = regs[r].name;
            //new regs[index_reg].is_used = false;
            reg_manager.freeReg((regName)index_reg);
            str_to_mem = true;
        }
        else
        {
            // if (to_debug) {
            //     int tmp = getReg();
            //     Emit("ldr", regs[tmp].name, "=#" + to_string(id->loc_->getMemOff()));
            //     Emit("add", regs[tmp].name, "fp", regs[tmp].name);
            //     des = regs[tmp].name;
            //     str_to_mem = true;
            // } else {
            des = id->loc_->getLocation();
            // }
            // des = id->loc_->getLocation();
        }
        Emit("str", src, des, str_to_mem);
        if (str_to_mem)
        {
            //new regs[getRegIndex(des)].is_used = false;
            reg_manager.freeReg(getRegIndex(des));
        }
        //new regs[getRegIndex(src)].is_used = false;
        reg_manager.freeReg(getRegIndex(src));
        return;
    }
    // 对于全局变量
    if (!findGloblData(id))
    {
        globl_data.push_back(id);
    }
    int index_reg = genVarArrayIndex(assign->id_);
    //new int des_reg = getReg();
    int des_reg = (int)reg_manager.getReg();
    Emit("ldr", regs[des_reg].name, id->loc_->getLocation());
    // index_reg != -1 说明数组
    if (index_reg != -1) {
        Emit("add", regs[des_reg].name, regs[index_reg].name, regs[des_reg].name);
        //new regs[index_reg].is_used = false;
        reg_manager.freeReg(index_reg);
    }
    Emit("str", src, regs[des_reg].name, true);
    //new regs[des_reg].is_used = false;
    //new regs[getRegIndex(src)].is_used = false;
    reg_manager.freeReg(des_reg);
    reg_manager.freeReg(getRegIndex(src));
}
// tlx
void Generator::visitVarDecl(NVarDecl *var_decl)
{
    Assignlist *assign_list = var_decl->assign_list_;
    if (!assign_list)
    {
        return;
    }
    NAssign *assign = nullptr;
    // 如果为全局变量
    if (var_decl->is_global_)
    {
        genGlobalDecl(var_decl);
        return;
    }
    // 如果非全局变量
    int assign_list_size = assign_list->size();
    // 偏移量
    int offset = 0;
    for (int i = 0; i < assign_list_size; i++)
    {
        assign = assign_list->at(i);
        if (!var_decl->is_in_loop_ || loop_level == 0)
        {
            offset = assign->id_->type_->getWidth();
            // 标记变量位置
            //new int tmp = getTempReg();
            int tmp = (int)reg_manager.getReg();
            Emit("ldr", regs[tmp].name, "=#" + to_string(offset));
            Emit("sub", "sp", "sp", regs[tmp].name);
            cur_sp_pos -= offset;
            // 记录位置
            assign->id_->loc_ = new Location(cur_sp_pos);
            //new regs[tmp].is_used = false;
            reg_manager.freeReg(tmp);
            // 加入变量到符号表
            symtbl->push(assign->id_);
        }
        // Todo 该部分应为assign，由于比较特殊，暂时放在这里
        if (assign->id_->type_->isArray() && assign->isInit())
        {
            NArray *arr_val = dynamic_cast<NArray *>(assign->assign_val_);
            Exprlist vals = linify(arr_val);
            NIdent *ident = assign->id_;
            if (vals.size() * 4 < ident->type_->getWidth())
            {
                NIdent *id = new NIdent("memset");
                Exprlist *args = new Exprlist();
                args->push_back(assign->id_);
                args->push_back(new NInt(0));
                args->push_back(new NInt(assign->id_->type_->getWidth()));
                NFuncCall *call_memset = new NFuncCall(id, args);
                call_memset->accept(this);
            }
            for (int i = 0; i < vals.size(); i++)
            {
                string src = genExpr(vals.at(i));
                // 计算偏移量
                //new int index_reg = getTempReg();
                int index_reg = (int)reg_manager.getReg();
                Emit("ldr", regs[index_reg].name, "=#" + to_string(i));
                Emit("lsl", regs[index_reg].name, regs[index_reg].name, 2);
                // 获得首地址
                //new int r = getReg();
                int r = (int)reg_manager.getReg();
                Emit("ldr", regs[r].name, "=#" + to_string(assign->id_->loc_->getMemOff()));
                Emit("add", regs[r].name, "fp", regs[r].name);
                // 加上偏移量
                Emit("add", regs[r].name, regs[index_reg].name, regs[r].name);
                //new regs[index_reg].is_used = false;
                reg_manager.freeReg(index_reg);
                // 存入值
                Emit("str", src, regs[r].name, true);
                //new regs[r].is_used = false;
                //new regs[getRegIndex(src)].is_used = false;
                reg_manager.freeReg(r);
                reg_manager.freeReg(getRegIndex(src));
            }
            continue;
        }
        // 赋值
        assign->accept(this);
    }
}

Exprlist Generator::linify(NArray *list)
{
    Exprlist ret;
    NArray *arr_ptr = nullptr;
    for (int i = 0; i < list->value_.size(); i++)
    {
        arr_ptr = dynamic_cast<NArray *>(list->value_.at(i));
        if (arr_ptr)
        {
            Exprlist tmp = linify(arr_ptr);
            for (int i = 0; i < tmp.size(); i++)
            {
                ret.push_back(tmp.at(i));
            }
            tmp.resize(0);
            continue;
        }
        ret.push_back(list->value_.at(i));
    }
    return ret;
}
void Generator::genGlobalDecl(NVarDecl *var_decl)
{
    // assert(var_decl->is_global_);
    int size = var_decl->assign_list_->size();
    NAssign *assign = nullptr;
    int value;
    // // 对于常量而言
    // if(var_decl->type_->is_const_) {
    //     for (int i = 0; i < size; i++) {
    //         //tosolvece
    //         assign = var_decl->assign_list_->at(i);
    //         assign->id_->loc_ = new Location(new NLabelStmt());
    //         // assign = (*var_decl->assign_list_)[i];
    //         // 加入符号表
    //         symtbl->push(assign->id_);
    //         // 设id的type为const
    //         assign->id_->type_->is_const_ = true;
    //         Emit(".globl", assign->id_->name_);
    //         // 分配到指定区域
    //         Emit(".section", ".rodata");
    //         EmitLabel(assign->id_->name_);
    //         // 非数组量
    //         if (!assign->id_->type_->isArray()) {
    //             // 加入常量表
    //             int value = assign->assign_val_->getValue();
    //             const_data_map.insert(make_pair(assign->id_->name_, value));
    //             //fix const_data_map[assign->id_->name_] = value;
    //             Emit(".word", to_string(value));
    //         }
    //         else {
    //             // 数组常量
    //             vector<int> value = genArrayValue(assign->assign_val_);
    //             const_arr_map.insert(make_pair(assign->id_->name_, value));
    //             // 打印初始化的值
    //             for(int i = 0; i < value.size(); i++) {
    //                 Emit(".word", to_string(value.at(i)));
    //             }
    //         }
    //     }
    //     return;
    // }
    // 如果非const
    // 遍历转换声明的每个变量为ARM代码
    for (int i = 0; i < size; i++)
    {
        //tosolvece
        assign = var_decl->assign_list_->at(i);
        // 分配Label，存入变量的location属性中
        assign->id_->loc_ = new Location(new NLabelStmt());
        // 加入符号表
        symtbl->push(assign->id_);
        // 如果已经初始化
        if (assign->isInit())
        {
            Emit(".globl", assign->id_->name_);
            Emit(".data");
            Emit(".size", assign->id_->name_, to_string(assign->id_->type_->getWidth()));
            EmitLabel(assign->id_->name_);
            // 对于非数组变量
            if (!assign->id_->type_->isArray())
            {
                value = assign->assign_val_->getValue();
                Emit(".word", to_string(value));
            }
            else
            {
                // 对于数组变量，调用函数, 转换成一维vector
                vector<int> array_val = genArrayValue(assign->assign_val_);
                int width = assign->id_->type_->getWidth();
                // 对于 int a[10] = {1,2,3} 这种形式，即元素没有完全初始化,需要 .space填充
                for (int i = 0; i < array_val.size(); i++)
                {
                    Emit(".word", to_string(array_val.at(i)));
                }
                if (array_val.size() * 4 < width)
                {
                    Emit(".space", to_string(width - array_val.size() * 4));
                }
            }
            continue;
        }
        // 未初始化数据，放在.comm section里面
        Emit(
            ".comm",
            assign->id_->name_ + ", " + to_string(assign->id_->type_->getWidth()) + ", 4");
    }
}

// ml
void Generator::visitFuncDef(NFuncDef *func_def)
{
    reg_manager.regClear();
    // 将函数加入符号表
    symtbl->push(func_def);
    Emit(".text");
    // 现在暂时默认函数的linkage都是global的: .global <func_name>
    Emit(".globl", func_def->id_->name_.c_str());
    // thumb inst
    Emit(".arm");
    // 函数label
    EmitLabel(func_def->id_->name_.c_str());
    Emit("push", "{r4, r5, r6, r7, r8, r9, r10, fp, lr}");
    cur_sp_pos -= 36;
    Emit("mov", "fp", "sp");
    // 模拟sp,fp位置, 通过将sp赋值给fp
    cur_fp_pos = cur_sp_pos;
    // 栈空间大小，预留4
    int pad_size = 4;
    cur_sp_pos -= 4;
    // 获得参数空间大小
    for (int i = 0; i < func_def->params_->size(); i++)
    {
        NIdent *id = func_def->params_->at(i)->assign_list_->at(0)->id_;
        if (id->type_->isArray())
        {
            id->is_addr = true;
        }
        // 将函数参数加入参数符号表
        func_param_tbl->push(id);
        // 获得需要占用的栈空间大小
        int width = func_def->params_->at(i)->type_->getWidth();
        id->loc_ = new Location(cur_sp_pos);
        cur_sp_pos -= width;
        pad_size += width;
    }
    //new int tmp = getTempReg();
    int tmp = (int)reg_manager.getReg();
    Emit("ldr", regs[tmp].name, "=#" + to_string(pad_size));
    Emit("sub", "sp", "sp", regs[tmp].name);
    //new regs[tmp].is_used = false;
    reg_manager.freeReg((regName)tmp);
    // 保存函数参数
    for (int i = 0; i < func_def->params_->size(); i++)
    {
        NIdent *id = func_def->params_->at(i)->assign_list_->at(0)->id_;
        string pos = id->loc_->getLocation();
        if (i < 4)
        {
            Emit("str", regs[i].name, pos);
            regs[i].is_used = false;
        }
        else
        {
            //new int r = getReg();
            int r = (int)reg_manager.getReg();
            Emit("ldr", regs[r].name, "[fp, #" + to_string(4 * (func_def->params_->size() - i - 1) + 36) + "]");
            Emit("str", regs[r].name, pos);
            //new regs[r].is_used = false;
            reg_manager.freeReg((regName)r);
        }
    }
    // 函数body
    func_def->body_->accept(this);
    // 如果还没有return, （if return else return但没有外部return 例10_if_else.sy)
    if (pre_ret_label)
    {
        EmitLabel(pre_ret_label->getTag());
        pre_ret_label = nullptr;
        if (pre_data.length() > 0)
        {
            Emit("mov", "r0", pre_data);
            pre_data = "";
        }
    }
    Emit("mov", "sp", "fp");
    Emit("pop", "{r4, r5, r6, r7, r8, r9, r10, fp, pc}");
    // 恢复sp, fp 位置
    cur_sp_pos = cur_fp_pos;
    cur_sp_pos += 36;
    // 函数声明完成之后打印所需要的全局变量label
    for (int i = 0; i < globl_data.size(); i++)
    {
        NIdent *e = globl_data.at(i);
        if (e->loc_->label_->has_print == false)
        {
            EmitLabel(e->loc_->getLocation());
            e->loc_->label_->has_print = true;
            Emit(".word", e->name_);
        }
    }
    // 函数声明完成之后打印Rodata
    for (int i = 0; i < rodatas.size(); i++)
    {
        Rodata *e = rodatas.at(i);
        if (!e->val_)
        {
            continue;
        }
        EmitLabel(e->getLabel());
        Emit(".word", e->getTag());
        EmitLabel(e->getTag());
        vector<int> vals = genArrayValue(e->val_);
        for (int i = 0; i < vals.size(); i++)
        {
            Emit(".word", to_string(vals.at(i)));
        }
    }
    // 清除当前的rodatas
    rodatas.resize(0);
}
void Generator::visitIfStmt(NIfStmt *if_stmt)
{
    now_block_level++;
    Condition *cond = genCondition(if_stmt->cond_);
    // if的执行体label和结束label
    NLabelStmt if_label;
    NLabelStmt end_label;
    // 产生if的arm代码
    if (if_stmt->else_)
    {
        NLabelStmt else_label;
        // 由于有else，条件跳转应到跳到else标签
        cond->flushCondition(this, else_label, if_label);
        // else体
        EmitLabel(else_label.getTag());
        if_stmt->else_->accept(this);
        // 不存在提前跳转，则跳转到之后的语句
        // if (!pre_ret_label) {
        Emit("b", end_label.getTag());
        // }
        // if执行体，执行完了就直接跳到end
        EmitLabel(if_label.getTag());
        if_stmt->then_->accept(this);
        // 如果不存在提前return，才跳转到之后的语句，否则直接跳到return的标签
        // if (!pre_ret_label) {
        Emit("b", end_label.getTag());
        // }
        EmitLabel(end_label.getTag());
    }
    else
    {
        cond->flushCondition(this, end_label, if_label);
        // 产生if体
        EmitLabel(if_label.getTag());
        if_stmt->then_->accept(this);
        // 如果不存在提前return，才跳转到之后的语句，否则直接跳到return的标签
        // if (!pre_ret_label) {
        Emit("b", end_label.getTag());
        // }
        // if结束标签
        EmitLabel(end_label.getTag());
    }
    now_block_level--;
    return;
}
void Generator::visitWhileStmt(NWhileStmt *while_stmt)
{
    loop_level++;
    // 循环体开始label
    NLabelStmt begin_label;
    // 条件判断label
    NLabelStmt *cond_label = new NLabelStmt();
    code_begin_label.push(cond_label);
    // 循环体结束label
    NLabelStmt *end_label = new NLabelStmt();
    code_end_label.push(end_label);
    // 增加嵌套深度
    now_block_level++;
    Condition *cond = genCondition(while_stmt->cond_);
    EmitLabel(cond_label->getTag());
    cond->flushCondition(this, *end_label, begin_label);
    // 先直接跳到条件比较字段看是否满足条件
    // Emit("b", cond_label->getTag());
    // 循环体内
    EmitLabel(begin_label.getTag());
    while_stmt->do_->accept(this);
    // 每次执行完再次跳转到条件比较字段
    Emit("b", cond_label->getTag());
    // 循环体外
    // EmitLabel(cond_label->getTag());
    // // 直接flushCondition递归打印跳转代码
    // cond->flushCondition(this, *end_label, begin_label);
    // 如果满足条件则跳到循环体
    EmitLabel(end_label->getTag());
    // while结束嵌套深度 - 1
    now_block_level--;
    // 循环层数 - 1
    code_begin_label.pop();
    code_end_label.pop();
    loop_level--;
}
void Generator::visitJumpStmt(NJmpStmt *jmp_stmt)
{
    switch (jmp_stmt->jmp_type_)
    {
    case AST_CONTINUE_JMP:
        Emit("b", code_begin_label.top()->getTag());
        return;
    case AST_BREAK_JMP:
        Emit("b", code_end_label.top()->getTag());
        return;
    }
}

// 表达式
string Generator::genExpr(NExpr *expr)
{
    expr->accept(this);
    // 返回当前存值的寄存器
    return cur_str_reg;
}
// ml
void Generator::visitRetStmt(NRetStmt *ret_stmt)
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
        {
            string des = genExpr(ret_stmt->ret_val_);
            pre_data = des;
            //new regs[getRegIndex(des)].is_used = false;
            reg_manager.freeReg(getRegIndex(des));
        }
        Emit("b", pre_ret_label->getTag());
        return;
    }
    // 不存在提前跳转
    if (!pre_ret_label)
    {
        if (ret_stmt->ret_val_)
        {
            string des = genExpr(ret_stmt->ret_val_);
            Emit("mov", "r0", des);
            // regs[getRegIndex(des)].is_used = false;
            reg_manager.freeReg(getRegIndex(des));
        }
    }
    else
    {
        // 存在提前跳转需要先打印Label，然后写ret的语句
        if (ret_stmt->ret_val_)
        {
            string des = genExpr(ret_stmt->ret_val_);
        }
        EmitLabel(pre_ret_label->getTag());
        if (pre_data.length() > 0)
        {
            Emit("mov", "r0", pre_data);
            pre_data = "";
            pre_ret_label = nullptr;
        }
    }
}
void Generator::visitEmptyStmt(NEmptyStmt *empty_stmt)
{
    // for empty stmt like ';', we do nothing;
    ;
}
// zyj
void Generator::visitExprStmt(NExprStmt *expr_stmt)
{
    //对应的是stmt -> exp;
    //依靠子结点的具体信息去生成
    expr_stmt->exp_->accept(this);
}
