#pragma once

#include <iostream>
#include <string>

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
            std::path_name = temp_name;
            path_name += file_name
            path_name += suffix;
            return path_name;
        }

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
            
        }

    private:
        
    };
}
