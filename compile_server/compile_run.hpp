#pragma once
#include <jsoncpp/json/json.h>
#include <signal.h>
#include <unistd.h>

#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace cx_compile_and_run
{
    //命名空间引入
    using namespace cx_compiler;
    using namespace cx_runner;
    using namespace cx_log;
    using namespace cx_util;

    class CompileAndRun
    {
    public:
        //将使用完的临时文件删除
        static void RemoveTempFile(const std::string& file_name)    
        {
            //由于我们不知道形成了多少个文件，所以逐个if判断，成立有用unlink删除文件
            //Src-cpp
            std::string _src = PathUtil::Src(file_name);
            if(FileUtil::IsFileExists(_src))
                unlink(_src.c_str());
            //compiler_error
            std::string _compiler_error = PathUtil::CompilerError(file_name);
            if(FileUtil::IsFileExists(_compiler_error))
                unlink(_compiler_error.c_str());
            //exe
            std::string _exe = PathUtil::Exe(file_name);
            if(FileUtil::IsFileExists(_exe))
                unlink(_exe.c_str());
            //stdin
            std::string _stdin = PathUtil::Stdin(file_name);
            if(FileUtil::IsFileExists(_stdin))
                unlink(_stdin.c_str());
            //stdout
            std::string _stdout = PathUtil::Stdout(file_name);
            if(FileUtil::IsFileExists(_stdout))
                unlink(_stdout.c_str());
            //stderr
            std::string _stderr = PathUtil::Stderr(file_name);
            if(FileUtil::IsFileExists(_stderr))
                unlink(_stderr.c_str());
        }

        //根据status_code(code)填充reason键值对
        static std::string CodeToDesc(int code,const std::string& file_name)
        {
            std::string desc;//定义一个描述词
            switch(code)
            {
                case SIGXCPU://24
                    desc = "CPU使用超时";
                    break;
                case SIGFPE: // 8
                    desc = "浮点数溢出";
                    break;
                case SIGABRT: // 6
                    desc = "内存空间不足";
                    break;
                case 0:
                    desc = "编译运行成功";
                    break;
                case -1:
                    desc = "代码为空";
                    break;
                case -2:
                    desc = "内部错误";
                    break;
                case -3:
                    // desc = "代码编译错误";
                    FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                    break;
                default:
                    desc = "未知: " + std::to_string(code);
                    break;
            }
            return desc;
        }
        //处理编译链接部分全部内容
        static void start(const std::string& in_json,std::string* out_json)
        {
            //输入：code-代码 input-用户输入 cpu_limit-时间限制 mem_limit-空间限制
            //输出：status-状态码 reason-状态结果 stdout-程序运行完毕结果 stdout-程序运行完毕错误信息
            //利用json处理，例如：in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
            Json::Value in_value;//存储键值对
            Json::Reader reader;//读取
            reader.parse(in_json,in_value);//将字符串转为键值对

            //得到对应的信息：
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();
            
            int status_code = 0;//定义状态信息
            Json::Value out_value;//输出键值对
            int run_result = 0;
            std::string file_name;//定义代码形成的文件名(无目录、路径) 

            //判断处理：
            if(code.size() == 0)
            {
                //用户没有输入任何内容，代码为空情况
                status_code = -1;
                goto END;
            }

            //形成带路径等信息的文件名
            file_name = FileUtil::UniqFileName();//获取在临时文件中路径等信息

            //形成临时Src文件
            if(!FileUtil::WriterFile(PathUtil::Src(file_name),code))
            {
                //将code写入到临时文件中，并判断是否成功(内部错误)
                status_code = -2;//写入失败
                goto END;
            }

            //编译:
            if(!Compiler::Compile(file_name))
            {
                //编译失败
                status_code = -3;//代码编译错误
                goto END;
            }

            //运行,其结果存在三种情况(<0 ==0 >0)，所以我们用run_result提前存放
            run_result = Runner::Run(file_name,cpu_limit,mem_limit);
            if(run_result < 0)
            {
                //代码未运行成功（内部错误），用-2存放，和形成临时文件同理，皆是服务器方面错误
                status_code = -2;
            }
            else if(run_result > 0)
            {
                //程序异常，返回信号
                status_code = run_result;
            }
            else
            {
                //程序正常运行
                status_code = 0;
            }
        //goto 跳转位置    
        END:
            //填写输出键值对
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name);
            //根据status_code == 0来完成标准输出和标准错误
            if(status_code == 0)
            {
                //前面所有皆正常
                std::string _stdout;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true);
                out_value["stdout"] = _stdout;

                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr, true);
                out_value["stderr"] = _stderr;
            }
            //向out_json中写入out_value键值对内容
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            //删除临时文件
            RemoveTempFile(file_name);
        }

    private:

    };
}
