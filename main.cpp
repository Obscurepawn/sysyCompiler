#include "genArm.hpp"
#include "ast.hpp"
#include "hashMap.hpp"
#include "parser.hpp"
#include "mid2Arm.hpp"
#include "genMid.hpp"
extern int preOpt(string file_name);
// #define DEBUG
extern FILE* yyin;
extern midCodeList midCode;
FILE* as_file;
NBlock *prog_block = nullptr;
#if defined(DEBUG)
bool to_debug = false;
int main(int argc, char *argv[]) {
    try {
        yyparse();
        // Code gen
        Generator g("asm.s");
        g.genArmCode(prog_block);
    } catch(std::out_of_range e) {
        printf("Error: %s\n", e.what());
        exit(EXIT_FAILURE);
    }
    return 0;
}
#else
bool to_debug = false;
bool opt = false;
int main(int argc, char *argv[])
{
    string in_file_name = "";
    string out_file_name = "";
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // 进行代码优化
            if(argv[i][1] == 'O') {
                opt = true;
            }
            continue;
        }
        string file_name = string(argv[i]);
        int pos = file_name.find_last_of('.');
        // 寻找.sy 源文件
        if(file_name.substr(pos) == ".sy") {
            in_file_name = argv[i];
        }
        // 寻找输出文件
        if(file_name.substr(pos) == ".s") {
            out_file_name = argv[i];
        }
    }
    // 找不到源文件，就退出
    if(in_file_name.length() == 0) {
        fprintf(stderr,"Error: sy file needed!\n");
        exit(EXIT_FAILURE);
    }
    // 找不到输出文件，就替换.sy为.s
    if(out_file_name.length() == 0) {
        int pos = in_file_name.find_last_of('.');
        out_file_name = in_file_name.substr(0, pos) + ".s";
    }
    yyin = fopen(in_file_name.c_str(), "r");
    if (!yyin) {
        fprintf(stderr, "Error: file %s not exist!\n", in_file_name.c_str());
        exit(EXIT_FAILURE);
    }
    int pos = in_file_name.find_last_of('_') + 1;
    int len = in_file_name.find_last_of('.') - pos;
    string name = in_file_name.substr(pos, len);
    // 代码生成
    if(!opt) {
        // 语法分析，得到 AST
        yyparse();
        // 如果不进行优化则直接从AST翻译得到ARM汇编
        Generator g(out_file_name);
        g.genArmCode(prog_block);
        return 0;
    }
    // 预优化
    // int res = preOpt(in_file_name);
    // yyin = fopen("tmp.sy", "r");
    yyparse();
    Generator g(out_file_name);
    g.genArmCode(prog_block);
    return 0;
    // 代码优化
    // as_file = fopen(out_file_name.c_str(), "w");
    // MidGenerator mid_generator;
    // // 生成中间代码
    // printf("gen mid\n");
    // mid_generator.genMidCode(prog_block);
    // // 优化
    // // 生成arm
    // printf("gen arm\n");
    // mid2ArmCode(midCode);
    // printf("end\n");
    return 0;
}
#endif