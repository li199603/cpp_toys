#pragma once
#include <memory>

namespace tiny_log
{

using FormatterPtr = std::shared_ptr<Formatter>;

class Sink
{
  private:
  public:
    void set_formatter(FormatterPtr formatter_ptr);
};

void Sink::set_formatter(FormatterPtr formatter_ptr)
{
}

} // namespace tiny_log