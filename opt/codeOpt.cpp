#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <vector>
#include <fstream>
#include <memory>
#include <set>
#include <unordered_map>
#include <regex>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include "codeOpt.h"
#include "FlexLexer.h"

#define SHOW_RECEIVE_TOKEN()              \
    for (auto element : receiveToken)     \
    std::cout << (*element).token << "\n" \
              << (*element).value << "\n" \
              << std::endl
#define YYLESS(t) tokenIndex -= t
#define RETURN_HAS_BODY_BLOCK(blockType, newObject)   \
    std::shared_ptr<funcBody> body(new funcBody());   \
    std::shared_ptr<blockType> result(new newObject); \
    if (deepLevel == 0)                               \
        optCodegener.push_back(result);               \
    tempReceiver.clear();                             \
    bodyAppend(body);                                 \
    return result

#define MAKE_IDENT(t) std::string("\t") * (deepLevel - t)
#define OUTPUT_IDENT() std::cout << ident
#define NOT_FOUND INT32_MAX
#define NOT_A_NUMBER INT32_MAX
#define UNABLE_CAL INT32_MAX

std::regex numberPattern{"[ \t]*([1-9]*[0-9]+)|(0[0-7]*)|(0[xX][0-9a-fA-F]*)[ \t]*"};
std::regex numCalPattern{"[0-9+\\(\\)-\\\\*/\\\\% \t]+"};

std::regex funcDeclPattern{"[ \t]*(int|void)[ \t]+[a-zA-Z][a-zA-Z0-9]*[ \t]*\\(.*\\)[ \t]*\\{[ \t]*"};
std::regex initDeclVarPattern{"[ \t]*int[ \t]+[a-zA-Z][a-zA-Z0-9]*[ \t]*=[ \t]*[0-9a-zA-Z+\\(\\)-\\\\*/\\\\% \t\\[\\]]+;[ \t]*"};
std::regex noInitDeclVarPattern{"[ \t]*int[ \t]+[a-zA-Z][a-zA-Z0-9]*[ \t]*;[ \t]*"};
std::regex constDeclVarPattern{"[ \t]*const[ \t]+int[ \t]+[a-zA-Z][a-zA-Z0-9]*[ \t]*=[ \t]*[0-9a-zA-Z+\\(\\)-\\\\*/ \t\\[\\]]+;[ \t]*"};
std::regex varAssignPattern{"[ \t]*[0-9a-zA-Z+\\(\\)-\\\\*/\\\\% \t\\[\\]]+[ \t]*=[0-9a-zA-Z+\\(\\)-\\\\*/\\\\% \t\\[\\]]+;[ \t]*"};
std::regex varComparePattern{"[ \t]*[0-9a-zA-Z+\\(\\)-\\\\*/\\\\% \t\\[\\]]+[ \t]*([><]|>=|<=)[ \t]*[0-9a-zA-Z+\\(\\)-\\\\*/\\\\% \t\\[\\]]+;[ \t]*"};
std::regex whilePattern{"[ \t]*while[ \t]*\\(.*\\)[ \t]*\\{[ \t]*"};
std::regex ifPattern{"[ \t]*if[ \t]*\\(.*\\)[ \t]*[ \t]*"};
std::regex returnPattern{"[ \t]*return[ \t]*.*[ \t]*;[ \t]*"};
std::regex arrayInitDeclPattern{"[ \t]*(const){0,1}[ \t]*int[ \t]*[a-zA-Z][A-Z0-9a-z]*[ \t]*\\[.*\\][ \t]*=[ \t]*\\{.*\\}[ \t]*;[ \t]*"};
std::regex arrayNoInitDeclPattern{"[ \t]*int[ \t]*[a-zA-Z][A-Z0-9a-z]*[ \t]*(\\[.*\\])*[ \t]*;[ \t]*"};
std::regex declListPattern{"[ \t]*(const){0,1}[ \t]*int[ \t]*[0-9a-zA-Z+\\(\\)-\\\\*\\\\% \t\\[\\],]+;[ \t]*"};
std::regex funcCallPattern{"[ \t]*[a-z][A-Z0-9a-z]*[ \t]*\\(.*\\)[ \t]*;[ \t]*"};
std::regex innerBlockPattern{"[ \t]*\\{[ \t]*"};

const std::regex reducePatternArray[] = {
    funcDeclPattern,
    constDeclVarPattern,
    initDeclVarPattern,
    noInitDeclVarPattern,
    varAssignPattern,
    varComparePattern,
    whilePattern,
    ifPattern,
    returnPattern,
    arrayInitDeclPattern,
    arrayNoInitDeclPattern,
    declListPattern,
    funcCallPattern,
    innerBlockPattern};
const int reducePatternArraySize = 14;
int deepLevel = 0;

std::string &replace_all(std::string &str, const std::string &old_value, const std::string &new_value);
std::string cutString(std::string cuttedStr, char start, char end, bool isConsistEnd);

typedef int Coefficient;
typedef std::multiset<std::string> Polynomial;
struct Cmp
{
    bool operator()(const Polynomial &a, const Polynomial &b) const
    {
        if (a.size() < b.size()) //次数高的放前面
            return false;
        else if (a.size() > b.size())
            return true;
        else
            return a < b; //次数相同，字典序小的放前面
    }
};
typedef std::map<Polynomial, Coefficient, Cmp> Polynomials;
typedef std::pair<std::string, int> E_Value_Pair;
class calculator
{
public:
    int solution(std::string expression, std::vector<std::string> &evalvars, std::vector<int> &evalints)
    {
        int res = getIntRes(basicCalculator(expression, evalvars, evalints));
        evalvars.clear();
        evalints.clear();
        return res;
    }

    int getIntRes(std::vector<std::string> strVec)
    {
        if (strVec.size() == 1 && std::regex_match(strVec[0], numberPattern))
            return std::stoi(strVec[0]);
        else if (strVec.size() == 0)
            return 0;
        return NOT_A_NUMBER;
    }

    std::vector<std::string> basicCalculator(std::string expression, std::vector<std::string> &evalvars, std::vector<int> &evalints)
    {
        std::stack<Polynomials> opd; //放操作数
        std::stack<char> opr;        //放运算符
        char c;
        char pre_opr;
        char pre_c = '#';
        expression += "#";
        std::unordered_map<std::string, int> e_value_map;
        for (int i = 0; i < evalvars.size(); i++)
        {
            e_value_map.insert(std::pair<std::string, int>(evalvars[i], evalints[i]));
        }

        //opd.push(Polynomials());
        opr.push('#');
        int max_len = expression.size();

        for (int i = 0; i < max_len; i++)
        {
            c = expression[i];
            pre_opr = opr.top();

            if (c == '+' || c == '-' || c == ')' || c == '#') //要考虑作为数字的一元运算符
            {
                while (pre_opr == '+' || pre_opr == '-' || pre_opr == '*' || pre_opr == '/' || pre_opr == '%')
                {
                    opr.pop();
                    if (pre_opr == '+')
                    {
                        auto top = opd.top();
                        if (pre_c != '#' && pre_c != '(') //只有在作为二元运算符的时候才考虑
                        {
                            opd.pop();
                            add(opd.top(), top);
                        }
                    }
                    else if (pre_opr == '-')
                    {
                        auto top = opd.top();
                        if (pre_c != '#' && pre_c != '(') //2元运算
                        {
                            opd.pop();
                            sub(opd.top(), top);
                        }
                        else
                        {
                            multi(opd.top(), Polynomials{{Polynomial{""}, -1}}); //相当于乘上-1
                        }
                    }
                    else if (pre_opr == '*')
                    {
                        auto top = opd.top();
                        opd.pop();
                        multi(opd.top(), top);
                    }
                    else if (pre_opr == '/')
                    {
                        auto top = opd.top();
                        opd.pop();
                        div(opd.top(), top);
                    }
                    else if (pre_opr == '%')
                    {
                        auto top = opd.top();
                        opd.pop();
                        mod(opd.top(), top);
                    }
                    pre_opr = opr.top();
                }
                if (pre_opr == '(' && c == ')')
                    opr.pop();
                else
                    opr.push(c);
            }
            else if (c == '*' || c == '/' || c == '%')
            {
                while (pre_opr == '*' || pre_opr == '/')
                {
                    if (pre_opr == '*')
                    {
                        auto top = opd.top();
                        opd.pop();
                        multi(opd.top(), top);
                    }
                    else if (pre_opr == '/')
                    {
                        auto top = opd.top();
                        opd.pop();
                        div(opd.top(), top);
                    }
                    else if (pre_opr == '%')
                    {
                        auto top = opd.top();
                        opd.pop();
                        mod(opd.top(), top);
                    }
                    opr.pop();
                    pre_opr = opr.top();
                }
                opr.push(c);
            }
            else if (c >= '0' && c <= '9') //常数项作为空串
            {
                int start = i;
                int len = 0;
                for (; i < max_len && expression[i] >= '0' && expression[i] <= '9'; i++, len++)
                    ;
                std::string str = expression.substr(start, len);
                int num;
                sscanf(str.c_str(), "%d", &num);
                //if(num!=0)
                opd.push(Polynomials{{Polynomial(), num}}); //常数项不应该设为0
                i--;
            }
            else if (c >= 'a' && c <= 'z') //处理单个表达式
            {
                int start = i;
                int len = 0;
                for (; i < max_len && expression[i] >= 'a' && expression[i] <= 'z'; i++, len++)
                    ;
                std::string name = expression.substr(start, len);
                std::unordered_map<std::string, int> Num;
                auto iter = e_value_map.find(name);
                if (iter == e_value_map.end()) //非常数项
                {
                    opd.push(Polynomials{{Polynomial{name}, 1}});
                }
                else //在 当作常数项
                {
                    //if(iter->second!=0)
                    opd.push(Polynomials{{Polynomial(), iter->second}});
                }
                i--;
            }
            else if (c == '(')
            {
                opr.push('(');
                //opd.push(Polynomials());
            }
            if (c != ' ')
                pre_c = c;
        }
        //print(opd.top());
        if (opd.empty())
            return std::vector<std::string>();
        else
            return get_result(opd.top());
    }
    void add(Polynomials &a, const Polynomials &b) //把b都放到a里面
    {
        for (auto p : b)
        {
            auto iter = a.find(p.first);
            if (iter == a.end())
            {
                a.insert(p);
            }
            else if (iter->second + p.second == 0)
            {
                a.erase(iter);
            }
            else
            {
                iter->second += p.second;
            }
        }
    }

    void sub(Polynomials &a, const Polynomials &b)
    {
        for (auto p : b)
        {
            auto iter = a.find(p.first);
            if (iter == a.end())
            {
                p.second = -p.second;
                a.insert(p);
            }
            else if (iter->second - p.second == 0)
            {
                a.erase(iter);
            }
            else
            {
                iter->second -= p.second;
            }
        }
    }
    void multi(Polynomials &a, const Polynomials &b)
    {
        Polynomials ret;
        for (auto pb : b)
        {
            if (pb.second == 0)
                continue;
            for (auto pa : a)
            {
                if (pa.second == 0)
                    continue;
                Coefficient co = pa.second * pb.second;
                Polynomial tmp = pa.first;
                //merge函数是c++17的，所以得自己实现
                for (auto word : pb.first)
                {
                    tmp.insert(word);
                }

                add(ret, Polynomials{{tmp, co}});
            }
        }
        a = ret;
    }
    void div(Polynomials &a, const Polynomials &b)
    {
        Polynomials ret;
        for (auto pb : b)
        {
            if (pb.second == 0)
                continue;
            for (auto pa : a)
            {
                if (pa.second == 0)
                    continue;
                Coefficient co = pa.second / pb.second;
                Polynomial tmp = pa.first;
                //merge函数是c++17的，所以得自己实现
                for (auto word : pb.first)
                {
                    tmp.insert(word);
                }

                add(ret, Polynomials{{tmp, co}});
            }
        }
        a = ret;
    }
    void mod(Polynomials &a, const Polynomials &b)
    {
        Polynomials ret;
        for (auto pb : b)
        {
            if (pb.second == 0)
                continue;
            for (auto pa : a)
            {
                if (pa.second == 0)
                    continue;
                Coefficient co = pa.second % pb.second;
                Polynomial tmp = pa.first;
                //merge函数是c++17的，所以得自己实现
                for (auto word : pb.first)
                {
                    tmp.insert(word);
                }

                add(ret, Polynomials{{tmp, co}});
            }
        }
        a = ret;
    }
    void print(Polynomials &ps)
    {
        for (auto p : ps)
        {
            std::cout << p.second;
            for (auto little_p : p.first)
            {
                std::cout << "*" << little_p.c_str();
            }

            std::cout << std::endl;
        }
    }
    std::vector<std::string> get_result(Polynomials &ps)
    {
        std::vector<std::string> ret;
        for (auto p : ps)
        {
            std::string line;
            //cout << p.second;
            if (p.second != 0)
                line += std::to_string(p.second);
            for (auto little_p : p.first)
            {
                //cout << "*" << little_p.c_str();
                line += "*" + little_p;
            }
            if (!line.empty())
                ret.push_back(line);
            //cout << endl;
        }
        return ret;
    }
};

std::string operator*(const std::string &s, size_t n)
{
    std::string result;
    result.reserve(s.size() * n);
    for (size_t i = 0; i < n; ++i)
    {
        result += s;
    }
    return result;
}

int str2int(std::string num)
{
    int powVal = 0;
    int sum = 0;
    if (std::regex_search(num, std::regex{"0x"}) || std::regex_search(num, std::regex{"0X"}))
    {
        replace_all(num, "0x", "");
        replace_all(num, "0X", "");
        for (int index = num.size() - 1; index >= 0; --index)
        {
            if (num[index] == '-')
                return sum;
            sum += pow(16, powVal++) * std::stoi(std::string(1, num[index]));
        }
    }
    else if (num[0] == '0' || (num[0] == '-' && num[1] == '0'))
    {
        for (int index = num.size() - 1; index >= 0; --index)
        {
            if (num[index] == '-')
                return sum;
            sum += pow(8, powVal++) * std::stoi(std::string(1, num[index]));
        }
    }
    else
        return std::stoi(num);
    return sum;
}

std::vector<std::string> split(const std::string &str, const std::string &pattern)
{
    std::vector<std::string> res;
    if (str == "")
        return res;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + pattern;
    size_t pos = strs.find(pattern);
    while (pos != strs.npos)
    {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(pattern);
    }
    return res;
}

class hashMap
{
public:
    std::map<std::string, int> varMap;
    hashMap(){};
    ~hashMap(){};
    int retSymbolVal(std::string varName)
    {
        if (varMap.find(varName) == varMap.end())
            return NOT_FOUND;
        return varMap[varName];
    }

    void updateSymbolVal(std::string varName, int val)
    {
        varMap[varName] = val;
    }
};

class symbolTable
{
public:
    std::vector<hashMap> table;
    symbolTable()
    {
        table.push_back(hashMap());
    };
    ~symbolTable(){};
    int findVal(std::string varName)
    {
        int res;
        for (int index = this->table.size() - 1; index >= 0; --index)
        {
            res = table[index].retSymbolVal(varName);
            if (res != NOT_FOUND)
                return res;
        }
        return NOT_FOUND;
    }
    void pushMapLevel()
    {
        table.push_back(hashMap());
    }
    void popMapLevel()
    {
        table.pop_back();
    }
    void updateTable(std::string varName, int val)
    {
        table[table.size() - 1].updateSymbolVal(varName, val);
    }
};

calculator caler;
symbolTable symTable; //符号表

std::regex varPattern("[a-zA-Z][a-zA-Z0-9]*");
std::regex funcPattern("[a-zA-Z][a-zA-Z0-9]*\\(.*\\)");

std::vector<std::string> regex_search(std::string calStr, std::regex pattern)
{
    std::smatch matches;
    auto current = cbegin(calStr);
    auto last = cend(calStr);
    std::vector<std::string> res;
    std::ssub_match match;
    while (current != last)
    {
        if (regex_search(current, last, matches, pattern))
        {
            match = matches[0];
            current = match.second;
            res.push_back(match.str());
        }
        else
            break;
    }
    return res;
}

std::string &replace_all(std::string &str, const std::string &old_value, const std::string &new_value)
{
    while (true)
    {
        std::string::size_type pos(0);
        if ((pos = str.find(old_value)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

int calculate(std::string calStr)
{
    // auto checkArrayItem = regex_search(calStr, std::regex{"[a-zA-Z][a-zA-Z0-9]*\\[.*\\]"});
    // if (checkArrayItem.size() > 0) //TODO: 暂时这么处理，一会修改
    //     return INT32_MAX;
    if (std::regex_match(calStr, std::regex{"[ \t]*"}))
        return INT32_MAX;
    calStr = std::regex_replace(calStr, std::regex{"[ \t]*"}, "");
    //std::cout << calStr << std::endl;
    //单个数组元素或者变量直接查表返回
    if (std::regex_match(calStr, numberPattern))
        return str2int(calStr);
    if (std::regex_match(calStr, std::regex{"[+-]?[a-zA-Z][a-zA-Z0-9]*"}))
        return (calStr[0]=='-')?(-symTable.findVal(calStr)):symTable.findVal(calStr);
    if (std::regex_match(calStr, std::regex{"[+-]?[a-zA-Z][a-zA-Z0-9]*\\[[^+^-^%^*^/]*\\]"}))
    {
        std::string indexStr = cutString(calStr, '[', ']', false);
        int index = calculate(indexStr);
        if(calStr[0]=='-')
            return index == INT32_MAX ? INT32_MAX : -symTable.findVal(calStr.substr(0, calStr.size() - indexStr.size() - 2) + "[" + std::to_string(index) + ']');
        return index == INT32_MAX ? INT32_MAX : symTable.findVal(calStr.substr(0, calStr.size() - indexStr.size() - 2) + "[" + std::to_string(index) + ']');
    }
    std::vector<std::string> var;
    std::vector<int> val;
    if (std::regex_search(calStr, funcPattern))
        return INT32_MAX;
    std::string replaceName = "bbb";
    int temp;
    auto vars = regex_search(calStr, std::regex{"[a-zA-Z][a-zA-Z0-9]*\\[[^+^-^%^*^/]*\\]"});
    for (auto c : vars)
    {
        replace_all(calStr, c, replaceName);
        calStr = std::regex_replace(calStr, std::regex(c), replaceName);
        if ((temp = symTable.findVal(c)) != INT32_MAX || (temp = calculate(c)) != INT32_MAX)
        {
            var.push_back(replaceName);
            val.push_back(temp);
        }
        replaceName += "b";
    }
    vars = regex_search(calStr, varPattern);
    for (auto c : vars)
    {
        if ((temp = symTable.findVal(c)) != INT32_MAX)
        {
            var.push_back(c);
            val.push_back(temp);
        }
    }

    return caler.solution(calStr, var, val);
}

bool calBracket(std::string str)
{
    int res = 0;
    for (auto c : str)
    {
        switch (c)
        {
        case ')':
            res--;
            break;
        case '(':
            res++;
            break;
        default:
            break;
        }
    }
    return (res == 0);
}

#define SPLIT_BY_CMP_OP(t)                       \
    if ((res = split(condition, t)).size() != 0) \
    {                                            \
        res.push_back(t);                        \
        return res;                              \
    }

std::vector<std::string> getCmpOp(std::string condition)
{
    std::vector<std::string> res;
    SPLIT_BY_CMP_OP("<")
    SPLIT_BY_CMP_OP("<=")
    SPLIT_BY_CMP_OP("==")
    SPLIT_BY_CMP_OP(">=")
    SPLIT_BY_CMP_OP(">")
    return res;
}

#undef SPLIT_BY_CMP_OP

std::map<int, std::string> tokenMap = {
    {INT, "int"}, {VOID, "void"}, {IF, "if"}, {ELSE, "else"}, {WHILE, "while"}, {BREAK, "break"}, {CONTINUE, "continue"}, {RETURN, "return"}, {EQ, "=="}, {NE, "!="}, {LE, "<="}, {GE, ">="}, {AND, "&&"}, {OR, "||"}, {CONST, "const"}, {NUMBER, "number"}, {IDENT, "ident"}};

class tokenInfo
{
public:
    const int token;
    const std::string value;
    tokenInfo(int token, std::string value = "") : token(token), value(value){};
    ~tokenInfo(){};
};

class basicBlock
{
public:
    std::string contents;
    std::string ident = "";
    basicBlock() : contents(""){};
    ~basicBlock(){};
    virtual std::string getBlockType()
    {
        return "basicBlock";
    }
    virtual void getContents()
    {
        // OUTPUT_TO_FILE(this->contents);
        std::cout << this->contents;
    }
    virtual void addContents(std::string newContent)
    {
        contents += (newContent + " ");
    }
};

class varDecl : public basicBlock
{
public:
    const std::string type = "int";
    const bool isConst;
    const bool isArray;
    const std::string varName;
    const std::string dimension;
    std::string initVal;
    varDecl(std::string varName, bool isConst = false, bool isArray = false, std::string initVal = "", std::string dimension = "") : varName(varName), isConst(isConst), isArray(isArray), initVal(initVal), dimension(dimension)
    {
        ident = MAKE_IDENT(0);
        if (!isArray)
        {
            if (this->initVal == "")
                return;
            if (std::regex_match(this->initVal, numberPattern))
                symTable.updateTable(varName, str2int(this->initVal));
            else
            {
                int value = calculate(this->initVal);
                if (value != UNABLE_CAL)
                    symTable.updateTable(varName, value);
            }
        }
        //TODO 需要考虑数组初始化
    };
    ~varDecl(){};
    virtual std::string getBlockType()
    {
        return "varDecl";
    }
    void getContents()
    {
        OUTPUT_IDENT();
        if (!isArray)
        {
            std::string prefixConst = (isConst == false) ? "" : "const ";
            if (this->initVal == "")
                std::cout << this->type + " " + this->varName + ";" << std::endl;
            else
                std::cout << prefixConst + this->type + " " + this->varName + "=" + this->initVal + ";" << std::endl;
            return;
        }
        if (isConst)
            std::cout << "const " + this->type + " " + this->varName + this->dimension;
        else
            std::cout << this->type + " " + this->varName + this->dimension;
        if (this->initVal != "")
            std::cout << "=" + this->initVal + ";" << std::endl;
        else
            std::cout << ";" << std::endl;
    }
};

class manyDeclBlock : public basicBlock
{
public:
    std::vector<std::string> varNames;
    const bool isConst;
    manyDeclBlock(std::vector<std::string> varNames, bool isConst = false) : varNames(varNames), isConst(isConst) { ident = MAKE_IDENT(0); };
    ~manyDeclBlock(){};
    virtual std::string getBlockType()
    {
        return "manyDeclBlock";
    }
    void getContents()
    {
        OUTPUT_IDENT();
        std::string prefixConst = (isConst == false) ? "" : "const ";
        std::cout << prefixConst + "int" + " " << varNames[0];
        for (int index = 1; index < varNames.size(); ++index)
            std::cout << "," << varNames[index];
        std::cout << ";" << std::endl;
    }
};

class funcCallBlock : public basicBlock
{
public:
    std::string funcName;
    std::vector<std::string> params;
    funcCallBlock(std::string funcName, std::vector<std::string> params) : funcName(funcName), params(params)
    {
        ident = MAKE_IDENT(0);
        int temp;
        for (int index = 0; index < params.size(); index++)
        {
            if ((temp = calculate(params[index])) != INT32_MAX)
                this->params[index] = std::to_string(temp);
        }
    };
    ~funcCallBlock(){};
    virtual std::string getBlockType()
    {
        return "funcCallBack";
    }
    void getContents()
    {
        OUTPUT_IDENT();
        std::cout << funcName << "(" << params[0];
        for (int index = 1; index < params.size(); ++index)
            std::cout << "," << params[index];
        std::cout << ");" << std::endl;
    }
};

class funcBody : public basicBlock
{
public:
    std::vector<std::shared_ptr<basicBlock>> body;
    virtual std::string getBlockType()
    {
        return "funcBody";
    }
    void getContents()
    {
        for (auto c : body)
            c->getContents();
    }
    funcBody(){};
    ~funcBody(){};
};

class param
{
public:
    virtual std::string getParamType()
    {
        return "param";
    }
    virtual std::string codeOutPut()
    {
        return "You can't call codeOoutPut() in param class";
    };
    param(){};
    ~param(){};
};

class intParam : public param
{
public:
    const std::string paramName;
    intParam(std::string paramName) : paramName(paramName){};
    ~intParam(){};
    std::string codeOutPut()
    {
        return "int " + paramName + " ";
    }
    virtual std::string getParamType()
    {
        return "intParam";
    }
};

class intArrayParam : public param
{
public:
    const std::string paramName;
    const std::string dimension;
    intArrayParam(std::string paramName, std::string dimension) : paramName(paramName), dimension(dimension){};
    ~intArrayParam(){};
    std::string codeOutPut()
    {
        return "int " + paramName + dimension + " ";
    }
    virtual std::string getParamType()
    {
        return "intArrayParam";
    }
};

class funcDef : public basicBlock
{
public:
    const std::string returnType;
    const std::string funcName;
    std::vector<std::shared_ptr<param>> parameters;
    std::shared_ptr<funcBody> body;
    funcDef(std::string returnType, std::string funcName, std::vector<std::shared_ptr<param>> parameters, std::shared_ptr<funcBody> body) : returnType(returnType), funcName(funcName), parameters(parameters), body(body){};
    ~funcDef(){};
    virtual std::string getBlockType()
    {
        return "funcDef";
    }
    virtual void getContents()
    {
        std::cout << this->returnType + " " + this->funcName + "(";
        if (parameters.size() > 0)
        {
            std::cout << parameters[0]->codeOutPut();
            for (int index = 1; index < parameters.size(); ++index)
            {
                std::cout << ",";
                std::cout << parameters[index]->codeOutPut();
            }
        }
        std::cout << ")"
                  << "{" << std::endl;
        this->body->getContents();
        std::cout << "}" << std::endl;
    }
};

class innerBlock : public basicBlock
{
public:
    std::shared_ptr<funcBody> body;
    innerBlock(std::shared_ptr<funcBody> body) : body(body) { ident = MAKE_IDENT(1); }
    ~innerBlock(){};

    virtual std::string getBlockType()
    {
        return "innerBlock";
    }
    virtual void getContents()
    {
        OUTPUT_IDENT();
        std::cout << "{\n";
        this->body->getContents();
        OUTPUT_IDENT();
        std::cout << "}\n";
    }
};

class whileBlock : public basicBlock
{
public:
    std::shared_ptr<funcBody> body;
    std::string condition;
    int loopTimes = -1;
    bool isOpt = false;
    whileBlock(std::string condition, std::shared_ptr<funcBody> body) : body(body), condition(condition)
    {
        ident = MAKE_IDENT(1);
        this->loopTimes = handleWhileCondition(this->condition, getLoopIncrease());
        if (loopTimes < 0)
            isOpt = false;
    };
    ~whileBlock(){};
    int handleWhileCondition(std::string condition, int increse)
    {
        int leftNum, rightNum;
        std::vector<std::string> conditionCMP = getCmpOp(condition);
        if (conditionCMP.size() != 3)
        {
            this->isOpt = false;
            return -1;
        }
        if (conditionCMP[2] == "<")
        {
            rightNum = calculate(conditionCMP[1]);
            leftNum = calculate(conditionCMP[0]);
            if (rightNum > leftNum && rightNum != UNABLE_CAL && leftNum != UNABLE_CAL)
            {
                isOpt = true;
                return (rightNum - leftNum - 1) / increse;
            }
            else
                return -1;
        }
        if (conditionCMP[2] == "<=")
        {
            rightNum = calculate(conditionCMP[1]);
            leftNum = calculate(conditionCMP[0]);
            if (rightNum > leftNum && rightNum != UNABLE_CAL && leftNum != UNABLE_CAL)
            {
                isOpt = true;
                return (rightNum - leftNum) / increse;
            }
            else
                return -1;
        }
        if (conditionCMP[2] == "==")
        {
            rightNum = calculate(conditionCMP[1]);
            leftNum = calculate(conditionCMP[0]);
            if (rightNum > leftNum && rightNum != UNABLE_CAL && leftNum != UNABLE_CAL)
            {
                isOpt = true;
                return (rightNum == leftNum);
            }
            else
                return -1;
        }
        if (conditionCMP[2] == ">=")
        {
            rightNum = calculate(conditionCMP[1]);
            leftNum = calculate(conditionCMP[0]);
            if (rightNum > leftNum && rightNum != UNABLE_CAL && leftNum != UNABLE_CAL)
            {
                isOpt = true;
                return (leftNum - rightNum) / increse;
            }
            else
                return -1;
        }
        if (conditionCMP[2] == ">")
        {
            rightNum = calculate(conditionCMP[1]);
            leftNum = calculate(conditionCMP[0]);
            if (rightNum > leftNum && rightNum != UNABLE_CAL && leftNum != UNABLE_CAL)
            {
                isOpt = true;
                return (leftNum - rightNum - 1) / increse;
            }
            else
                return -1;
        }
        return -1;
    }
    int getLoopIncrease()
    {
        return 1;
    }
    virtual std::string getBlockType()
    {
        return "whileBlock";
    }
    virtual void getContents()
    {
        if (!isOpt)
        {
            OUTPUT_IDENT();
            std::cout << "while(" + this->condition + ") {" << std::endl;
            this->body->getContents();
            OUTPUT_IDENT();
            std::cout << "}" << std::endl;
        }
        else
        {
            for (int i = 0; i < this->loopTimes; ++i)
                this->body->getContents();
        }
    }
};

class ifBlock : public basicBlock
{
public:
    std::shared_ptr<funcBody> body;
    std::string condition;
    ifBlock(std::string condition, std::shared_ptr<funcBody> body) : body(body), condition(condition) { ident = MAKE_IDENT(1); };
    ~ifBlock(){};
    virtual std::string getBlockType()
    {
        return "ifBlock";
    }
    virtual void getContents()
    {
        OUTPUT_IDENT();
        std::cout << "if(" + this->condition + ") {" << std::endl;
        this->body->getContents();
        OUTPUT_IDENT();
        std::cout << "}" << std::endl;
    }
};

class binaryOP : public basicBlock
{
public:
    const std::string rightValue;
    const std::string op;
    const std::string leftValue;
    binaryOP(std::string rightValue, std::string op, std::string leftValue) : rightValue(rightValue), op(op), leftValue(leftValue)
    {
        std::regex_replace(leftValue, std::regex(" "), "");
        ident = MAKE_IDENT(0);
        if (std::regex_match(leftValue, varPattern))
        {
            if (std::regex_match(rightValue, numberPattern))
                symTable.updateTable(leftValue, str2int(rightValue));
            else
            {
                int value = calculate(rightValue);
                if (value != UNABLE_CAL)
                    symTable.updateTable(leftValue, value);
            }
        }
    };
    ~binaryOP(){};
    virtual std::string getBlockType()
    {
        return "binaryOP";
    }
    virtual void getContents()
    {
        OUTPUT_IDENT();
        std::cout << this->leftValue + " " + this->op + " " + this->rightValue;
        if (op == "=")
            std::cout << ";" << std::endl;
    }
};

class returnStmt : public basicBlock
{
public:
    const std::string retValue;
    returnStmt(std::string retValue) : retValue(retValue) { ident = MAKE_IDENT(0); };
    ~returnStmt(){};
    virtual std::string getBlockType()
    {
        return "returnStmt";
    }
    virtual void getContents()
    {
        OUTPUT_IDENT();
        std::cout << "return " + this->retValue + ";" << std::endl;
    }
};

std::string cutString(std::string cuttedStr, char start, char end, bool isConsistEnd);
std::shared_ptr<funcDef> makeFuncDefBlock();
std::shared_ptr<varDecl> makeInitVarDeclBlock();
std::shared_ptr<varDecl> makeConstDeclVarBlock();
std::shared_ptr<varDecl> makeNoInitDeclVarBlock();
std::shared_ptr<whileBlock> makeWhileBlock();
std::shared_ptr<ifBlock> makeIfBlock();
std::shared_ptr<binaryOP> makeAssignBlock();
std::shared_ptr<binaryOP> makeCompareBlock();
std::shared_ptr<varDecl> makeInitArrayBlock();
std::shared_ptr<varDecl> makeNoInitArrayBlock();
std::shared_ptr<tokenInfo> advance();
std::shared_ptr<basicBlock> reduce(std::string originalCode);
void bodyAppend(std::shared_ptr<funcBody> body);
void blockTypeChange(std::shared_ptr<basicBlock> ptr);
void makeCodeGenTable();

std::shared_ptr<tokenInfo> currentToken;
std::shared_ptr<basicBlock> getReduce;

int token;
bool isInBody = false;
std::vector<std::shared_ptr<basicBlock>> optCodegener;
std::stack<std::vector<std::shared_ptr<tokenInfo>>> reduceStack;
std::vector<std::shared_ptr<tokenInfo>> receiveToken;
std::vector<std::shared_ptr<tokenInfo>> tempReceiver;
std::string originalCode;
int tokenIndex = -1;

std::regex intArrayPattern{"int[ \t]+[a-zA-Z][a-zA-Z0-9]*[ \t]*((\\[.*\\])|(\\[\\]))+[ \t]*(\\)|,)"};
std::regex intParamPattern{"int[ \t]+[a-zA-Z][a-zA-Z0-9]*[ \t]*(\\)|,)"};
std::regex dimensionPattern{"((\\[.*\\])|(\\[\\]))+"};
std::regex identPattern{"[a-zA-Z][a-zA-Z0-9]*"};

std::string cutString(std::string cuttedStr, char start, char end, bool isConsistEnd)
{
    int indexLP = cuttedStr.find_first_of(start);
    if (indexLP == std::string::npos)
    {
        std::cout << "indexLP==std::string::npos" << std::endl;
        exit(EXIT_FAILURE);
    }
    int indexRP = cuttedStr.find_last_of(end);
    if (indexRP == std::string::npos)
    {
        std::cout << "indexRP==std::string::npos" << std::endl;
        exit(EXIT_FAILURE);
    }
    return cuttedStr.substr(indexLP + 1, indexRP - indexLP - (!isConsistEnd));
}

std::shared_ptr<funcDef> makeFuncDefBlock()
{
    deepLevel += 1;
    // for(int i = 0;i<tempReceiver.size();++i)
    //     std::cout<<tempReceiver[i]->value << std::endl;
    std::string returnType = tempReceiver[0]->value;
    std::string funcName = tempReceiver[1]->value;
    std::vector<std::shared_ptr<param>> parameters;
    std::string paramStr = cutString(originalCode, '(', ')', true);
    std::string scanedStr = "";
    std::string paramName = "";
    std::string dimension = "";
    std::smatch sm;
#define ArrayParam 1
#define IntParam 0
    auto pushParameters = [&scanedStr, &sm, &parameters, &paramName, &dimension](int type) {
        scanedStr = std::regex_replace(scanedStr, std::regex("int"), "");
        std::regex_search(scanedStr.cbegin(), scanedStr.cend(), sm, identPattern);
        paramName = sm[0].str();
        if (type == ArrayParam)
        {
            std::regex_search(scanedStr.cbegin(), scanedStr.cend(), sm, dimensionPattern);
            dimension = sm[0].str();
            parameters.push_back(std::shared_ptr<param>(new intArrayParam(paramName, dimension)));
        }
        else if (type == IntParam)
        {
            parameters.push_back(std::shared_ptr<param>(new intParam(paramName)));
        }
        scanedStr = "";
    };
    for (auto c : paramStr)
    {
        scanedStr += c;
        if (std::regex_search(scanedStr, intArrayPattern))
            pushParameters(ArrayParam);
        else if (std::regex_search(scanedStr, intParamPattern))
            pushParameters(IntParam);
    }
#undef ArrayParam
#undef IntParam
    std::shared_ptr<funcBody> body(new funcBody());
    std::shared_ptr<funcDef> result(new funcDef(returnType, funcName, parameters, body));
    if (deepLevel == 0)
        optCodegener.push_back(result);
    tempReceiver.clear();
    bodyAppend(body);
    return result;
}

#define MAKE_VAR_DECL_BLOCK(index, isConst, isArray)                \
    std::string varName = tempReceiver[index]->value;               \
    std::string initVal = cutString(originalCode, '=', ';', false); \
    tempReceiver.clear();                                           \
    return std::shared_ptr<varDecl>(new varDecl(varName, isConst, isArray, initVal));

std::shared_ptr<varDecl> makeInitVarDeclBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    MAKE_VAR_DECL_BLOCK(1, false, false);
}

std::shared_ptr<varDecl> makeConstDeclVarBlock()
{
    MAKE_VAR_DECL_BLOCK(2, true, false);
}

std::shared_ptr<varDecl> makeNoInitDeclVarBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    std::string varName = tempReceiver[1]->value;
    tempReceiver.clear();
    return std::shared_ptr<varDecl>(new varDecl(varName, false, false));
}

#undef MAKE_VAR_DECL_BLOCK

std::shared_ptr<varDecl> makeInitArrayBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    std::string varName = tempReceiver[1]->value;
    std::string dimension = tempReceiver[2]->value;
    std::string array_init = cutString(originalCode, '=', ';', false);
    // std::cout << array_init << std::endl;
    tempReceiver.clear();
    return std::shared_ptr<varDecl>(new varDecl(varName, false, true, array_init, dimension));
}

std::shared_ptr<varDecl> makeNoInitArrayBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    std::string varName = tempReceiver[1]->value;
    std::string dimension = tempReceiver[2]->value;
    tempReceiver.clear();
    return std::shared_ptr<varDecl>(new varDecl(varName, false, true, "", dimension));
}

#define INIT_DEAL   \
    deepLevel += 1; \
    std::string condition = cutString(originalCode, '(', ')', false)

std::shared_ptr<whileBlock> makeWhileBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    INIT_DEAL;
    RETURN_HAS_BODY_BLOCK(whileBlock, whileBlock(condition, body));
}

std::shared_ptr<innerBlock> makeInnerBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    deepLevel += 1;
    RETURN_HAS_BODY_BLOCK(innerBlock, innerBlock(body));
}

std::shared_ptr<ifBlock> makeIfBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    INIT_DEAL;
    RETURN_HAS_BODY_BLOCK(ifBlock, ifBlock(condition, body));
}
#undef INIT_DEAL

std::shared_ptr<returnStmt> makeReturnBlock()
{
    //从return的最后一个字母开始截取
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    return std::shared_ptr<returnStmt>(new returnStmt(cutString(originalCode, 'n', ';', false)));
}

std::shared_ptr<binaryOP> makeBinaryBlock(std::string op)
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    int indexRP = originalCode.find(op);
    if (indexRP == std::string::npos)
        exit(EXIT_FAILURE);
    std::string leftValue = originalCode.substr(0, indexRP);
    std::string rightValue = cutString(originalCode, op[op.size() - 1], ';', false);
    originalCode = "";
    tempReceiver.clear();
    return std::shared_ptr<binaryOP>(new binaryOP(rightValue, op, leftValue));
}

std::shared_ptr<binaryOP> makeAssignBlock()
{
    return makeBinaryBlock("=");
}

std::shared_ptr<binaryOP> makeCompareBlock()
{
    originalCode = std::regex_replace(originalCode, std::regex(" "), "");
    std::string tempSaver[] = {">", "<", ">=", "<="};
    int indexRP = INT32_MIN;
    std::string op;
    for (auto c : tempSaver)
    {
        indexRP = originalCode.find(c);
        if (indexRP != std::string::npos)
        {
            op = c;
            break;
        }
    }
    return makeBinaryBlock(op);
}
#undef MAKE_BINARY_OP_BLOCK

std::shared_ptr<manyDeclBlock> makeManyDeclBlock()
{
    std::string varNamesStr;
    bool isConst = (varNamesStr.find("const") != std::string::npos);
    varNamesStr = std::regex_replace(originalCode, std::regex("int"), "");
    varNamesStr = std::regex_replace(varNamesStr, std::regex(" "), "");
    varNamesStr = std::regex_replace(varNamesStr, std::regex(";"), "");
    //std::cout << "varNamesStr:" <<varNamesStr << std::endl;
    std::vector<std::string> varNames = split(varNamesStr, ",");
    tempReceiver.clear();
    return std::shared_ptr<manyDeclBlock>(new manyDeclBlock(varNames, isConst));
}

std::shared_ptr<funcCallBlock> makeFuncCallBlock()
{
    std::vector<std::string> params;
    std::string funcName = tempReceiver[0]->value;
    std::string paramStr = cutString(originalCode, '(', ')', false);
    params = split(paramStr, ",");
    return std::shared_ptr<funcCallBlock>(new funcCallBlock(funcName, params));
}

bool isNotEnd()
{
    int maxIndex = receiveToken.size() - 1;
    return (tokenIndex < maxIndex);
}

std::shared_ptr<tokenInfo> advance()
{
    return receiveToken[++tokenIndex];
}

std::shared_ptr<basicBlock> reduce(std::string originalCode)
{
#define FUNC_DECL_PATTERN 0
#define CONST_DECL_VAR_PATTERN 1
#define INIT_DECL_VAR_PATTERN 2
#define NO_INIT_DECL_VAR_PATTERN 3
#define VAR_ASSIGN_PATTERN 4
#define VAR_COMPARE_PATTERN 5
#define WHILE_PATTERN 6
#define IF_PATTERN 7
#define RETURN_PATTERN 8
#define ARRAY_INIT_DECL_PATTERN 9
#define ARRAY_NO_INIT_DECL_PATTERN 10
#define DECL_LIST_PATTERN 11
#define FUNC_CALL_PATTERN 12
#define INNER_BLOCK_PATTERN 13
    for (int index = 0; index < reducePatternArraySize; ++index)
    {
        if (std::regex_match(originalCode, reducePatternArray[index]))
        {
            switch (index)
            {
            case FUNC_DECL_PATTERN:
                return makeFuncDefBlock();
            case CONST_DECL_VAR_PATTERN:
                return makeConstDeclVarBlock();
            case INIT_DECL_VAR_PATTERN:
                return makeInitVarDeclBlock();
            case NO_INIT_DECL_VAR_PATTERN:
                return makeNoInitDeclVarBlock();
            case VAR_ASSIGN_PATTERN:
                return makeAssignBlock();
            case VAR_COMPARE_PATTERN:
                return makeCompareBlock();
            case WHILE_PATTERN:
                return makeWhileBlock();
            case IF_PATTERN:
                return makeIfBlock();
            case RETURN_PATTERN:
                return makeReturnBlock();
            case ARRAY_INIT_DECL_PATTERN:
                return makeInitArrayBlock();
            case ARRAY_NO_INIT_DECL_PATTERN:
                return makeNoInitArrayBlock();
            case DECL_LIST_PATTERN:
                return makeManyDeclBlock();
            case FUNC_CALL_PATTERN:
                return makeFuncCallBlock();
            case INNER_BLOCK_PATTERN:
                return makeInnerBlock();
            default:
                return NULL;
            }
        }
    }
    return NULL;
}

#define VAR_DECL 0
#define FUNC_BODY 1
#define FUNC_DEF 2
#define WHILE_BLOCK 3
#define IF_BLOCK 4
#define BINARY_OP 5
#define RETURN_STMT 6

std::map<std::string, int> hashClassNameMap = {
    {"varDecl", VAR_DECL}, {"funcBody", FUNC_BODY}, {"funcDef", FUNC_DEF}, {"whileBlock", WHILE_BLOCK}, {"ifBlock", IF_BLOCK}, {"assign", BINARY_OP}, {"returnStmt", RETURN_STMT}};

struct changerResultSet
{
    std::shared_ptr<varDecl> varDeclPtr;
    std::shared_ptr<funcBody> funcBodyPtr;
    std::shared_ptr<whileBlock> whileBlockPtr;
    std::shared_ptr<ifBlock> ifBlockPtr;
    std::shared_ptr<binaryOP> binaryOpPtr;
    std::shared_ptr<returnStmt> returnStmtPtr;
    std::shared_ptr<funcDef> funcDefPtr;
    int ptrType;
} pointerSet;

#define TYPE_CHANGE_STMT(PtrName, PtrType, TypeValue)             \
    pointerSet.PtrName = std::dynamic_pointer_cast<PtrType>(ptr); \
    pointerSet.ptrType = TypeValue;                               \
    break
void blockTypeChange(std::shared_ptr<basicBlock> ptr)
{
    switch (hashClassNameMap[ptr->getBlockType()])
    {
    case VAR_DECL:
        TYPE_CHANGE_STMT(varDeclPtr, varDecl, VAR_DECL);
    case FUNC_BODY:
        TYPE_CHANGE_STMT(funcBodyPtr, funcBody, FUNC_BODY);
    case WHILE_BLOCK:
        TYPE_CHANGE_STMT(whileBlockPtr, whileBlock, WHILE_BLOCK);
    case IF_BLOCK:
        TYPE_CHANGE_STMT(ifBlockPtr, ifBlock, IF_BLOCK);
    case BINARY_OP:
        TYPE_CHANGE_STMT(binaryOpPtr, binaryOP, BINARY_OP);
    case RETURN_STMT:
        TYPE_CHANGE_STMT(returnStmtPtr, returnStmt, RETURN_STMT);
    }
}
#undef TYPE_CHANGE_STMT

#define PUSH_BODY_TO_FUNC_BODY(type) \
    pointerSet.type->body = body;    \
    break
void bodyAppend(std::shared_ptr<funcBody> body)
{
    bool isOneLevelBody = true;
    symTable.pushMapLevel();
    if (receiveToken[tokenIndex + 1]->value == "{" && receiveToken[tokenIndex]->value != "{")
    {
        ++tokenIndex;
        originalCode += "{";
    }
    if (originalCode.find('{') != std::string::npos)
    {
        isOneLevelBody = false;
    }
    originalCode = "";
    while (isNotEnd())
    {
        currentToken = advance();
        tempReceiver.push_back(currentToken);
        if (currentToken->value == "}" && !isOneLevelBody)
        {
            symTable.popMapLevel();
            if (deepLevel != 0)
                --deepLevel;
            tempReceiver.clear();
            originalCode = "";
            return;
        }
        originalCode += (currentToken->value + " ");
        if (!calBracket(originalCode))
            continue;
        if ((getReduce = reduce(originalCode)) != NULL)
        {
            tempReceiver.clear();
            body->body.push_back(getReduce);
            originalCode = "";
            if (isOneLevelBody)
            {
                symTable.popMapLevel();
                if (deepLevel != 0)
                    --deepLevel;
                return;
            }
        }
    }
}
#undef PUSH_BODY_TO_FUNC_BODY

#undef VAR_DECL
#undef FUNC_BODY
#undef FUNC_DEF
#undef WHILE_BLOCK
#undef IF_BLOCK
#undef BINARY_OP
#undef RETURN_STMT

void makeCodeGenTable()
{
    deepLevel = 0;
    while (isNotEnd())
    {
        currentToken = advance();
        tempReceiver.push_back(currentToken);
        originalCode += (currentToken->value + " ");
        if (calBracket(originalCode) && (getReduce = reduce(originalCode)) != NULL && deepLevel == 0)
        {
            optCodegener.push_back(getReduce);
            originalCode = "";
        }
    }
}

//#define main outputToFile
// int main(int argc, const char **argv)
// {
//     // freopen("des.sy", "w+", stdout);
//     in.open(argv[1]);
//     lexer = new yyFlexLexer(&in);
//     while ((token = lexer->yylex()) != EOF)
//         receiveToken.push_back(std::shared_ptr<tokenInfo>(new tokenInfo(token, tokenValue)));
//     //SHOW_RECEIVE_TOKEN();
//     makeCodeGenTable();
//     for (auto c : optCodegener)
//         c->getContents();
//     return 0;
// }
//#undef main outputToFile

extern std::string tokenValue;
extern FlexLexer* lexer;
extern std::ifstream in;

int preOpt(std::string file_name)
{
    in.open(file_name.c_str());
    lexer = new yyFlexLexer(&in);
    freopen("tmp.sy", "w", stdout);
    while ((token = lexer->yylex()) != EOF)
        receiveToken.push_back(std::shared_ptr<tokenInfo>(new tokenInfo(token, tokenValue)));
    makeCodeGenTable();
    for (auto c : optCodegener)
        c->getContents();
    freopen("/dev/tty", "w", stdout);
    return 0;
}
