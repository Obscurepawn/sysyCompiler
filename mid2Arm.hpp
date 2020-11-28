#ifndef MID2ARM_HPP
#define MID2ARM_HPP

#include "midCode.hpp"
void codeGen(NmidCode mid);
void mid2ArmCode(midCodeList mid_code);
void genBinary(NmidCode mid);
void genUnary(NmidCode mid);
void genAssign(NmidCode mid);
void genArrayToInt(NmidCode mid);
void genIntToArray(NmidCode mid);
void genJmp(NmidCode mid);
void genFuncCall(NmidCode func_call);
void genRet(NmidCode ret);
void genIntDecl(NmidCode int_decl);
void genArrayDecl(NmidCode arr_decl);
void genFuncDef(NmidCode func_def);
#endif