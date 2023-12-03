#pragma once
#include <chrono>
#include <memory>
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

    Message(Level level, std::shared_ptr<std::string> payload);
};

Message::Message(Level level, std::shared_ptr<std::string> payload) : time(Clock::now()), level(level), payload(payload)
{
}

} // namespace tiny_log