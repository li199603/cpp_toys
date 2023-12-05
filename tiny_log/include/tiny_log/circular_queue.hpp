#pragma once

#include "tiny_log/common.hpp"
#include <vector>

namespace tiny_log
{

template <typename T> class CircularQueue
{
  private:
    size_t m_start, m_end; // valid values range [m_start, m_end)
    std::vector<T> m_vec;

  public:
    CircularQueue();
    CircularQueue(size_t capacity);
    ~CircularQueue() = default;

    void push_back(const T &value);
    void pop_front();
    void clear();
    T &front();

    size_t size();
    size_t capacity();
    bool empty();
    bool full();
};

template <typename T> CircularQueue<T>::CircularQueue() : CircularQueue(0)
{
}

template <typename T> CircularQueue<T>::CircularQueue(size_t capacity) : m_start(0), m_end(0), m_vec(capacity + 1)
{
}

template <typename T> void CircularQueue<T>::push_back(const T &value)
{
    m_vec[m_end] = value;
    m_end = (m_end + 1) % m_vec.capacity();
    if (m_start == m_end)
    {
        m_start = (m_start + 1) % m_vec.capacity();
    }
}

template <typename T> void CircularQueue<T>::pop_front()
{
    if (empty())
    {
        throw TinyLogException("There is no available data in the circular queue.");
    }
    m_start = (m_start + 1) % m_vec.capacity();
}

template <typename T> void CircularQueue<T>::clear()
{
    m_start = 0;
    m_end = 0;
}

template <typename T> T &CircularQueue<T>::front()
{
    return m_vec[m_start];
}

template <typename T> size_t CircularQueue<T>::size()
{
    if (m_start < m_end)
    {
        return m_end - m_start;
    }
    if (m_start > m_end)
    {
        return m_vec.capacity() - (m_start - m_end);
    }
    return 0;
}

template <typename T> size_t CircularQueue<T>::capacity()
{
    return m_vec.capacity() - 1;
}

template <typename T> bool CircularQueue<T>::empty()
{
    return size() == 0;
}

template <typename T> bool CircularQueue<T>::full()
{
    return size() == capacity();
}

} // namespace tiny_log