/*
reference:
https://github.com/eglimi/cpptime
*/
#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

namespace toys
{
using TaskFunc = std::function<void()>;

struct Task
{
    uint64_t id;
    uint64_t period;
    bool repeated;
    TaskFunc func;
    bool removed;
    Task(uint64_t id, uint64_t period, bool repeated, TaskFunc func)
        : id(id), period(period), repeated(repeated), func(func), removed(false)
    {
    }
};

class Timer
{
  private:
    // var
    std::thread m_worker;
    std::atomic<bool> m_stop;
    std::multimap<uint64_t, Task> m_tasks;
    std::mutex m_tasks_mutex;
    std::condition_variable m_condition;
    uint64_t m_cur_id;
    // func
    void run();
    uint64_t now();

  public:
    // func
    Timer();
    ~Timer();
    uint64_t add(uint64_t period_ms, bool repeated, TaskFunc func);
    bool remove(uint64_t);
};

Timer::Timer() : m_stop(false)
{
    m_worker = std::thread(&Timer::run, this);
}

Timer::~Timer()
{
    m_stop.store(true);
    m_condition.notify_all();
    m_worker.join();
}

uint64_t Timer::add(uint64_t period_ms, bool repeated, TaskFunc func)
{
    uint64_t when = now() + period_ms;
    Task task(m_cur_id, period_ms, repeated, func);
    {
        std::lock_guard<std::mutex> lock(m_tasks_mutex);
        m_tasks.insert({when, task});
    }
    m_condition.notify_all();
    return m_cur_id++;
}

bool Timer::remove(uint64_t id)
{
    bool flag = false;
    std::lock_guard<std::mutex> lock(m_tasks_mutex);
    std::multimap<uint64_t, Task>::iterator it =
        std::find_if(m_tasks.begin(), m_tasks.end(),
                     [id](const std::pair<uint64_t, Task> &item) -> bool { return item.second.id == id; });
    if (it != m_tasks.end())
    {
        it->second.removed = true;
        flag = true;
    }
    return flag;
}

void Timer::run()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_tasks_mutex);
        m_condition.wait(lock, [this]() -> bool { return !m_tasks.empty() || m_stop; });
        if (m_stop)
        {
            break;
        }
        uint64_t cur_time = now();
        std::multimap<uint64_t, Task>::iterator it = m_tasks.begin();
        uint64_t task_time = it->first;
        if (cur_time >= task_time)
        {
            Task &cur_task = it->second;
            if (!cur_task.removed)
            {
                lock.unlock();
                cur_task.func();
                lock.lock();
                if (cur_task.repeated && !cur_task.removed)
                {
                    uint64_t when = cur_time + cur_task.period;
                    Task new_task(cur_task.id, cur_task.period, cur_task.repeated, cur_task.func);
                    m_tasks.insert({when, new_task});
                }
            }
            m_tasks.erase(it);
        }
        else
        {
            m_condition.wait_for(lock, std::chrono::milliseconds(task_time - cur_time));
        }
    }
}

uint64_t Timer::now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
}

} // namespace toys