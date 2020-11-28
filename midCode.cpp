#include <iostream>
#include <cstring>
#include "midCode.hpp"

// 中间代码优化 类
class midCodeOpt
{
public:
    midCodeList* midCode;
    flowGraph fGraph;
    changeInfoList changeInfo;
    // 存储公共子表达式结构，公共子表达式拥有相同的int值，无公共子表达式的为0
    vector<int> commonExpr;
    // 公共子表达式替换信息，string为替换的标识符，为0代表不可替换
    vector<string> replaceInfo;

    midCodeOpt(midCodeList* _midCode);
    ~midCodeOpt() {};
    void printMidCode();
    void initFlowGraph();
    void initChangeInfo();
    void initChangeInfo_dfs(NBasicBlock* block, map<string, int> global_change_info, set<int> has_jmp);
    void commonExprSearch();
    void commonExprReplace();
    void commonExprReplace_dfs(NBasicBlock* block, vector<commonExprInfo> commonExprVar, set<int> has_jmp);
};

midCodeOpt::midCodeOpt(midCodeList* _midCode)
{
    midCode = _midCode;
    //initFlowGraph();
}

// 打印中间代码
void midCodeOpt::printMidCode()
{
    if(midCode->empty())
        return;
    int size = midCode->size();
    for(int i = 0; i < size; i++)
    {
        NmidCode *temp = &midCode->at(i);
        printf("%d %s %s %s %s %d %d %s %d\n",temp->op, temp->tag.c_str(), temp->lVal.c_str(),
                temp->rVal1.ident.c_str(), temp->rVal2.ident.c_str(), temp->rVal1.val, temp->rVal2.val, 
                temp->to_tag.c_str(), temp->cmp);
    }
}

// 通过中间代码 midCode 初始化流图 fGraph
void midCodeOpt::initFlowGraph()
{
    if(midCode->empty())
        return;
    
    NBasicBlock temp;
    int start = 0;
    int midCodeSize = midCode->size();

    // 划分基本块
    for(int i = 0; i < midCodeSize; i++)
    {
        // 如有标号
        if(!midCode->at(i).tag.empty())
        {
            // 当 该句的前一句为跳转语句 或 该句为第0句 时，特判
            if(start == i)
                continue;
            temp.start = &(midCode->at(start));
            temp.end = &(midCode->at(i-1));
            fGraph.push_back(temp);
            start = i;
        }
        // 如为跳转语句
        if(midCode->at(i).op >= cal_op::GOTO_C && midCode->at(i).op <= cal_op::FUNC_CALL)
        {
            temp.start = &(midCode->at(start));
            temp.end = &(midCode->at(i));
            fGraph.push_back(temp);
            start = i+1;
        }
    }
    // 最后一块特判
    if(start <= midCode->size())
    {
        temp.start = &(midCode->at(start));
        temp.end = &(midCode->back());
        fGraph.push_back(temp);
    }

    int fGraphSize = fGraph.size();

    // 存放每个tag的位置的map
    map<string, NBasicBlock*> tag_map;

    // 遍历基本块填入 tag_map
    for(int i = 0; i < fGraphSize; i++)
    {
        if(!fGraph[i].start->tag.empty())
            tag_map.insert(pair<string, NBasicBlock*>(fGraph[i].start->tag, &fGraph[i]));
    }

    // 填入NBasicBlock的goto_order项
    for(int i = 0; i < fGraphSize; i++)
    {
        // 无条件跳转
        if(fGraph[i].end->op == cal_op::GOTO_U || fGraph[i].end->op == cal_op::FUNC_CALL)
        {
            fGraph[i].next = tag_map.find(fGraph[i].end->to_tag)->second;
            fGraph[i].jmp = NULL;
        }
        // 有条件跳转，有两个后继基本块
        else if(fGraph[i].end->op == cal_op::GOTO_C)
        {
            fGraph[i].jmp = tag_map.find(fGraph[i].end->to_tag)->second;
            if(i < fGraphSize - 1)
                fGraph[i].next = &fGraph[i+1];
        }
        // 无跳转语句的基本块
        else if(i < fGraphSize - 1)
        {
            fGraph[i].next = &fGraph[i+1];
            fGraph[i].jmp = NULL;
        }
    }
}

// 初始化各语句中变量的最近修改信息
void midCodeOpt::initChangeInfo()
{
    if(midCode->empty())
        return;
    // 设置最近修改信息vector的大小
    changeInfo.resize(midCode->size());
    // 记录当前所有变量的最后修改语句
    map<string, int> global_change_info;
    // 记录已经跳转的跳转语句，防止dfs死循环
    set<int> has_jmp;

    initChangeInfo_dfs(&fGraph[0], global_change_info, has_jmp);

}

// dfs的方式完成最近修改信息的初始化
void midCodeOpt::initChangeInfo_dfs(NBasicBlock* block, map<string, int> global_change_info, set<int> has_jmp)
{
    // 确定该基本块的首位语句在midCode中的下标
    int start = block->start - &(midCode->front());
    int end = block->end - &(midCode->front());

    for(int i = start; i <= end; i++)
    {
        // 对变量赋值的语句，对数组赋值不算在里面
        if((midCode->at(i).op >= cal_op::ADD && midCode->at(i).op <= cal_op::COPY_AtoI) || midCode->at(i).op == cal_op::FUNC_CALL)
        {
            // 首先针对左值修改global_change_info的内容
            // 定义迭代器，搜索该表达式的左值
            map<string, int>::iterator it = global_change_info.find(midCode->at(i).lVal);
            // 左值的变量未被找到，则插入
            if(it == global_change_info.end())
                global_change_info.insert( map<string, int>::value_type( midCode->at(i).lVal, i) );
            // 左值的变量被找到，则修改
            else
                it->second = i;

            // 然后写入changeInfo
            if(midCode->at(i).op >= cal_op::ADD && midCode->at(i).op <= cal_op::MOD)
            {
                // rVal1
                if(midCode->at(i).rVal1.type == VIDENT)
                {
                    if(changeInfo[i].pos_r1 == 0)
                    {
                        changeInfo[i].rVal1_lc = global_change_info.find(midCode->at(i).rVal1.ident)->second;
                        changeInfo[i].pos_r1 = 1;
                    }
                    else if(changeInfo[i].pos_r1 == 1 && changeInfo[i].rVal1_lc == global_change_info.find(midCode->at(i).rVal1.ident)->second)
                    {}
                    else
                        ++changeInfo[i].pos_r1;
                }
                // rVal2
                if(midCode->at(i).rVal2.type == VIDENT)
                {
                    if(changeInfo[i].pos_r2 == 0)
                    {
                        changeInfo[i].rVal2_lc = global_change_info.find(midCode->at(i).rVal2.ident)->second;
                        changeInfo[i].pos_r2 = 1;
                    }
                    else if(changeInfo[i].pos_r2 == 1 && changeInfo[i].rVal2_lc == global_change_info.find(midCode->at(i).rVal2.ident)->second)
                    {}
                    else
                        ++changeInfo[i].pos_r2;
            }
            }
        }
    }
    // 进行dfs
    if(block->next != NULL)
        initChangeInfo_dfs(block->next, global_change_info, has_jmp);
    if(block->jmp != NULL)
    {
        // 如果 has_jmp 里没有该跳转语句，则dfs下去，并将其插入has_jmp
        int index = block - &(fGraph.front());
        if(has_jmp.find(index) != has_jmp.end())
        {
            initChangeInfo_dfs(block->jmp, global_change_info, has_jmp);
            has_jmp.insert(index);
        }
    }
}

// 遍历 midCode 查找公共子表达式
void midCodeOpt::commonExprSearch()
{
    int common_expr_num = 0;
    commonExpr.clear();
    int midCodeSize = midCode->size();
    commonExpr.resize(midCodeSize, 0);
    map<string, int> expr;
    // 遍历 midCode
    for(int i = 0; i < midCodeSize; i++)
    {
        if(midCode->at(i).op >= cal_op::ADD && midCode->at(i).op <= cal_op::MOD)
        {
            if(changeInfo[i].pos_r1 > 1 || changeInfo[i].pos_r2 > 1)
                continue;
            // 把表达式变成字符串方便查询
            // r1, r2 为两操作数，lc(last change) 指最后修改的语句的下标
            string temp_exp, op, r1, r2, lc1, lc2;
            op = to_string((int)midCode->at(i).op);
            if(midCode->at(i).rVal1.type == VIDENT)
                r1 = midCode->at(i).rVal1.ident;
            else
                r1 = to_string(midCode->at(i).rVal1.val);
            if(midCode->at(i).rVal2.type == VIDENT)
                r2 = midCode->at(i).rVal2.ident;
            else
                r2 = to_string(midCode->at(i).rVal2.val);
            lc1 = to_string(changeInfo[i].rVal1_lc);
            lc2 = to_string(changeInfo[i].rVal2_lc);

            temp_exp.clear();
            temp_exp = r1 +" "+ op +" "+ r2 +" "+ lc1 +" "+ lc2;
            map<string, int>::iterator it = expr.find(temp_exp);
            // 找到就记录，没找到就添加
            if(it != expr.end())
            {
                if(commonExpr[it->second] == 0)
                {
                    commonExpr[it->second] = common_expr_num;
                    commonExpr[i] = common_expr_num;
                    ++common_expr_num;
                }
                else
                    commonExpr[i] = commonExpr[it->second];
            }
            else if(midCode->at(i).op == cal_op::ADD || midCode->at(i).op == cal_op::MUL)   // ADD 和 MUL 操作数可反向
            {
                temp_exp.clear();
                temp_exp = r2 +" "+ op +" "+ r1 +" "+ lc2 +" "+ lc1;
                it = expr.find(temp_exp);
                if(it != expr.end())
                {
                    if(commonExpr[it->second] == 0)
                    {
                        commonExpr[it->second] = common_expr_num;
                        commonExpr[i] = common_expr_num;
                        ++common_expr_num;
                    }
                    else
                        commonExpr[i] = commonExpr[it->second];
                }
                else
                    expr.insert(map<string, int>::value_type(temp_exp, i));
            }
            else
                expr.insert(map<string, int>::value_type(temp_exp, i));
        }
    }
}

// 替换公共子表达式
void midCodeOpt::commonExprReplace()
{
    // 存储公共子表达式的信息
    vector<commonExprInfo> commonExprVar;
    // // 记录已经跳转的跳转语句，防止dfs死循环
    set<int> has_jmp;

    replaceInfo.clear();
    replaceInfo.resize(midCode->size());

    commonExprReplace_dfs(&fGraph[0], commonExprVar, has_jmp);

    // 根据 replaceInfo 的信息进行公共子表达式替换
    int midCode_size = midCode->size();
    for(int i = 0; i < midCode_size; i++)
    {
        if(!replaceInfo[i].empty() && replaceInfo[i] != "0")
        {
            midCode->at(i).op = cal_op::COPY;
            midCode->at(i).rVal1.type = VIDENT;
            midCode->at(i).rVal1.ident = replaceInfo[i];
        }
    }
}

// dfs的方式替换公共子表达式
void midCodeOpt::commonExprReplace_dfs(NBasicBlock* block, vector<commonExprInfo> commonExprVar, set<int> has_jmp)
{
    // 确定该基本块的首位语句在midCode中的下标
    int start = block->start - &(midCode->front());
    int end = block->end - &(midCode->front());
    bool is_insert = false;     // 是否向 commonExprVar 中插入条目

    for(int i = start; i <= end; i++)
    {
        // 对于公共子表达式
        if(commonExpr[i] != 0)
        {
            // 该句是否可以替换
            if(replaceInfo[i] != "0")
            {
                // 查询是否在 commonExprVar 中
                int commonExprVar_size = commonExprVar.size();
                bool is_in = false;
                int commonExprVar_index;    // 在 commonExprVar 中的下标
                for(commonExprVar_index = 0; commonExprVar_index < commonExprVar_size; commonExprVar_index++)
                    if(commonExprVar[commonExprVar_index].commonExprID == commonExpr[i])
                    {
                        is_in = true;
                        break;
                    }
                // 如果查询到
                if(is_in)
                {
                    // 如果已经被标记替换
                    if(!replaceInfo[i].empty())
                    {
                        // 不是当前存在 commonExprVar 中的标识符
                        if(replaceInfo[i] != commonExprVar[commonExprVar_index].ident)
                            replaceInfo[i] = "0";
                    }
                    // 没被标记替换，则添加替换信息
                    else
                        replaceInfo[i] = commonExprVar[commonExprVar_index].ident;
                }
                // 没查询到就插入，并将该句设为不可替换
                else
                {
                    commonExprVar.push_back( commonExprInfo{commonExpr[i], midCode->at(i).lVal} );
                    replaceInfo[i] = "0";
                    is_insert = true;
                }
            }
        }

        // 对于所有语句，查询该句的左值是否是 commonExprVar 中存在的
        // 如果是则需要在 commonExprVar 中删除该条，该次插入的不算
        int commonExprVar_size = commonExprVar.size();
        for(int j = 0; j < commonExprVar_size; j++)
        {
            if(commonExprVar[j].ident == midCode->at(i).lVal)
            {
                // 该条是这次插入的，忽略
                if(is_insert && j == commonExprVar_size - 1)
                    ;
                // 不是这次插入的，删除
                else
                    commonExprVar.erase(commonExprVar.begin() + j);
                break;
            }
        }
    }

    // 进行dfs
    if(block->next != NULL)
        commonExprReplace_dfs(block->next, commonExprVar, has_jmp);
    if(block->jmp != NULL)
    {
        // 如果 has_jmp 里没有该跳转语句，则dfs下去，并将其插入has_jmp
        int index = block - &(fGraph.front());
        if(has_jmp.find(index) != has_jmp.end())
        {
            commonExprReplace_dfs(block->jmp, commonExprVar, has_jmp);
            has_jmp.insert(index);
        }
    }

    /* 
        思路：
        遇到每一句公共子表达式，先搜索是否存在，
        存在就看它是否被标记已更改
            是则
                不是当前这个，把它的更改信息改成0
                是当前这个 就忽略
            不是就更改
        不存在就插入，插入时将该条语句设置为不可更改
    
        对于每条语句（包括公共子表达式语句），如左值为公共子表达式的标识符，删除该条
    */
}

// midCodeList genMidCode()
// {
//     midCodeList midCode;
//     int num;
//     freopen ("test.mid","r",stdin); 
//     // cin>>num;
//     for(int i = 0; i < num; i++)
//     {
//         NmidCode temp;
//         cin >> temp.op;
//         if(temp.op >= cal_op::ADD && temp.op <= cal_op::MOD)
//         {
//             cin>>temp.lVal;

//             int is_ident;
//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal1.type = VIDENT;
//                 cin>>temp.rVal1.ident;
//             }
//             else
//             {
//                 temp.rVal1.type = VINT;
//                 cin>>temp.rVal1.val;
//             }

//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal2.type = VIDENT;
//                 cin>>temp.rVal2.ident;
//             }
//             else
//             {
//                 temp.rVal2.type = VINT;
//                 cin>>temp.rVal2.val;
//             }

//             int have_tag;
//             cin>>have_tag;
//             if(have_tag == 1)
//                 cin>>temp.tag;
//         }

//         if(temp.op == NEG)
//         {
//             cin>>temp.lVal;

//             int is_ident;
//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal1.type = VIDENT;
//                 cin>>temp.rVal1.ident;
//             }
//             else
//             {
//                 temp.rVal1.type = VINT;
//                 cin>>temp.rVal1.val;
//             }

//             int have_tag;
//             cin>>have_tag;
//             if(have_tag == 1)
//                 cin>>temp.tag;
//         }

//         if(temp.op == cal_op::COPY)
//         {
//             cin>>temp.lVal;

//             int is_ident;
//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal1.type = VIDENT;
//                 cin>>temp.rVal1.ident;
//             }
//             else
//             {
//                 temp.rVal1.type = VINT;
//                 cin>>temp.rVal1.val;
//             }

//             int have_tag;
//             cin>>have_tag;
//             if(have_tag == 1)
//                 cin>>temp.tag;
//         }

//         if(temp.op == cal_op::COPY_AtoI || temp.op == cal_op::COPY_ItoA)
//         {
//             cin>>temp.lVal;

//             int is_ident;

//             temp.rVal1.type = VIDENT;
//             cin>>temp.rVal1.ident;

//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal1.type = VIDENT;
//                 cin>>temp.rVal1.ident;
//             }
//             else
//             {
//                 temp.rVal1.type = VINT;
//                 cin>>temp.rVal1.val;
//             }

//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal2.type = VIDENT;
//                 cin>>temp.rVal2.ident;
//             }
//             else
//             {
//                 temp.rVal2.type = VINT;
//                 cin>>temp.rVal2.val;
//             }

//             int have_tag;
//             cin>>have_tag;
//             if(have_tag == 1)
//                 cin>>temp.tag;
//         }

//         if(temp.op == cal_op::GOTO_U)
//         {
//             cin>>temp.to_tag;

//             int have_tag;
//             cin>>have_tag;
//             if(have_tag == 1)
//                 cin>>temp.tag;
//         }

//         if(temp.op == cal_op::GOTO_C)
//         {
//             cin>>temp.to_tag;

//             int is_ident;
//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal1.type = VIDENT;
//                 cin>>temp.rVal1.ident;
//             }
//             else
//             {
//                 temp.rVal1.type = VINT;
//                 cin>>temp.rVal1.val;
//             }

//             cin>>is_ident;
//             if(is_ident == 1)
//             {
//                 temp.rVal1.type = VIDENT;
//                 cin>>temp.rVal1.ident;
//             }
//             else
//             {
//                 temp.rVal1.type = VINT;
//                 cin>>temp.rVal1.val;
//             }
            
//             cin>>temp.cmp;

//             int have_tag;
//             cin>>have_tag;
//             if(have_tag == 1)
//                 cin>>temp.tag;
//         }

//         midCode.push_back(temp);
//     }
//     fclose(stdin);
//     return midCode;
// }

// int main()
// {
//     midCodeList midCode;
//     midCode = genMidCode();
//     midCodeOpt opt(&midCode);
//     // TODO 进行优化
//     opt.printMidCode();
//     return 0;
// }

void midCodeOutput(midCodeList *midCode)
{
    if (midCode->empty())
        return;
    int size = midCode->size();
    for (int i = 0; i < size; i++)
    {
        NmidCode *stmt = &midCode->at(i);
        if (!stmt->tag.empty())
        {
            printf("%s:\n", stmt->tag.c_str());
        }
        if (stmt->op >= cal_op::NOP)
            continue;

        printf("\t");

        if (stmt->op >= cal_op::ADD && stmt->op <= cal_op::MOD)
        {
            printf("%s = ", stmt->lVal.c_str());

            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());

            switch (stmt->op)
            {
            case cal_op::ADD:
                printf(" + ");
                break;
            case cal_op::SUB:
                printf(" - ");
                break;
            case cal_op::MUL:
                printf(" * ");
                break;
            case cal_op::DIV:
                printf(" / ");
                break;
            case cal_op::MOD:
                printf(" %% ");
                break;
            default:
                break;
            }

            if (stmt->rVal2.type == VINT)
                printf("%d", stmt->rVal2.val);
            else
                printf("%s", stmt->rVal2.ident.c_str());
        }
        else if (stmt->op == cal_op::NEG || stmt->op == cal_op::NOT)
        {
            printf("%s = ", stmt->lVal.c_str());

            if (stmt->op == cal_op::NEG)
                printf("-");
            else
                printf("!");

            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());
        }
        else if (stmt->op == cal_op::COPY)
        {
            printf("%s = ", stmt->lVal.c_str());

            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());
        }
        else if (stmt->op == cal_op::COPY_AtoI)
        {
            printf("%s = ", stmt->rVal2.ident.c_str());

            printf("%s", stmt->lVal.c_str());

            printf("[");
            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());
            printf("]");
        }
        else if (stmt->op == cal_op::COPY_ItoA)
        {
            printf("%s", stmt->lVal.c_str());

            printf("[");
            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());
            printf("]");

            printf(" = %s", stmt->rVal2.ident.c_str());
        }
        else if (stmt->op == cal_op::GOTO_U)
        {
            printf("goto %s", stmt->to_tag.c_str());
        }
        else if (stmt->op == cal_op::GOTO_C)
        {
            printf("if ");

            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());

            switch (stmt->cmp)
            {
            case cmp_op::EQ:
                printf(" == ");
                break;
            case cmp_op::NE:
                printf(" != ");
                break;
            case cmp_op::G:
                printf(" > ");
                break;
            case cmp_op::L:
                printf(" < ");
                break;
            case cmp_op::GE:
                printf(" >= ");
                break;
            case cmp_op::LE:
                printf(" <= ");
                break;
            default:
                break;
            }

            if (stmt->rVal2.type == VINT)
                printf("%d", stmt->rVal2.val);
            else
                printf("%s", stmt->rVal2.ident.c_str());

            printf("then goto %s", stmt->to_tag.c_str());
        }
        else if (stmt->op == cal_op::FUNC_CALL)
        {
            printf("%s = ", stmt->lVal.c_str());

            printf("%s", stmt->to_tag.c_str());

            printf("(");

            int paraSize = stmt->paraList.size();
            if (paraSize == 1)
            {
                if (stmt->paraList[0].type == VINT)
                    printf("%d", stmt->paraList[0].val);
                else
                    printf("%s", stmt->paraList[0].ident.c_str());
            }
            else if (paraSize > 1)
            {
                if (stmt->paraList[0].type == VINT)
                    printf("%d", stmt->paraList[0].val);
                else
                    printf("%s", stmt->paraList[0].ident.c_str());
                for (int j = 1; j < paraSize; j++)
                {
                    if (stmt->paraList[j].type == VINT)
                        printf(", %d", stmt->paraList[j].val);
                    else
                        printf(", %s", stmt->paraList[j].ident.c_str());
                }
            }

            printf(")");
        }
        else if (stmt->op == cal_op::FUNC_DEF)
        {
            printf("FUNC_DEF: ");

            printf("%s", stmt->to_tag.c_str());

            printf("(");

            int paraSize = stmt->paraList.size();
            if (paraSize == 1)
            {
                if (stmt->paraList[0].type == VINT)
                    printf("%d", stmt->paraList[0].val);
                else
                    printf("%s", stmt->paraList[0].ident.c_str());
            }
            else if (paraSize > 1)
            {
                if (stmt->paraList[0].type == VINT)
                    printf("%d", stmt->paraList[0].val);
                else
                    printf("%s", stmt->paraList[0].ident.c_str());
                for (int j = 1; j < paraSize; j++)
                {
                    if (stmt->paraList[j].type == VINT)
                        printf(", %d", stmt->paraList[j].val);
                    else
                        printf(", %s", stmt->paraList[j].ident.c_str());
                }
            }

            printf(")");
        }
        else if (stmt->op == cal_op::RETURN)
        {
            printf("return ");

            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());
        }
        else if (stmt->op == cal_op::INT_DECL)
        {
            printf("int ");
            printf("%s", stmt->lVal.c_str());

            if (stmt->is_init)
            {
                printf(" = ");
                if (stmt->rVal1.type == VINT)
                    printf("%d", stmt->rVal1.val);
                else
                    printf("%s", stmt->rVal1.ident.c_str());
            }
        }
        else if (stmt->op == cal_op::ARR_DECL)
        {
            printf("int ");
            printf("%s", stmt->lVal.c_str());

            printf("[");
            if (stmt->rVal1.type == VINT)
                printf("%d", stmt->rVal1.val);
            else
                printf("%s", stmt->rVal1.ident.c_str());
            printf("]");

            if (stmt->is_init)
            {
                printf(" = ");

                printf("{");

                int paraSize = stmt->paraList.size();
                if (paraSize == 1)
                {
                    if (stmt->paraList[0].type == VINT)
                        printf("%d", stmt->paraList[0].val);
                    else
                        printf("%s", stmt->paraList[0].ident.c_str());
                }
                else if (paraSize > 1)
                {
                    if (stmt->paraList[0].type == VINT)
                        printf("%d", stmt->paraList[0].val);
                    else
                        printf("%s", stmt->paraList[0].ident.c_str());
                    for (int j = 1; j < paraSize; j++)
                    {
                        if (stmt->paraList[j].type == VINT)
                            printf(", %d", stmt->paraList[j].val);
                        else
                            printf(", %s", stmt->paraList[j].ident.c_str());
                    }
                }

                printf("}");
            }
        }

        printf("\n");
    }
}