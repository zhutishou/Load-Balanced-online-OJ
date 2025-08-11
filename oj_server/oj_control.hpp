#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <mutex>
#include <cassert>
#include <jsoncpp/json/json.h>

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "../comm/httplib.h"

//用于处理核心业务

namespace cx_control
{
    //引入头文件
    using namespace cx_log;
    using namespace cx_util;
    using namespace cx_model;
    using namespace cx_view;
    using namespace httplib;

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
        void ResetLoad()
        {
            _mtx->lock();
            load = 0;
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
                    machines[which].ResetLoad();
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

            mtx.lock();
            online.insert(online.end(), offline.begin(), offline.end());
            offline.erase(offline.begin(), offline.end());
            mtx.unlock();
            LOG(INFO) << "所有的主机有上线啦!" << "\n";
            
        }
        //only for test
        void ShowMachines()
        {
            //上锁
            mtx.lock();
            //显示上线的主机情况
            std::cout<<"当前上线的主机列表: ";
            for(auto id : online)
            {
                //显示输出id情况
                std::cout<<id<<" ";
            }
            std::cout<<std::endl;
            //显示离线的主机情况
            std::cout<<"当前离线的主机列表: ";
            for(auto id : offline)
            {
                //显示输出id情况
                std::cout<<id<<" ";
            }
            std::cout<<std::endl;

            //解锁
            mtx.unlock();
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
        void RecoveryMachine()
        {
            _load_blance.OnlineMachine();
        }
        //根据题目数据构建网页
        bool AllQuestions(std::string* html)
        {
            std::vector<struct Question> all;
            //利用Model类函数
            if(_model.GetAllQuestions(&all))
            {
                //排序，使题目按顺序显示asc
                sort(all.begin(),all.end(),[](const struct Question& q1,const Question& q2){
                    return atoi(q1.number.c_str()) < atoi(q2.number.c_str());
                });
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
            //1.根据题目编号找到题目细节部分
            struct Question q;
            _model.GetOneQuestion(number,&q);

            //2.将in_json进行反序列化，得到用户提交的源码和题目id
            Json::Reader reader;
            Json::Value in_value;
            reader.parse(in_json,in_value);
            std::string code = in_value["code"].asString();

            //3.重新拼接用户代码和测试用例代码形成新的代码
            Json::Value compile_value;
            compile_value["input"] = in_value["input"].asString();
            compile_value["code"] = code + q.tail;
            compile_value["cpu_limit"] = q.cpu_limit;
            compile_value["mem_limit"] = q.mem_limit;
            Json::FastWriter writer;
            std::string compile_string = writer.write(compile_value);

            //4.选择负载情况最低的主机
            while(true)
            {
                int id;
                Machine* m = nullptr;
                if(!_load_blance.SmartChoice(&id,&m))
                {
                    //选择失败
                    break;
                }
                //选择成功，打印日志
                LOG(INFO)<<"选择主机成功,主机id: "<< id << "详细: " << m->ip << " : "<<m->port << "\n";
                
                //4.发起http请求,得到结果
                Client cli(m->ip,m->port);
                m->IncLoad();
                if(auto res = cli.Post("/compile_and_run",compile_string,"application/json;charset=utf-8"))
                {
                    //5.将结果赋值给out_json
                    if(res->status == 200)
                    {
                        //200表示传输成功
                        *out_json = res->body;
                        m->DecLoad();
                        LOG(INFO)<<"请求编译和运行成功 "<<"\n";
                        break;
                    }
                    m->DecLoad();
                }
                else
                {
                    //失败
                    //打印日志
                    LOG(ERROR)<<"当前请求的主机id: "<< id << "详细:" << m->ip <<":"<<m->port<<"可能已经离线"<<"\n";
                    _load_blance.OfflineMachine(id);
                    //only for test 打印观察在线主机和离线主机
                    _load_blance.ShowMachines();
                }

            }
        }
    private:
        Model _model;               //提供后台数据
        View _view;                 //提供html渲染功能
        LoadBalance _load_blance;   //核心负载均衡器
    };
}
