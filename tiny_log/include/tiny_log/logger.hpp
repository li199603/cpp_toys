#pragma once

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include <atomic>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include "common.hpp"
#include "tiny_log/fmt/core.h"
#include "tiny_log/formatter.hpp"
#include "tiny_log/sink.hpp"

namespace tiny_log
{

using SinkPtr = std::shared_ptr<Sink>;
using FormatterPtr = std::shared_ptr<Formatter>;

class Logger
{
  private:
    std::string m_name;
    std::vector<SinkPtr> m_sinks;
    std::atomic<int> m_level;

    bool should_log(Level level);
    // bool should_backtrace();
    template <typename... Args> void log(Level level, fmt::format_string<Args...> fstring, Args &&...args);
    void log_it(const Message &msg);

  public:
    Logger(std::string name);
    Logger(std::string name, SinkPtr sink_ptr);
    Logger(std::string name, std::initializer_list<SinkPtr> sinks);
    Logger(const Logger &other);
    ~Logger() = default;
    Logger &operator=(Logger other);
    void swap(Logger &other);

    template <typename Arg> void debug(const Arg &arg);
    template <typename Arg> void info(const Arg &arg);
    template <typename Arg> void warn(const Arg &arg);
    template <typename Arg> void error(const Arg &arg);
    template <typename... Args> void debug(fmt::format_string<Args...> fstring, Args &&...args);
    template <typename... Args> void info(fmt::format_string<Args...> fstring, Args &&...args);
    template <typename... Args> void warn(fmt::format_string<Args...> fstring, Args &&...args);
    template <typename... Args> void error(fmt::format_string<Args...> fstring, Args &&...args);

    void set_level(Level level);
    void set_formatter(FormatterPtr formatter_ptr);

    Level get_level();
    std::string get_name();
    std::vector<SinkPtr> &get_sinks();

    // void enable_backtrace(size_t num_msg);
    // void disable_backtrace();
    // void dump_backtrace();
};

Logger::Logger(std::string name) : Logger(std::move(name), {})
{
}

Logger::Logger(std::string name, SinkPtr sink_ptr) : Logger(std::move(name), {std::move(sink_ptr)})
{
}

Logger::Logger(std::string name, std::initializer_list<SinkPtr> sinks)
    : m_name(std::move(name)), m_sinks(sinks.begin(), sinks.end()), m_level(Level::INFO)
{
}

Logger::Logger(const Logger &other) : m_name(other.m_name)
{
}

Logger &Logger::operator=(Logger other)
{
    swap(other);
    return *this;
}

void Logger::swap(Logger &other)
{
    m_name.swap(other.m_name);
    m_sinks.swap(other.m_sinks);
    int tmp_level = m_level.exchange(other.m_level.load());
    other.m_level.store(tmp_level);
}

template <typename Arg> void Logger::debug(const Arg &arg)
{
    debug("{}", arg);
}

template <typename Arg> void Logger::info(const Arg &arg)
{
    info("{}", arg);
}

template <typename Arg> void Logger::warn(const Arg &arg)
{
    warn("{}", arg);
}

template <typename Arg> void Logger::error(const Arg &arg)
{
    error("{}", arg);
}

template <typename... Args> void Logger::debug(fmt::format_string<Args...> fstring, Args &&...args)
{
    log(Level::DEBUG, fstring, std::forward<Args>(args)...);
}

template <typename... Args> void Logger::info(fmt::format_string<Args...> fstring, Args &&...args)
{
    log(Level::INFO, fstring, std::forward<Args>(args)...);
}

template <typename... Args> void Logger::warn(fmt::format_string<Args...> fstring, Args &&...args)
{
    log(Level::WARN, fstring, std::forward<Args>(args)...);
}

template <typename... Args> void Logger::error(fmt::format_string<Args...> fstring, Args &&...args)
{
    log(Level::ERROR, fstring, std::forward<Args>(args)...);
}

bool Logger::should_log(Level level)
{
    return level >= m_level.load();
}

template <typename... Args> void Logger::log(Level level, fmt::format_string<Args...> fstring, Args &&...args)
{
    if (!should_log(level))
    {
        return;
    }
    std::shared_ptr<std::string> payload =
        std::make_shared<std::string>(fmt::format(fstring, std::forward<Args>(args)...));
    Message msg(level, payload);
    log_it(msg);
}

void Logger::log_it(const Message &msg)
{
    for (const SinkPtr &sink_ptr : m_sinks)
    {
        printf("%s", msg.payload->c_str());
        printf("\n");
    }
}

void Logger::set_level(Level level)
{
    m_level.store(level);
}

void Logger::set_formatter(FormatterPtr formatter_ptr)
{
    for (SinkPtr &sink_ptr : m_sinks)
    {
        sink_ptr->set_formatter(formatter_ptr);
    }
}

Level Logger::get_level()
{
    return Level(m_level.load());
}

std::string Logger::get_name()
{
    return m_name;
}

std::vector<SinkPtr> &Logger::get_sinks()
{
    return m_sinks;
}

} // namespace tiny_log