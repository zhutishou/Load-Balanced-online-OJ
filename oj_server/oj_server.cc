#include <iostream>

#include "../comm/httplib.h"

using namespace httplib;


int main()
{
    //用户请求的路由器功能
    Server svr;

    //1.获取题目列表
    svr.Get("all_questions",[](const Request& req,Response& resp){
        resp.set_content("这是题目列表","text/plain;charset = utf-8");
    });
    //2.根据题目编号获取题目内容
    // /question/id 正则表达式
    svr.Get(R"(/question/(\d+))",[](const Request& req,Response& resp){
        std::string number = req.matches[1];//id
        resp.set_content("这是指定的题目: " + number,"text/plain;charset = utf-8");
    });
    //3.根据用户提交的代码进行判题
    svr.Get(R"(/judge/(d+))",[](const Request& req,Response& resp){
        std::string number = req.matches[1];//id
        resp.set_content("判断指定的题目: " + number,"text/plain;charset = utf-8");
    });
    //设置路径
    svr.set_base_dir("./wwwroot");
    //监听窗口
    svr.listen("0.0.0.0",8080);
    return 0;
}
