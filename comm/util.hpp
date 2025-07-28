#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <sys/time.h>
#include <sys/stat.h>

//该文件作用：用来存放一些工具类，并实现工具类中各函数功能

namespace cx_util
{
    //路径处理工具：
    const std::string temp_name = "./temp/";

    class PathUtil
    {
    public:
        //对需求文件路径填充
        static std::string AddSuffix(const std::string& file_name,const std::string& suffix)
        {
            std::string path_name = temp_name;
            path_name += file_name;
            path_name += suffix;
            return path_name;
        }
        
        //编译时需要构建的临时文件
        //构建源文件完整路径和文件名
        //mytest -> mytest.cpp
        static std::string Src(const std::string& file_name)
        {
            return AddSuffix(file_name,".cpp");
        }
        //构建可执行文件完整路径和文件后缀名
        //mytest -> mytest.exe
        static std::string Exe(const std::string& file_name)
        {
            return AddSuffix(file_name,".exe");
        }
        //文件在编译时报错
        static std::string CompilerError(const std::string& file_name)
        {
            return AddSuffix(file_name,".compiler_error");
        }

        //运行是需要构建的临时文件
        //构建标准输入文件完整路径和文件后缀名
        static std::string Stdin(const std::string& file_name)
        {
            return AddSuffix(file_name,".stdin");
        }
        //构建标准输出文件完整路径和文件后缀名
        static std::string Stdout(const std::string& file_name)
        {
            return AddSuffix(file_name,".stdcout");
        }
        //构建标准错误文件完整路径和文件后缀名
        //mytest -> mytest.stderr
        static std::string Stderr(const std::string& file_name)
        {
            return AddSuffix(file_name,".stderr");
        }

    private:
        
    };

    //文件判断是否创建成功类
    class FileUtil
    {
    public:
        //判断文件是否存在
        static bool IsFileExists(const std::string& file_name)
        {
            //利用struct stat结构体
            struct stat st;
            if(stat(file_name.c_str(),&st) == 0)//int stat(const char *pathname, struct stat *statbuf);
            {
                //文件确实已创建
                return true;
            }
            return false;
        }

    private:

        
    };

    //时间戳获取类
    class TimeUtil
    {
    public:
        //获取当前时间戳
        static std::string GetTimeStamp()
        {
            //利用 struct timeval结构体
            struct timeval _time;
            //获取当前时间
            gettimeofday(&_time,nullptr);
            //返回毫秒数,time_t 要转换为字符串
            return std::to_string(_time.tv_sec);
        }

    private:
        
    };
}
