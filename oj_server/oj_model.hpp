#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdlib>
#include <cassert>

#include "../comm/log.hpp"
#include "../comm/util.hpp"

//该文件作用：提供对数据操作

namespace cx_model
{
    using namespace cx_log;
    using namespace cx_util;

    //定义一个结构体存放题目相关信息
    struct Question
    {
        std::string number;     //题目编号
        std::string title;      //题目名称
        std::string star;       //题目难度:简单、中等、困难
        int cpu_limit;          //时间限制(s)
        int mem_limit;          //空间限制(KB)
        std::string desc;       //题目描述
        std::string header;     //用户提交的代码
        std::string tail;       //测试用例代码
    };

    //题目路径和题目列表
    std::string questions_path = "./questions/";
    std::string questions_list = "./questions/questions.list";
    
    //定义处理模块类
    class Model 
    {
    public:
        Model()
        {
            //判断是否加载题目列表
            assert(LoadQuestionList(questions_list));
        }
        ~Model(){}

        //加载题目列表
        bool LoadQuestionList(const std::string& questions_list)
        {
            //加载配置文件 --- questions/questions.list + 题目编号文件
            //打开文件
            ifstream in(questions_list);
            //判断
            if(!in.is_open())
            {
                LOG(FATAL)<<"加载题库失败,请检查是否存在题库文件"<<"\n";
                return false;
            }
            //按行读取
            std::string line;
            while(std::getline(in,line))
            {
                vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                //1 判断回文数 简单 1 30000
                //判断size是否正确
                if(tokens.size() != 5)
                {
                    LOG(WARNING) << "加载部分题目失败, 请检查文件格式" << "\n";
                    return false;
                }
                //定义一个Question类对象，根据题目编号存放Question
                Question q;
                //根据tokens填入Question对象内容
                q.number = tokens[0];
                q.title = tokens[1];
                q.star = tokens[2];
                q.cpu_limit = atoi(tokens[3].c_str());
                q.mem_limit = atoi(tokens[4].c_str());
                //填入Question中其他成员变量
                std::string path = questions_path;
                path += q.number;// ./questions/编号
                path += "/";//  ./questions/编号/
                //利用util中FileUtil填充desc.txt、header.cpp、tail.cpp文件
                FileUtil::ReadFile(path + "desc.txt", &(q.desc), true);
                FileUtil::ReadFile(path + "header.cpp",&(q.header),true);
                FileUtil::ReadFile(path + "tail.cpp",&(q.tail),true);
                //将类对象插入到unordered_map中
                questions.insert({q.number,q});
            }
            //以上内容全部完成，打印日志
            LOG(INFO) << "加载题库...成功!" << "\n";
            //关闭文件
            in.close();
            return true;
        }

        //向用户提供所有题目列表
        bool GetAllQuestions(vector<Question> *out)
        {
            //判断是否存在题目
            if(questions.size() == 0)
            {
                LOG(ERROR) << "用户获取题库失败" << "\n";
                return false;
            }
            //将unordered_map中second属性插入out数组中
            for(const auto& q : questions)
            {
                *out->push_back(q.second);
            }
            return true;
        }

        //根据题目编号找到具体的某题
        bool GetOneQuestion(const std::string& number,Question* q)
        {
            const auto& iter = questions.find(number);
            //检查
            if(iter == questions.end())
            {
                LOG(ERROR) << "用户获取题目失败, 题目编号: " << number << "\n";
                return false;
            }
            (*q) = iter->second;
            return true;
        }
    private:
        //根据题目编号找到相应的题目信息
        unordered_map<std::string,Question> questions;
    };
}
