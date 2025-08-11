#include <iostream>
#include <signal.h>

#include "../comm/httplib.h"
#include "oj_control.hpp"


using namespace cx_control;
using namespace httplib;

static Control *ctrl_ptr = nullptr;
void Recovery(int signo)
{
    ctrl_ptr->RecoveryMachine();
}



int main()
{
    signal(SIGQUIT, Recovery);
    //用户请求的路由器功能
    Server svr;
    //定义一个控制器类对象
    Control ctrl;
    ctrl_ptr = &ctrl;

    //1.获取题目列表
    svr.Get("/all_questions",[&ctrl](const Request& req,Response& resp){
        std::string html;
        ctrl.AllQuestions(&html);
        resp.set_content(html,"text/html;charset = utf-8");
    });
    //2.根据题目编号获取题目内容
    // /question/id 正则表达式
    svr.Get(R"(/question/(\d+))",[&ctrl](const Request& req,Response& resp){
        std::string number = req.matches[1];//id
        std::string html;
        ctrl.Questions(number,&html);
        resp.set_content(html,"text/html;charset = utf-8");
    });
    //3.根据用户提交的代码进行判题
    svr.Post(R"(/judge/(\d+))",[&ctrl](const Request& req,Response& resp){
        std::string number = req.matches[1];//id
        std::string result_json;
        ctrl.Judge(number,req.body,&result_json);
        resp.set_content(result_json,"application/json;charset = utf-8");

        // std::string number = req.matches[1];//id
        // resp.set_content("判断指定的题目: " + number,"text/plain;charset = utf-8");
    });
    //设置路径
    svr.set_base_dir("./wwwroot");
    //监听窗口
    svr.listen("0.0.0.0",8080);
    return 0;
}
