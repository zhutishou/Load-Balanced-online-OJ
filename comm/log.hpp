#pragma once

#include <iostream>
#include <string>

#include "util.hpp"


namespace cx_log
{
    using namespace cx_util;
    //日志等级 --- 从低到高
    enum{
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    //标准输出打印日志情况，支持 << 操作符链式调用 --- LOG()<<"message";
    inline std::ostream& Log(const std::string& level,const std::string& file_name,int line)
    {
        //添加日志等级
        std::string message = "[";
        message += level;
        message += "]:";

        //添加报错文件名
        message += "[";
        message += file_name;
        message += "]:"

        //添加报错行数
        message += "[";
        message += std::to_string(line);
        message += "]:";

        //添加时间戳
        message += "[";
        message += TimeUtil::GetTimeStamp();
        message += "]";

        //向cout缓冲区中输入message，注意：不要用endl刷新缓冲区
        std::cout<<message;

        return std::cout;
    }

    //采取宏方式来打印日志信息
    //注意：当用宏来处理enum时，如果想打印的是字符串，可以加上#
    #define LOG(level) Log(#level,_FILE_,_LINE_)
}
