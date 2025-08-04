#ifndef COMPILER_ONLINE

#include "header.cpp"

#endif
//该文件是进行测试用例检测
void Test1()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(121);
    if(ret)
    {
        std::cout << "通过用例1, 测试121通过 ... OK!" << std::endl;
    } 
    else
    {
        std::cout << "没有通过用例1, 测试的值是: 121" << std::endl;
    }
}

void Test2()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(-121);
    if(ret)
    {
        std::cout << "通过用例2, 测试-121通过 ... OK!" << std::endl;
    } 
    else
    {
        std::cout << "没有通过用例2, 测试的值是: -121" << std::endl;
    }
}

void Test3()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(10);
    if(ret)
    {
        std::cout << "通过用例3, 测试10通过 ... OK!" << std::endl;
    } 
    else
    {
        std::cout << "没有通过用例3, 测试的值是: 10" << std::endl;
    }
}

int main()
{
    //测试用例：
    test1();
    test2();
    test3();
    return 0;
}
