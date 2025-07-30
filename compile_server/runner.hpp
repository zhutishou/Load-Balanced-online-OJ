#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"

//该文件作用：这个文件是用来使代码运行的


namespace cx_runner
{
    //引入命名空间
    using namespace cx_util;
    using namespace cx_log;

    //定义一个运行类
    class Runner
    {
    public:
        //构造与析构
        Runner()
        {}
        ~Runner()
        {}
        
        //专门处理运行的文件
        static int Run(const std::string& file_name)
        {
            //当一段代码运行会出现三种结果：1.代码未运行成功    2.代码运行完毕，结果错误    3.代码运行完毕，结果正确
            //该文件不需要考虑代码运行完毕结果正确与否，这是由我们的测试样例来决定的，该文件只注重能否正确运行完毕

            //由可执行文件推导出-> 标准输入文件、标准输出文件和标准错误文件
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            //设置umask
            umask(0);
            //打开对应的文件(标准输入文件、标准输出文件和标准错误文件)
            int _stdin_fd = open(_stdin.c_str(),O_CREAT|O_RDONLY,0644);
            int _stdout_fd = open(_stdout.c_str(),O_CREAT|O_WRONLY,0644);
            int _stderr_fd = open(_stderr.c_str(),O_CREAT|O_WRONLY,0644);
            //判断文件是否打开成功
            if(_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                //打印日志信息
                LOG(ERROR)<<"运行时打开标准文件错误"<<"\n";
                return -1;//打开失败
            }

            //创建子进程
            pid_t pid = fork();
            if(pid < 0)
            {
                //打印日志信息
                LOG(ERROR)<<"创建子进程失败"<<"\n";
                //注意：要关闭标准文件
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; //创建子进程失败   
            }
            else if(pid == 0)
            {
                //子进程
                //利用dup重定向
                dup2(_stdin_fd,0);
                dup2(_stdout_fd,1);
                dup2(_stderr_fd,2);
                execl(_execute.c_str(),_execute.c_str(),nullptr);//第一个参数是要执行的内容，第二个参数是如何执行该内容
                //结尾要加nullptr
                exit(1);//失败退出
            }
            else
            {
                //父进程
                //父进程不需要open文件，因此都关闭
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                //等待子进程
                int status = 0;
                waitpid(pid,&status,0);
                LOG(INFO) << "运行完毕, info: " << (status & 0x7F) << "\n"; 
                return status& 0x7F;
            }
            //返回值说明：
            //1.返回值 > 0 说明程序异常，退出时收到对应的信号，返回值就是对应的信号编码
            //2.返回值 == 0 说明内部运行正常，运行结果已保存在临时文件中
            //3.返回值 < 0 说明内部出现错误

        }
    private:
        
    };
}
