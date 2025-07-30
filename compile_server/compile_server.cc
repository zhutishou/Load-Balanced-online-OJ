#include "compiler.hpp"
#include "runner.hpp"

using namespace cx_compiler;
using namespace cx_runner;

int main()
{
    std::string code = "code";
    //编译：
    Compiler::Compile(code);
    //运行：
    Runner::Run(code);
    return 0;
}
