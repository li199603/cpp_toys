#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace tiny_log
{

using Clock = std::chrono::steady_clock;

enum Level
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

struct Message
{
    Clock::time_point time;
    Level level;
    std::shared_ptr<std::string> payload;

    Message() = default;
    Message(Level level, std::shared_ptr<std::string> payload);
};

Message::Message(Level level, std::shared_ptr<std::string> payload) : time(Clock::now()), level(level), payload(payload)
{
}

class TinyLogException : public std::exception
{
  private:
    std::string m_msg;

  public:
    TinyLogException(std::string msg);
    const char *what() const noexcept override;
};

TinyLogException::TinyLogException(std::string msg) : m_msg(msg)
{
}

const char *TinyLogException::what() const noexcept
{
    return m_msg.c_str();
}

} // namespace tiny_log