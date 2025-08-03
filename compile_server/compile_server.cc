#include "compile_run.hpp"
#include "../comm/httplib.h"

using namespace cx_compile_and_run;
using namespace httplib;

void Usage(std::string proc)
{
    std::cerr << "Usage: " << "\n\t" << proc << " port" << std::endl;
}
//实际调用时，以带参数调用 ./compile_server port
int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        //说明调用错误
        Usage(argv[0]);
        return 1;
    }
    Server svr;
    //调用compile_and_run
    svr.Post("/compile_and_run",[](const Request& req,Response& resp){
        //用户请求的服务正文就是我们想要的代码---in_json
        std::string in_json = req.body;
        std::string out_json;
        if(!in_json.empty())
        {
            //判断传入的正文是否为空
            //正文存在调用编译运行文件
            CompileAndRun::start(in_json,&out_json);
            //将out_json返回给用户浏览器
            resp.set_content(out_json,"application/json;charset=utf-8");
        }
    });
    //调用监听窗口 port-argv[1]
    svr.listen("0.0.0.0",atoi(argv[1]));

    return 0;
}


// int main()
// {

//     // in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
//     // out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
//     // 通过http 让client 给我们 上传一个json string
//     // 下面的工作，充当客户端请求的json串
//     std::string in_json;
//     Json::Value in_value;
//     //R"()", raw string
//     in_value["code"] = R"(#include<iostream>
//     int main(){
//         std::cout << "这是一个测试" << std::endl;
//         return 0;
//     })";
//     in_value["input"] = "";
//     in_value["cpu_limit"] = 1;
//     in_value["mem_limit"] = 10240*3;
//     Json::FastWriter writer;
//     in_json = writer.write(in_value);
//     std::cout << in_json << std::endl;
//     //这个是将来给客户端返回的json串
//     std::string out_json;
//     CompileAndRun::start(in_json, &out_json);

//     std::cout << out_json << std::endl;
//     return 0;
// }

//例：
// #include "compiler.hpp"
// #include "runner.hpp"

// using namespace cx_compiler;
// using namespace cx_runner;

// int main()
// {
//     std::string code = "code";
//     //编译：
//     Compiler::Compile(code);
//     //运行：
//     Runner::Run(code);
//     return 0;
// }
