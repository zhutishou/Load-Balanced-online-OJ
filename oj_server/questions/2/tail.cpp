#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    vector<int> v = {1, 2, 3, 4, 5, 6};
    int max = Solution().Max(v);
    if (max == 6)
    {
        std::cout << "Test 1 .... OK" << std::endl;
    }
    else
    {
        std::cout << "Test 1 .... Failed" << std::endl;
    }
}

void Test2()
{
    vector<int> v = {-1, -2, -3, -4, -5, -6};
    int max = Solution().Max(v);
    if (max == -1)
    {
        std::cout << "Test 2 .... OK" << std::endl;
    }
    else
    {
        std::cout << "Test 2 .... Failed" << std::endl;
    }
}

void Test3()
{
    vector<int> v = {3,6,8,10,2,1};
    int max = Solution().Max(v);
    if (max == 10)
    {
        std::cout << "Test 3 .... OK" << std::endl;
    }
    else
    {
        std::cout << "Test 3 .... Failed" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    Test3();
    return 0;
}
