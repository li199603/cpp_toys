#include "ThreadPool.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int func1(int a, int b)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return a + b;
}

void func2(int &v)
{
    v += 1;
}

int main(int argc, char **argv)
{
    toys::ThreadPool tp(3);
    int v = 3;
    auto f1 = tp.add(&func1, 9, 6);
    auto f2 = tp.add(&func2, std::ref(v));
    auto f3 = tp.add(&func1, 9, 6);
    auto f4 = tp.add(&func1, 9, 6);
    auto f5 = tp.add(&func1, 9, 6);
    auto f6 = tp.add(&func1, 9, 6);

    f2.wait();
    std::cout << v << std::endl;
    std::cout << f1.get() << std::endl;
    std::cout << f3.get() << std::endl;
    std::cout << f4.get() << std::endl;
    std::cout << f5.get() << std::endl;
    std::cout << f6.get() << std::endl;

    return 0;
}