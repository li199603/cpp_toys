#pragma once

#include "tiny_log/circular_queue.hpp"
#include "tiny_log/common.hpp"
#include <atomic>
#include <functional>
#include <mutex>

namespace tiny_log
{

class BackTracer
{
  private:
    std::atomic<bool> m_enabled;
    CircularQueue<Message> m_messages;
    mutable std::mutex m_messages_mutex;

  public:
    BackTracer();
    BackTracer(const BackTracer &other);
    ~BackTracer() = default;
    BackTracer &operator=(BackTracer other);

    void enable(size_t size);
    void disable();
    bool enabled();
    void push_back(const Message &msg);
    void foreach_pop(std::function<void(const Message &)> func);
};

BackTracer::BackTracer() : m_enabled(false)
{
}

BackTracer::BackTracer(const BackTracer &other)
{
    std::lock_guard<std::mutex> lock(other.m_messages_mutex);
    m_enabled.store(other.m_enabled.load());
    m_messages = other.m_messages;
}

BackTracer &BackTracer::operator=(BackTracer other)
{
    std::lock_guard<std::mutex> lock(other.m_messages_mutex);
    m_enabled.store(other.m_enabled.load());
    m_messages = std::move(other.m_messages);
    return *this;
}

void BackTracer::enable(size_t max_num)
{
    std::lock_guard<std::mutex> lock(m_messages_mutex);
    m_enabled.store(true);
    if (max_num == m_messages.capacity())
    {
        m_messages.clear();
    }
    else
    {
        m_messages = CircularQueue<Message>(max_num);
    }
}

void BackTracer::disable()
{
    std::lock_guard<std::mutex> lock(m_messages_mutex);
    m_enabled.store(false);
}

bool BackTracer::enabled()
{
    return m_enabled.load();
}

void BackTracer::push_back(const Message &msg)
{
    std::lock_guard<std::mutex> lock(m_messages_mutex);
    m_messages.push_back(msg);
}

void BackTracer::foreach_pop(std::function<void(const Message &)> func)
{
    std::lock_guard<std::mutex> lock(m_messages_mutex);
    while (!m_messages.empty())
    {
        Message &msg = m_messages.front();
        func(msg);
        m_messages.pop_front();
    }
}

} // namespace tiny_log