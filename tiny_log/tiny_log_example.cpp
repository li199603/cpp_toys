#include "tiny_log/logger.hpp"
#include "tiny_log/sink.hpp"
#include <memory>

int main(int argc, char ** argv)
{
    tiny_log::Logger logger("aaa", {std::make_shared<tiny_log::Sink>()});
    logger.info("The answer is {}.", 42);
    return 0;
}