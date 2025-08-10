#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

//该文件作用：只对代码进行编译

namespace cx_compiler
{
    //引入功能文件：
    using namespace cx_util;
    //引入日志文件：
    using namespace cx_log;

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
                //打印日志
                LOG(ERROR)<<"创建子进程失败"<<"\n";
                return false;
            }
            else if(pid == 0)
            {
                umask(0);//设置文件权限
                int _stdcerr = open(PathUtil::CompilerError(file_name).c_str(),O_CREAT|O_WRONLY,0644);
                //检查：
                if(_stdcerr < 0)
                {
                    LOG(WARNING)<<"没有成功形成stderr文件"<<"\n";
                    exit(1);//打印错误为1
                }
                //重定向:将标准错误重定向到_stdcerr中
                dup2(_stdcerr,2);
    

                //子进程处理部分：
                //我们现在先只实现C++:g++ -o dest src -std=c++11
                //利用execlp函数
                execlp("g++","g++","-o",PathUtil::Exe(file_name).c_str(),PathUtil::Src(file_name).c_str(),"-D","COMPILER_ONLINE","-std=c++11",nullptr);//注意-std=c++11,std前面要加一个-
                //注意点：string要转换为char*  execlp函数输入结束要加上nullptr

                //程序走到这里说明 g++编译器启动失败，请检查参数
                LOG(ERROR)<<"g++编译器启动失败，请检查参数"<<"\n";
                exit(2);//退出原因设置为2
            }
            else
            {
                //父进程：等待子进程结束
                waitpid(pid,nullptr,0);
                //检查是否编译成功
                if(FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO)<<PathUtil::Src(file_name)<<" 编译成功！ "<<"\n";
                    return true;
                }

            }
            //程序走到这，说明编译失败了
            LOG(ERROR)<<"编译失败,未形成可执行程序"<<"\n";
            return false;
        }
    private:
    };
}
