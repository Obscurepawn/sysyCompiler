# sysyCompiler

2020 毕昇杯参赛编译器项目，将 SysY 源文件解析为 AST，再生成 ARMv7 汇编。仓库包含 Flex/Bison 语法定义、生成的解析器，以及中间代码和优化相关实验代码。

## 构建与运行

在仓库根目录执行以下命令，适用于 Linux/WSL，使用支持 C++17 的 Clang。它们直接编译仓库内已有的解析器，覆盖当前 `main.cpp` 使用的 AST → ARM 路径：

```sh
mkdir -p build
clang++ -std=c++17 -O2 \
  ast.cpp lexer.cpp parser.cpp main.cpp genArm.cpp type.cpp \
  condition.cpp midCode.cpp genMid.cpp Mid2Arm.cpp \
  -o build/compiler

printf 'int main() { return 0; }\n' > build/hello.sy
./build/compiler build/hello.sy build/hello.s
```

输出文件 `build/hello.s` 包含 `.arch armv7-a` 和 `main` 的汇编代码。省略输出文件名时，编译器会将输入的 `.sy` 后缀替换为 `.s`。这里完成的是汇编生成；执行生成的程序还需要 ARM 工具链及对应运行环境。

## 代码入口

| 文件 | 用途 |
| --- | --- |
| [main.cpp](main.cpp) | 输入/输出文件处理和编译入口 |
| [lexer.l](lexer.l)、[parser.y](parser.y) | 词法与语法定义 |
| [lexer.cpp](lexer.cpp)、[parser.cpp](parser.cpp)、[parser.hpp](parser.hpp) | 已生成的解析器文件 |
| [ast.hpp](ast.hpp)、[ast.cpp](ast.cpp) | 抽象语法树 |
| [genArm.cpp](genArm.cpp) | ARM 汇编生成 |
| [genMid.cpp](genMid.cpp)、[midCode.cpp](midCode.cpp)、[Mid2Arm.cpp](Mid2Arm.cpp) | 中间代码相关实现 |
| [opt](opt) | 优化相关实验代码 |
| [中间代码变量名使用规范.md](中间代码变量名使用规范.md) | 中间代码命名约定 |

## 原有 Makefile

[Makefile](Makefile) 保留了早期构建目标：默认 `make` 只生成解析器；`make compiler` 的源文件列表不完整；`make std-compiler` 会编译预优化模块，其中提交的 Flex 2.5.35 C++ 扫描器与现代 `FlexLexer.h` 不兼容。首次运行可使用上面的已生成解析器构建命令。

修改 `lexer.l` 或 `parser.y` 后需要使用 Flex/Bison 重新生成解析器。`main.cpp` 中 `-O` 分支目前仍走直接汇编生成路径，中间代码优化调用被注释，不能据此认为优化流水线已经启用。
