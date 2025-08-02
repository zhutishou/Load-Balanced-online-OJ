#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <sys/time.h>
#include <sys/stat.h>
#include <fstream>

//#include "log.hpp"

//该文件作用：用来存放一些工具类，并实现工具类中各函数功能

namespace cx_util
{
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
        //获取毫秒时间戳
        static std::string GettimeMs()
        {
            struct timeval _time;
            gettimeofday(&_time,nullptr);
            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    private:

    };

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
        //获取文件临时名称
        static std::string UniqFileName()
        {
            //定义一个原子性变量,通过毫秒级时间戳+该变量保证文件名称唯一性
            static std::atomic_uint id(0);
            id++;//从1开始计数
            std::string ms = TimeUtil::GettimeMs();
            std::string unq_id = to_string(id);
            return ms + "_" + unq_id;
        }
        static bool WriterFile(const std::string& target_file,const std::string& content)
        {
            std::ofstream out(target_file);
            //判断
            if(!out.is_open())
            {
                //LOG(ERROR)<<"写入文件打开失败"<<"\n";
                return false;
            }
            //写入
            out.write(content.c_str(),content.size());
            out.close();
            return true;
        }
        static bool ReadFile(const std::string& target_file,std::string* content,bool keep = false)
        {
            //先把之前content内容清空
            (*content).clear();
            //打开读文件操作
            std::ifstream in(target_file);
            //判断
            if(!in.is_open())
            {
                //LOG(ERROR)<<"读取文件打开失败"<<"\n";
                return false;
            }
            //按行读取,是否需要'\n'根据keep决定
            std::string line;
            while(std::getline(in,line))
            {
                (*content) += line;
                if(keep)
                    (*content) += "\n";
            }
            //关闭
            in.close();
            return true;
        }
    private:
       
    };
}
