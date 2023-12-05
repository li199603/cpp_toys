#pragma once

#include "tiny_log/formatter.hpp"
#include <atomic>
#include <memory>
#include <string>

namespace tiny_log
{

class Sink
{
  protected:
    std::string m_name;
    std::atomic<int> m_level;

  public:
    void set_formatter(FormatterPtr formatter_ptr);
};

using SinkPtr = std::shared_ptr<Sink>;

void Sink::set_formatter(FormatterPtr formatter_ptr)
{
}

} // namespace tiny_log