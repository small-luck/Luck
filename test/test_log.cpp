#include <iostream>
#include <time.h>
#include <stdint.h>
#include "../src/utils/log.h"

using namespace Luck;

int main()
{
    uint64_t now = time(NULL);
    std::string func_name = "main";
    
    Logger::ptr logger(new Logger);
    logger->AddAppender(LogAppender::ptr(new StdoutLogAppender));
    
    LogEvent::ptr event(new LogEvent(logger, LogLevel::INFO, "test_log.cpp", 12, 1, 2, 3, now, func_name));

    event->format("hello world id = %d\n", 1);
    //std::cout << event->getContent() << std::endl;
    logger->log(LogLevel::INFO, event);
    return 0;
}

