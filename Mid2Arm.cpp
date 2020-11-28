#include "genMid.hpp"
#include "genArm.hpp"
#include "mid2Arm.hpp"
using namespace std;
// 标记结尾
bool is_func_end = false;
// 中间代码表
extern midCodeList midCode;
// 输出文件
extern FILE* as_file;
// 符号表
extern HashMap* symtbl;
// 寄存器类
struct Register {
    // 你的名字
    string name;
    // 是否被占用
    bool is_used;
    Register(string reg_name) {
        this->name = reg_name;
        this->is_used = false;
    }
};
Register *regs[16] = {
    new Register("r0"), new Register("r1"), new Register("r2"), new Register("r3"),
    new Register("r4"), new Register("r5"), new Register("r6"), new Register("r7"),
    new Register("r8"), new Register("r9"), new Register("r10"), new Register("r11"),
    new Register("r12"), new Register("sp"), new Register("lr"), new Register("pc")
};

Register* getTempReg() {
    for (int i = 8; i <= 10; i++) {
        if (regs[i]->is_used == false) {
            regs[i]->is_used = true;
            return regs[i];
        }
    }
    fprintf(stderr, "no temp regs left\n");
    exit(EXIT_FAILURE);
}

Register* getReg() {
    for (int i = 4; i <= 7; i++) {
        if (regs[i]->is_used == false) {
            regs[i]->is_used = true;
            return regs[i];
        }
    }
    return getTempReg();
}

Register* getRegFromName(string reg_name) {
    for (int i = 0; i < 16; i++) {
        if (regs[i]->name == reg_name) {
            return regs[i];
        }
    }
    return NULL;
}
// 模拟sp，fp位置
static int sp_pos = 0;
static int fp_pos = 0;
//  输出代码函数
void Emit(string inst) {
    fprintf(as_file, "\t%s\n", inst.c_str());
    fflush(as_file);
}
// example: bx lr
void Emit(string op_, string des) {
    fprintf(as_file, "\t%s\t%s\n", op_.c_str(), des.c_str());
    //fflush(as_file);
}
// example: ldr r0, [r1, #4]
void Emit(string op_, string des, string src) {
    fprintf(
        as_file, "\t%s\t%s, %s\n",
        op_.c_str(),
        des.c_str(),
        src.c_str());
    fflush(as_file);
}

void Emit(string op_, string des, string src, bool is_in_memory) {
    if (is_in_memory) {
        src = '[' + src + ']';
    }
    Emit(op_, des, src);
}
void Emit(string op_, string des, int digit) {
    string src = "#" + to_string(digit);
    Emit(op_, des, src);
}
// example: ldr r0, r1, r2
void Emit(string op_, string des, string src, string second_op) {
    fprintf(
        as_file, "\t%s\t%s, %s, %s\n",
        op_.c_str(),
        des.c_str(),
        src.c_str(),
        second_op.c_str());
    fflush(as_file);
}
// example: sub sp, sp, #12
void Emit(string op_, string des, string src, int second_op) {
    string op2 = "#" + to_string(second_op);
    Emit(op_, des, src, op2);
}

void EmitLabel(string label_) {
    fprintf(as_file, "%s:\n", label_.c_str());
    fflush(as_file);
}

// 通过变量名判断 变量类型
enum class var_type {
    GLOBAL = 0, LOCAL, TMP
};

var_type getVarType(string name) {
    if(name.length() <= 0) {
        fprintf(stderr, "Error: empty var name!\n");
        exit(EXIT_FAILURE);
    }
    switch(name.at(0)) {
        case 'g': return var_type::GLOBAL;
        case 'a': return var_type::LOCAL;
        case 't': return var_type::TMP;
    }
    fprintf(stderr, "Error: illegal var name %s!\n", name.c_str());
    exit(EXIT_FAILURE);
}

/**
 *   思路
 *  1. 根据变量名称，首先在符号表查询原变量，如果无位置信息则需要 new Location并记录，
 * 否则根据位置读出数据并存入寄存器
 *  2. 如果是数值则直接移动到寄存器
 *  3. jmp语句直接 b + cmp条件 到指定的tag
 * */
/**
 * Todo list
 * 1. 在符号表中加入根据 t_1_2_3这种形式查询符号表获取原变量NIdent的功能
 * 
*/
void codeGen(NmidCode mid) {
    switch(mid.op) {
        // Binary Calculate
        case cal_op::ADD:
        case cal_op::SUB:
        case cal_op::MUL:
        case cal_op::DIV:
        case cal_op::MOD:
            printf("binary\n");
            genBinary(mid);
            break;
        // Unary Calculate
        case cal_op::NEG:
        case cal_op::NOT:
            printf("unary\n");
            genUnary(mid);
            break;
        // Assign
        case cal_op::COPY:
            printf("copy\n");
            genAssign(mid);
            break;
        case cal_op::COPY_AtoI:
            printf("atoi\n");
            genArrayToInt(mid);
            break;
        case cal_op::COPY_ItoA:
            printf("itoa\n");
            genIntToArray(mid);
            break;
        // Jmp
        case cal_op::GOTO_C:case cal_op::GOTO_U:
            printf("goto\n");
            genJmp(mid);
            break;
        // FuncCall
        case cal_op::FUNC_CALL:
            printf("func_call\n");
            genFuncCall(mid);
            break;
        case  cal_op::RETURN:
            printf("return\n");
            genRet(mid);
            break;
        case cal_op::INT_DECL:
            printf("int_decl\n");
            genIntDecl(mid);
            break;
        case cal_op::ARR_DECL:
            printf("arr_decl\n");
            genArrayDecl(mid);
            break;
        case cal_op::FUNC_DEF:
            printf("func_def\n");
            genFuncDef(mid);
            break;
        case cal_op::NOP:
            printf("nop\n");
            break;
        default:
            fprintf(stderr, "Error: illegal mid-code type!\n");
            exit(EXIT_FAILURE);
    }
}
// 生成Arm
void mid2ArmCode(midCodeList mid_code) {
    int n = mid_code.size();
    printf("mid_size: %d\n", n);
    for(int i = 0; i < n; i++) {
        printf("gen %d\n", i);
        codeGen(mid_code.at(i));
        // 函数结尾恢复状态
        if(is_func_end) {
            Emit("mov", "sp", "fp");
            Emit("pop", "{r4, r5, fp, pc}");
            // 恢复sp, fp 位置
            sp_pos = fp_pos;
            sp_pos += 16;
            is_func_end = false;
        }
    }
}

// 将名为id_name的变量放入寄存器并返回
Register* genIdent(string id_name) {
    NIdent* id = symtbl->findVar(id_name);
    Register* reg;
    // 临时变量
    if(id->loc_->isInReg()) {
        reg = getRegFromName(id->loc_->getLocation());
        // 临时变量随使用而消亡
        reg->is_used = false;
        return getRegFromName(id->loc_->getLocation());      
    }
    // 局部变量
    if(id->loc_->isInMemory()) {
        reg = getReg();
        // 非数组
        if(!id->type_->isArray()) {
            Emit("ldr", reg->name, id->loc_->getLocation());
            return reg;
        }
        Register* tmpReg;
        // 加载首地址
        if(id->is_addr) {
            Emit("ldr", reg->name, id->loc_->getLocation());
        } else {
            tmpReg = getReg();
            Emit("ldr", tmpReg->name, "=#" + to_string(id->loc_->getMemOff()));
            Emit("add", reg->name, "fp", tmpReg->name);
            tmpReg->is_used = false;
        }
        return reg;
    }
    // 全局变量
    reg = getReg();
    // 加载地址
    Emit("ldr", reg->name, id->loc_->getLocation());
    // 非数组则需要取值
    if(!id->type_->isArray()) {
        Emit("ldr", reg->name, reg->name, true);
    }
    return reg;
}
// 将rval放入寄存器并返回
Register* genRval(rVal value) {
    Register* reg;
    if(value.type == VINT) {
        reg = getReg();
        Emit("ldr", reg->name, "=#" + to_string(value.val));
        return reg;
    }
    reg = genIdent(value.ident);
    return reg;
}
void genBinary(NmidCode binary) {
    string lhs, rhs1, rhs2;
    Register* reg;
    // 被X数
    reg = genRval(binary.rVal1);
    rhs1 = reg->name;
    // X数
    reg = genRval(binary.rVal2);
    rhs2 = reg->name;
    // 运算
    NIdent* lVal = symtbl->findVar(binary.lVal);
    // 除法和取余 需要调用函数 特殊处理
    if(binary.op == cal_op::DIV) {
        Emit("mov", "r0", rhs1);
        Emit("mov", "r1", rhs2);
        Emit("bl", "__aeabi_idiv");
        Emit("mov", rhs1, "r0");
        // 释放寄存器
        getRegFromName(rhs2)->is_used = false;
        return;
    }
    if(binary.op == cal_op::MOD) {
        Emit("mov", "r0", rhs1);
        Emit("mov", "r1", rhs2);
        Emit("bl", "__aeabi_idivmod");
        Emit("mov", rhs1, "r1");
        getRegFromName(rhs2)->is_used = false;
        return;
    }
    string inst;
    switch (binary.op) {
        case cal_op::ADD: 
            inst = "add";
            break;
        case cal_op::SUB:
            inst = "sub";
            break;
        case cal_op::MUL:
            inst = "mul";
            break;
        default:
            break;
    }
    Emit(inst, rhs1, rhs1, rhs2);
    // 先假设 binary中 lval都是临时变量
    if (!lVal->loc_) {
        // 如果lval没有位置则需要设置为“被X数”的寄存器
        lVal->loc_ = new Location(rhs1);
    }
    getRegFromName(rhs2)->is_used = false;
}

void genUnary(NmidCode unary) {
    Register* reg;
    string rhs;
    // 取出值
    reg = genRval(unary.rVal1);
    rhs = reg->name;
    // 运算
    string inst;
    NIdent *lVal = symtbl->findVar(unary.lVal);
    // 先假设 Unary lval都是临时变量
    if (!lVal->loc_) {
        // lVal 临时变量位置为 rhs对应寄存器
        lVal->loc_ = new Location(reg->name);
    }
    // 取反
    if(unary.op == cal_op::NEG) {
        Emit("rsb", rhs, rhs, 0);
        return;
    }
    // 非
    Emit("cmp", rhs, 0);
    Emit("moveq", rhs, 1);
    Emit("movne", rhs, 0);
}

void genAssign(NmidCode assign) {
    Register* srcReg = genRval(assign.rVal1);
    // 查找变量位置
    NIdent* target = symtbl->findVar(assign.lVal);
    // 对产生临时变量,分配寄存器
    if(!target->loc_) {
        Register* tmp = getReg();
        target->loc_ = new Location(tmp->name);
        Emit("mov", tmp->name, srcReg->name);
        srcReg->is_used = false;
        return;
    }
    // 对栈空间变量
    if(target->loc_->isInMemory()) {
        Emit("str", srcReg->name, target->loc_->getLocation());
        srcReg->is_used = false;
        return;
    }
    // 对于全局变量
    Register* desReg = getReg();
    // 加载全局变量地址到寄存器
    Emit("ldr", desReg->name, target->loc_->getLocation());
    // 存值
    Emit("str", srcReg->name, desReg->name, true);
    // 释放所有寄存器
    srcReg->is_used = false;
    desReg->is_used = false;
}

void genArrayToInt(NmidCode assign) {
    // 数组首地址
    Register* reg = genIdent(assign.lVal);
    // 加载偏移量
    Register* tmp = genRval(assign.rVal1);
    // 得到元素地址
    Emit("lsl", tmp->name, tmp->name, 2);
    Emit("add", reg->name, reg->name, tmp->name);
    tmp->is_used = false;
    // 取出元素
    Emit("ldr", reg->name, reg->name, true);
    // 赋值
    NIdent* target = symtbl->findVar(assign.rVal2.ident);
    // 对临时变量
    if(!target->loc_) {
        tmp = getReg();
        target->loc_ = new Location(tmp->name);
        Emit("mov", tmp->name, reg->name);
        reg->is_used = false;
        return;
    }
    // 对局部变量
    if(target->loc_->isInMemory()) {
        Emit("str", reg->name, target->loc_->getLocation());
        reg->is_used = false;
        return;
    }
    // 对全局变量
    tmp = getReg();
    Register *desReg = getReg();
    // 加载全局变量地址到寄存器
    Emit("ldr", tmp->name, target->loc_->getLocation());
    // 存值
    Emit("str", reg->name, tmp->name, true);
    // 释放所有寄存器
    tmp->is_used = false;
    reg->is_used = false;
}

void genIntToArray(NmidCode assign) {
    // 值存入寄存器 src
    Register* src = genRval(assign.rVal2);
    // 数组首地址
    Register *reg = genIdent(assign.lVal);
    // 加载偏移量
    Register *tmp = genRval(assign.rVal1);
    // 得到数组元素地址
    Emit("lsl", tmp->name, tmp->name, 2);
    Emit("add", reg->name, reg->name, tmp->name);
    tmp->is_used = false;
    // 赋值
    Emit("str", src->name, reg->name, true);
    // 释放寄存器
    src->is_used = false;
    reg->is_used = false;
}

void genJmp(NmidCode jmp) {
    // 无条件跳转
    if(jmp.op == cal_op::GOTO_U) {
        Emit("b", jmp.to_tag);
        return;
    }
    // 条件跳转
    string flag = "";
    // 判断跳转条件
    switch(jmp.cmp) {
        case cmp_op::EQ: flag = "eq"; break;
        case cmp_op::G: flag = "gt"; break;
        case cmp_op::GE: flag = "ge"; break;
        case cmp_op::L: flag = "lt"; break;
        case cmp_op::LE: flag = "le"; break;
        case cmp_op::NE: flag = "ne"; break;
        default:
            fprintf(stderr, "Error: illegal cmp in jmp midCode!\n");
            exit(EXIT_FAILURE);
    }
    Emit("b" + flag, jmp.to_tag);
}

void genFuncCall(NmidCode call) {
    // 首先加载参数
    int param_size = call.paraList.size();
    Register* reg;
    for(int i = 0; i < param_size; i ++) {
        // 加载参数到 reg
        reg = genRval(call.paraList.at(i));
        // 载入参数寄存器 or 内存
        if (i < 4) {
            Emit("mov", regs[i]->name, reg->name);
            // regs[i]->is_used = true;
        } else {
            // 超过4参数，存在内存中
            sp_pos -= 4;
            Emit("sub", "sp", "sp", 4);
            Emit("str", reg->name, "sp", true);
        }
        // 释放寄存器
        reg->is_used = false;
    }
    // 还就那个调用函数
    Emit("bl", call.to_tag);
    // 保存值
    NFuncDef* func = symtbl->findFunc(call.to_tag);
    // void 就return
    if(!func || func->ret_type_->isVoid()) {
        return;
    }
    // 返回值所在寄存器
    string src = "r0";
    // 查找赋值对象
    NIdent *target = symtbl->findVar(call.lVal);
    // 局部变量
    if(target->loc_->isInMemory()) {
        Emit("str", src, target->loc_->getLocation());
        return;
    }
    // 临时变量
    if(target->loc_->isInReg()) {
        Emit("mov", target->loc_->getLocation(), src);
        getRegFromName(target->loc_->reg_)->is_used = true;
        return;
    }
    // 全局变量
    reg = getReg();
    Emit("ldr", reg->name, target->loc_->getLocation());
    Emit("str", src, reg->name, true);
    reg->is_used = false;
}

void genRet(NmidCode ret) {
    if(ret.tag.length() != 0) {
        EmitLabel(ret.tag);
    }
    // 加载值到 vreg寄存器
    Register* vreg = genRval(ret.rVal1);
    // 移动到r0 作为返回值
    if(ret.to_tag.length() == 0) {
        // 如果存在提前return先打印label
        Emit("mov", "r0", vreg->name);
        return;
    }
    // 提前return跳转到结尾
    Emit("b", ret.to_tag);
}
void genIntDecl(NmidCode int_decl){
    // 查找原变量
    NIdent* id = symtbl->findVar(int_decl.lVal);
    var_type type = getVarType(int_decl.lVal);
    // 如果是全局变量
    if(type == var_type::GLOBAL) {
        // 根据有无初始化，执行不同操作
        if(int_decl.is_init) {
            Emit(".globl", id->name_);
            Emit(".data");
            Emit(".size", id->name_, to_string(id->type_->getWidth()));
            EmitLabel(id->name_);
            Emit(".word", to_string(int_decl.rVal1.val));
        } else {
            // 未初始化数据，放在.comm section里面
            Emit(
                ".comm",
                id->name_ + ", " + to_string(id->type_->getWidth()) + ", 4"
            );
        }
        return;
    }
    // 局部变量
    Emit("sub", "sp", "sp", 4);
    // 分配位置
    sp_pos -= 4;
    id->loc_ = new Location(sp_pos);
    // 没有初始值，就结束了
    if(!int_decl.is_init) {
        return;
    }
    // 计算并保存值
    Register* reg = genRval(int_decl.rVal1);
    Emit("str", reg->name, id->loc_->getLocation(), true);
}

void genArrayDecl(NmidCode array_decl) {
    // 查找原变量
    NIdent *id = symtbl->findVar(array_decl.lVal);
    var_type type = getVarType(array_decl.lVal);
    // 全局变量
    if (type == var_type::GLOBAL) {
        // 根据有无初始化，执行不同操作
        if (array_decl.is_init) {
            Emit(".globl", id->name_);
            Emit(".data");
            Emit(".size", id->name_, to_string(id->type_->getWidth()));
            EmitLabel(id->name_);
            // 初始值
            int init_size = array_decl.paraList.size();
            for(int i = 0 ; i < init_size; i++) {
                Emit(".word", to_string(array_decl.paraList.at(i).val) );
            }
            // 如果初始化个数小于维度则调用.space预留空间
            int width = id->type_->getWidth();
            if(init_size * 4 < width) {
                Emit(".space", to_string(width - init_size * 4) );
            }
        } else {
            // 未初始化数据，放在.comm section里面
            Emit (
                ".comm",
                id->name_ + ", " + to_string(id->type_->getWidth()) + ", 4"
            );
        }
        return;
    }
    // 局部变量
    Register* reg = getReg();
    int width = id->type_->getWidth();
    Emit("ldr", reg->name, "=#" + to_string(width));
    Emit("sub", "sp", "sp", reg->name);
    sp_pos -= width;
    reg->is_used = false;
    // 分配位置
    id->loc_ = new Location(sp_pos);
    int init_size = array_decl.paraList.size();
    // 无初始值直接return
    if(init_size <= 0) {
        return;
    }
    // 首地址
    Register *tmp_reg = getReg();
    Emit("ldr", tmp_reg->name, "=#" + to_string(id->loc_->getMemOff()));
    Emit("add", tmp_reg->name, "fp", tmp_reg->name);
    // 值寄存器
    Register* vreg;
    for(int i = 0 ; i < init_size; i++) {
        reg = getReg();
        // 偏移量
        Emit("ldr", reg->name, "=#" + to_string(i * 4));
        // 元素地址
        Emit("add", reg->name, tmp_reg->name, reg->name);
        // 加载值
        vreg = genRval(array_decl.paraList.at(i));
        Emit("str", vreg->name, reg->name, true);
        reg->is_used = false;
        vreg->is_used = false;
    }
    tmp_reg->is_used = false;
}

void genFuncDef(NmidCode func_def) {
    printf("in\n");
    Emit(".text");
    // 现在暂时默认函数的linkage都是global的: .global <func_name>
    Emit(".globl", func_def.to_tag);
    Emit(".arm");
    // 原ident
    NIdent *id;
    // 函数名称
    EmitLabel(func_def.to_tag);
    Emit("push", "{r4, r5, fp, lr}");
    sp_pos -= 16;
    Emit("mov", "fp", "sp");
    fp_pos = sp_pos;
    // 预留空间
    int pad_size = 4;
    sp_pos -= 4;
    // 获取参数占用空间大小
    int param_size = func_def.paraList.size();
    for(int i = 0; i < param_size; i++) {
        NIdent *id = symtbl->findVar(func_def.paraList.at(i).ident);
        // param中的数组标明是指针，而不是数组首地址
        if (id->type_->isArray()) {
            id->is_addr = true;
        }
        // 获得需要占用的栈空间大小
        id->loc_ = new Location(sp_pos);
        sp_pos -= 4;
        pad_size += 4;
    }
    Register* reg = getReg();
    Emit("ldr", reg->name, "=#" + to_string(pad_size));
    Emit("sub", "sp", "sp", reg->name);
    reg->is_used = false;
    // 保存函数参数
    for (int i = 0; i < param_size; i++) {
        NIdent *id = symtbl->findVar(func_def.paraList.at(i).ident);
        string pos = id->loc_->getLocation();
        // 小于4个参数,参数来自r0~r3,否则来自内存
        if (i < 4) {
            Emit("str", regs[i]->name, pos);
            regs[i]->is_used = false;
        } else {
            Register* r = getReg();
            Emit("ldr", r->name, "[fp, #" + to_string(4 * (param_size - i - 1) + 16) + "]");
            Emit("str", r->name, pos);
            r->is_used = false;
        }
    }
    printf("out\n");
    is_func_end = true;
}