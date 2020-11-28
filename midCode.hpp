#ifndef MID_CODE_HPP
#define MID_CODE_HPP
#include <string>
#include <vector>
#include <map>
#include <set>
#define VINT true
#define VIDENT false
using namespace std;

enum class cal_op {
    ADD = 0, SUB, MUL, DIV, MOD, NEG, NOT, COPY, COPY_AtoI, COPY_ItoA, GOTO_U, GOTO_C, FUNC_CALL, FUNC_DEF, RETURN, INT_DECL, ARR_DECL, NOP
};

enum class cmp_op {
    EQ = 0, NE, G, L, GE, LE
};

struct rVal
{
    bool type;      // type可取 VINT 和 VIDENT
    int val;
    string ident;
};


// 单句中间代码结构体
struct NmidCode {
    // op = ADD, SUB, MUL, DIV, MOD, lVal = rVal1 op rVal2
    // op = NEG, lVal = -rVal1
    // op = NOT, lVal = !rVal1
    // op = COPY 时, lVal = rVal1
    // op = COPY_AtoI 时，rVal2 = lVal[rVal1]
    // op = COPY_ItoA, 时，lVal[rVal1] = rVal2
    // op = GOTO_U 时，为无条件跳转，跳转的标号为 to_tag
    // op = GOTO_C 时，为有条件跳转，条件在cmp中
    // op = FUNC_CALL 时，为函数调用语句，函数名为to_tag，参数在 paraList 中，返回值存在 lVal 中
    // op = FUNC_DEF 时，为函数声明语句，函数名为to_tag，参数名在 paraList 中
    // op = RETURN 时，为函数返回语句，返回值在rVal1中
    // op = INT_DECL 时，为int声明语句，其他与COPY一致
    // op = ARR_DECL 时，lVal 为数组名，rVal1为数组大小
    cal_op op;
    // rVal1 cmp rVal2
    // cmp取值：EQ等于，NE不等，G大于，L小于，GE大于等于，LE小于等于
    cmp_op cmp;
    // 左值
    string lVal; 
    // 标号
    string tag;
    // 右值
    rVal rVal1, rVal2;
    // 跳转标号
    string to_tag;
    // DECL 是否初始化
    bool is_init;
    // 当 op = FUNC_CALL 时，存参数
    // 当 op = ARR_DECL 时，存初始值们
    // 当 op = FUNC_DEF 时，存函数参数名
    vector<rVal> paraList;
};

// 中间代码
typedef vector<NmidCode> midCodeList;

// 基本块结构体
struct NBasicBlock {
    // 基本块的起始位置指针
    NmidCode *start, *end;
    // 该基本块会跳往的基本块
    NBasicBlock *next, *jmp;
};

// 流图
typedef vector<NBasicBlock> flowGraph;

// 表达式右值的最近修改信息 的 结构体
struct NchangeInfo {
    // 最近修改(last change)的位置
    int rVal1_lc, rVal2_lc;
    // 最近修改位置的可能性，当 pos > 1 时，不能用于寻找公共子表达式
    int pos_r1, pos_r2;
    NchangeInfo() : pos_r1(0), pos_r2(0) {};
};

// 表达式右值的最近修改信息
typedef vector<NchangeInfo> changeInfoList;

struct commonExprInfo {
    // 公共子表达式ID
    int commonExprID;
    // 对应值所储存的标识符
    string ident;
};

void midCodeOutput(midCodeList *midCode);
#endif // MID_CODE_HPP