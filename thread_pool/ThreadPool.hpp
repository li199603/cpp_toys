/*
reference:
https://github.com/vit-vit/CTPL
https://github.com/progschj/ThreadPool
*/
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace toys
{

using TaskFunc = std::function<void()>;

class ThreadPool
{
  private:
    // var
    std::vector<std::thread> m_wokers;
    std::queue<TaskFunc> m_tasks;
    std::mutex m_tasks_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;
    // func
    void working();

  public:
    // func
    ThreadPool(int num_worker);
    ~ThreadPool();
    template <typename F, typename... Args>
    auto add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

ThreadPool::ThreadPool(int num_worker) : m_stop(false)
{
    for (int i = 0; i < num_worker; i++)
    {
        m_wokers.emplace_back(&ThreadPool::working, this);
    }
}

ThreadPool::~ThreadPool()
{
    m_stop.store(true);
    m_condition.notify_all();
    for (std::thread &worker : m_wokers)
    {
        worker.join();
    }
}

void ThreadPool::working()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_tasks_mutex);
        m_condition.wait(lock, [this]() -> bool { return !m_tasks.empty() || m_stop; });
        if (m_stop)
        {
            break;
        }
        TaskFunc task(std::move(m_tasks.front()));
        m_tasks.pop();
        lock.unlock();
        task();
    }
}

template <typename F, typename... Args>
auto ThreadPool::add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using ReturnType = typename std::result_of<F(Args...)>::type;
    auto pkgt_ptr =
        std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<ReturnType> result = pkgt_ptr->get_future();
    {
        std::lock_guard<std::mutex> lock(m_tasks_mutex);
        m_tasks.emplace([pkgt_ptr]() { (*pkgt_ptr)(); });
    }
    m_condition.notify_one();
    return result;
}

} // namespace toys
