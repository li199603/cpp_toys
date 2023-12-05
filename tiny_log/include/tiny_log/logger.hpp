#pragma once

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include "common.hpp"
#include "tiny_log/backtracer.hpp"
#include "tiny_log/fmt/core.h"
#include "tiny_log/formatter.hpp"
#include "tiny_log/sink.hpp"
#include <atomic>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

namespace tiny_log
{

class Logger
{
  protected:
    std::string m_name;
    std::vector<SinkPtr> m_sinks;
    std::atomic<int> m_level;
    BackTracer m_backtracer;

    bool should_log(Level level);
    bool should_traceback();
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

    void enable_backtracer(size_t size);
    void disable_backtracer();
    void dump_backtracer();
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

Logger::Logger(const Logger &other)
    : m_name(other.m_name), m_sinks(other.m_sinks), m_level(other.m_level.load()), m_backtracer(other.m_backtracer)
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

bool Logger::should_traceback()
{
    return m_backtracer.enabled();
}

template <typename... Args> void Logger::log(Level level, fmt::format_string<Args...> fstring, Args &&...args)
{
    bool log_flag = should_log(level);
    bool traceback_flag = should_traceback();
    if (!log_flag && !traceback_flag)
    {
        return;
    }
    std::shared_ptr<std::string> payload =
        std::make_shared<std::string>(fmt::format(fstring, std::forward<Args>(args)...));
    Message msg(level, payload);
    if (traceback_flag)
    {
        m_backtracer.push_back(msg);
    }
    if (log_flag)
    {
        log_it(msg);
    }
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

void Logger::enable_backtracer(size_t size)
{
    m_backtracer.enable(size);
}

void Logger::disable_backtracer()
{
    m_backtracer.disable();
}

void Logger::dump_backtracer()
{
    if (m_backtracer.enabled())
    {
        log_it(Message(Level::INFO,
                       std::make_shared<std::string>("****************** Backtrace Start ******************")));
        m_backtracer.foreach_pop([this](const Message &msg) { log_it(msg); });
        log_it(Message(Level::INFO,
                       std::make_shared<std::string>("******************* Backtrace End *******************")));
    }
}

} // namespace tiny_log