#pragma once

#include <iostream>
#include <vector>
#include <ctemplate/template.h>
#include "oj_model.hpp"

//用于处理网页渲染功能---template实现

namespace cx_view
{
    //命名空间引入
    using namespace cx_model;
    //渲染文件路径
    const std::string template_path = "./template_html";

    class View
    {
    public:
        View(){}
        ~View(){}
        //渲染所有文件：
        void AllExpandHtml(const vector<Question>& questions,std::string* html)
        {
            //形成路径
            std::string src_html = template_path + "all_questions.html";
            //形成数据字典
            ctemplate::TemplateDictionary root("all_questions");//unordered_map
            for(auto& q:questions)
            {
                ctemplate::TemplateDictionary* sub = root.AddSectionDictionary("")
                sub->SetValue("number",q.number);
                sub->SetValue("title",q.title);
                sub->SetValue("star",q.star);             
            }
            //获取被渲染对象
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
            //完成渲染
            tpl->Expand(html,&root);
        }
        void OneExpandHtml(const struct Question& question,std::string* html)
        {
            //形成路径
            std::string src_html = template_path + "one_question.html";
            //形成数据字典
            ctemplate::TemplateDictionary root("one_question");
            root->SetValue("number",q.number);
            root->SetValue("title",q.title);
            root->SetValue("stat",q.star);
            root->SetValue("desc",q.desc);
            root->SetValue("pre_code",q.header);
            //获取被渲染对象
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
            //完成渲染
            tpl->Expand(html,&root);
        }
    private:
        
    };

}
