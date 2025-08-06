#pragma once

#include <iostream>
#include <string>
#include <mutex>
#include <cassert>

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "../comm/util.hpp"
#include "../comm/log.hpp"

//用于处理核心业务

namespace cx_control
{
    //引入头文件
    using namespace cx_log;
    using namespace cx_util;
    using namespace cx_model;
    using namespace cx_view;

    //机器类
    class Machine
    {
    public:
        //定义成员变量
        std::string ip;     //主机IP
        int port;           //主机端口
        uint64_t load;      //服务负载
        std::mutex* _mtx;   //定义锁(mutex锁禁止拷贝)
    public:
        //构造和析构
        Machine()
            :ip(""),port(0),load(0),_mtx(nullptr)
        {}
        ~Machine(){}
        //主要功能：
        //提升主机负载
        void IncLoad()
        {
            //加锁
            _mtx->lock();
            //增加负载
            load++;
            //解锁
            _mtx->unlock();
        }
        //减少主机负载
        void DecLoad()
        {
            //加锁
            _mtx->lock();
            //减少负载
            load--;
            //解锁
            _mtx->unlock();
        }
        //获取主机负载
        uint64_t Load()
        {
            uint64_t _load = 0;
            //加锁
            _mtx->lock();
            //获取负载
            _load = load;
            //解锁
            _mtx->unlock();
            return _load;
        }
    };

    //主机位置信息路径
    const std::string service_machine = "./conf/service_machine.conf";

    //负载均衡类
    class LoadBalance
    {
    public:
        //构造和析构
        LoadBalance()
        {
            assert(LoadConf(service_machine));
            //打印日志
            LOG(INFO)<<"加载"<<service_machine<<"成功"<<"\n";
        }
        ~LoadBalance(){}
        //加载一个主机
        bool LoadConf(const std::string& machine_conf)
        {
            //打开文件
            std::ifstream in(machine_conf);
            if(!in.is_open())
            {
                //文件打开失败
                LOG(FATAL) << " 加载: " << machine_conf << " 失败"<<"\n";
                return false;
            }
            std::string line;
            while(std::getline(in,line))
            {
                //每行相当于读入一个主机
                std::vector<std::string> tokens;
                StringUtil::SplitString(line,&tokens,":");
                //判断是否切分正确
                if(tokens.size()!=2)
                {
                    LOG(WARNING) << " 切分 " << line << "失败" << "\n";
                    continue;
                }
                //写入machines和online中
                Machine m;
                m.ip = tokens[0];
                m.port = atoi(tokens[1].c_str());
                m.load = 0;
                m._mtx = new std::mutex();
                online.push_back(machines.size());
                machines.push_back(m);
            }
            //关闭文件
            in.close();
            return true;
        }
        //根据负载情况智能选择
        bool SmartChoice(int *id, Machine **m)
        {
            //利用轮询找到负载最小的主机
            //加锁
            mtx.lock();
            int online_num = online.size();
            if(online_num == 0)
            {
                //所有主机都已下线
                mtx.unlock();
                LOG(FATAL) << " 所有的后端编译主机已经离线, 请运维的同事尽快查看 "<< "\n";
                return false;
            }
            //遍历查找
            *id = online[0];//0号开始查找
            *m = &machines[online[0]];
            uint64_t min_load = machines[online[0]].Load();
            for(int i = 1;i < online_num; i++)
            {
                uint64_t cur_load = machines[online[i]].Load();
                if(cur_load < min_load)
                {
                    //更换
                    min_load = cur_load;
                    *id = online[i];
                    *m = &machines[online[i]];
                }
            }
            //解锁
            mtx.unlock();
            return true;
        }
        //将which主机离线并记录
        void OfflineMachine(int which)
        {
            //加锁
            mtx.lock();
            for(auto iter = online.begin(); iter != online.end(); iter++)
            {
                if(*iter == which)
                {
                    //将该主机下线
                    online.erase(iter);
                    offline.push_back(which);
                    break;
                }
            }
            //解锁
            mtx.unlock();
        }
        void OnlineMachine()
        {
            //
        }
        void ShowMachines()
        {
            //
        }
    private:
        std::vector<Machine> machines;//所有可以参与编译运行服务主机
        std::vector<int> online;//所有在线主机ID
        std::vector<int> offline;//所有离线主机ID
        std::mutex mtx;//定义锁
    };







    //核心控制器
    class Control
    {
    public:
        Control(){}
        ~Control(){}
        //根据题目数据构建网页
        bool AllQuestions(std::string* html)
        {
            vector<struct Question> all;
            //利用Model类函数
            if(_model.GetAllQuestions(&all))
            {
                //获取题目信息成功，根据题目列表构建网页
                _view.AllExpandHtml(all, html);
                return true;
            }
            else
            {
                *html = "获取题目失败，形成列表失败";
                return false;
            }
        }
        //根据某道题目构建网页
        bool Questions(const std::string number,std::string* html)
        {
            struct Question q;
            if(_model.GetOneQuestion(number,&q))
            {
                //跳转到具体某题时，获取题目成功
                _view.OneExpandHtml(q, html);
                return true;
            }
            else
            {
                *html = "指定题目: " + number + " 不存在!";
                return false;
            }
        }
        //判断用户编写的代码正确与否功能
        void Judge(const std::string number,const std::string in_json,std::string* out_json)
        {

        }
    private:
        Model _model;//提供后台数据
        View _view;//提供html渲染功能
        LoadBalance _load_blance; //核心负载均衡器
    };
}
