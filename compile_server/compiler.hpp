#pragma once


#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/util.hpp"


//该文件作用：只对代码进行编译

namespace cx_compiler
{
    //引入功能文件：
    using namespace cx_util;

    class Compiler
    {
    public:
        //构造与析构
        Compiler()
        {}
        ~Compiler()
        {}

        //编译功能：
        //目的：形成三个文件：.cpp .exe .stderr
        //例如：mytest -> mytest.cpp mytest.exe mytest.stderr
        static bool Compile(const std::string& file_name)
        {
            //利用父子进程：
            pid_t pid = fork();
            if(pid < 0)
            {
                //fork失败
                return false;
            }
            else if(pid == 0)
            {
                int _stdcerr = open(PathUtil::Stderr(file_name).c_str(),O_CRET|O_WRONLY,0644);
                //检查：
                if(_stdcerr < 0)
                    exit(1);//打印错误为1
                //重定向:将标准错误重定向到_stdcerr中
                dup2(_stdcerr,2);
    

                //子进程处理部分：
                //我们现在先只实现C++:g++ -o dest src -std=c++11
                //利用execlp函数
                execlp("g++","-o",PathUtil::Exe(file_name).c_str(),PathUtil::Src(file_name).c_str(),"std=c++11",nullptr);
                //注意点：string要转换为char*  execlp函数输入结束要加上nullptr

                exit(2);//退出原因设置为2
            }
            else
            {
                //父进程：等待子进程结束
                waitpid(pid,nullptr,0);
                //检查是否编译成功
                if(FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                    return true;

            }
            return false;
        }
    private:
    };
}
