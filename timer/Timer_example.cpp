#include "Timer.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

void print_func(int id)
{
    // 获取自纪元以来的时间点
    auto now = std::chrono::system_clock::now();

    // 转换为自纪元以来的毫秒数
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // 转换为日期和时间
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&time);

    // 输出日期和时间
    std::cout << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << std::setfill('0') << millis % 1000
              << " id = " << id << std::endl;
}

int main(int argc, char **argv)
{   
    toys::Timer *timer_p = new toys::Timer();
    timer_p->add(2000, false, std::bind(print_func, 0));
    uint64_t id = timer_p->add(200, true, std::bind(print_func, 1));
    timer_p->add(500, true, std::bind(print_func, 2));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer_p->remove(id);
    std::cout << "timer_p->remove(id)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete timer_p;
    std::cout << "delete timer_p" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}