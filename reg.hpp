#include <string>
#include <list>
using namespace std;

enum class regType
{
    IDENT, OTHER
};

enum class regName
{
    R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, NOT_FND = -1
};

struct regInfo
{
    regType type;
    string ident;
};

class reg
{
public:
    regInfo regs[11];
    list<int> free, distributed, keep;

    reg();
    ~reg() {};
    regName getReg();
    void freeReg(regName _reg);
    void freeReg(int _reg);
    regName serchIdent(string ident);
    void regWrite(regName _reg, regType _type, string _ident);
    void identChange(string ident);
    void regClear();
};

reg::reg()
{
    for(int i = 0; i < 11; i++)
    {
        regs[i].type = regType::OTHER;
        regs[i].ident.clear();
    }

    for(int i = 4; i < 11; i++)
        free.push_back(i);
}

// 获取一个reg，将它标记为已分配，如无空寄存器返回 regName::NOT_FND
regName reg::getReg()
{
    if(!free.empty())
    {
        int regNo = free.front();
        free.pop_front();
        distributed.push_back(regNo);
        regs[regNo].type = regType::OTHER;
        regs[regNo].ident.clear();
        return (regName)regNo;
    }
    else if(!keep.empty())
    {
        int regNo = keep.front();
        keep.pop_front();
        distributed.push_back(regNo);
        regs[regNo].type = regType::OTHER;
        regs[regNo].ident.clear();
        return (regName)regNo;
    }
    else {
        fprintf(stderr, "Register: no one left!\n");
        return regName::NOT_FND;
    }
}

// 使用完reg后进行释放，释放时如果寄存器中是变量值，会自动保留
void reg::freeReg(regName _reg)
{
    list<int>::iterator it;
    int regNo = static_cast<int>(_reg);
    for(it = distributed.begin(); it != distributed.end(); it++) {
        if(*it == regNo)
        {
            distributed.erase(it);
            if(regs[regNo].type == regType::IDENT)
            {
                for(it = keep.begin(); it != keep.end(); it++)
                {
                    if(regs[*it].ident == regs[regNo].ident)
                    {
                        regs[*it].ident.clear();
                        regs[*it].type = regType::OTHER;
                        free.push_back(*it);
                        keep.erase(it);
                        break;
                    }
                }
                keep.push_back(regNo);
            }
            else
                free.push_back(regNo);
            break;
        }
    }
    
}

void reg::freeReg(int _reg) {
    list<int>::iterator it;
    int regNo = _reg;
    for (it = distributed.begin(); it != distributed.end(); it++) {
        if (*it == regNo)
        {
            distributed.erase(it);
            if (regs[regNo].type == regType::IDENT)
            {
                for (it = keep.begin(); it != keep.end(); it++)
                {
                    if (regs[*it].ident == regs[regNo].ident)
                        keep.erase(it);
                }
                keep.push_back(regNo);
            }
            else
                free.push_back(regNo);
            break;
        }
    }
}
// 查找一个变量是否在reg中，如有，返回 regName::RX；没有则返回 regName::NOT_FND
// 如查找到，寄存器会被标志成已分配，用完后记得freeReg
regName reg::serchIdent(string ident)
{
    list<int>::iterator it;

    for(it = keep.begin(); it != keep.end(); it++)
        if(regs[*it].type == regType::IDENT && regs[*it].ident == ident)
        {
            int ret = *it;
            keep.erase(it);
            distributed.push_back(ret);
            return (regName)ret;
        }

    return regName::NOT_FND;
}

/*向reg写入数据，type取 IDENT 时才需要写string字段，type取 OTHER 时只需写成NULL
对于运算指令，要用regWrite更改 运算结果存入的寄存器 的内容

如 a = b + c，（寄存器中都不存在b，c的情况下），getReg得到R4, R5, R6, R4存b，R5存c，执行ADD R6, R4, R5后，
需要用regWrite把R6的type变成IDENT，ident改成a，然后str到内存后，free掉所用的这三个寄存器
如果是类似于 a = a + 1，一定只能申请一个寄存器，否则可能产生两个寄存器都存着同一个变量的新旧两个值，
free的时候由于顺序不同，可能会影响寄存器中变量的值（寄存器中保存变量的策略是新的覆盖旧的）

当然，如果左值是临时变量，那就把type变成OTHER，不要free

regWrite 只能用于标记为 已分配 的寄存器*/
void reg::regWrite(regName _reg, regType _type, string _ident)
{
    int regNo = static_cast<int>(_reg);
    regs[regNo].type = _type;
    if(_type == regType::IDENT)
        regs[regNo].ident = _ident;
}

// 清空寄存器
void reg::regClear()
{
    while(!free.empty())
        free.pop_back();
    
    while(!distributed.empty())
        distributed.pop_back();
    
    while(!keep.empty())
        keep.pop_back();

    for(int i = 4; i < 11; i++)
    {
        regs[i].type = regType::OTHER;
        regs[i].ident.clear();
        free.push_back(i);
    }
        
}
