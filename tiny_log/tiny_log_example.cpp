#include "tiny_log/logger.hpp"
#include "tiny_log/sink.hpp"
#include <memory>

int main(int argc, char ** argv)
{
    tiny_log::Logger logger("default_logger", {std::make_shared<tiny_log::Sink>()});
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn", 12);
    logger.error("error", 1);

    logger.info("Support for floats {:03.2f}", 1.23456);
    logger.info("{:<30}", "left aligned");
    
    logger.set_level(tiny_log::Level::DEBUG);
    logger.debug("debug");

    for(int i = 0; i < 10; i++)
    {
        logger.info(i);
    }
    logger.dump_backtracer();

    logger.enable_backtracer(3);
    for(int i = 0; i < 10; i++)
    {
        logger.info(i);
    }
    logger.dump_backtracer();
    logger.dump_backtracer();
    
    return 0;
}