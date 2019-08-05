#include <iostream>
#include <time.h>
#include <stdint.h>
#include "../src/utils/log.h"
#include "../src/utils/utils.h"


using namespace Luck;

int main()
{
<<<<<<< HEAD
   //uint64_t now = time(NULL);
    //std::string func_name = "main";
=======
    uint64_t now = time(NULL);
    std::string func_name = "main";
>>>>>>> 89b4b96838d3ed428a5b78f9884e24180b63dc1d
    
    Logger::ptr logger(new Logger);
    //LogAppender::ptr appender(new StdoutLogAppender);
    //std::cout << appender->hasFormmater() << std::endl;
    logger->AddAppender(LogAppender::ptr(new StdoutLogAppender));

    /*设置一个文件目的地，只有error才放进去*/
    LogAppender::ptr errfileAppend(new FileLogAppender("luck_error.log"));
    errfileAppend->setLevel(LogLevel::ERROR);
    logger->AddAppender(errfileAppend);
    
<<<<<<< HEAD
    //LogEvent::ptr event(new LogEvent(logger, LogLevel::INFO, "test_log.cpp", 12, 1, 2, 3, now, func_name));
    //event->getSS() << "hello luck log";
    
    //event->format("hello world id = %d", 1);
    //std::cout << event->getContent() << std::endl;
    //logger->info(event);
    //event->getSS() << "this is a warning message!!!";
    //event->format("today is %d day of a week", 5);
=======
    LogEvent::ptr event(new LogEvent(logger, LogLevel::INFO, "test_log.cpp", 12, 1, 2, 3, now, func_name));
    event->getSS() << "hello luck log";
    
    //event->format("hello world id = %d\n", 1);
    //std::cout << event->getContent() << std::endl;
    logger->info(event);
    //event->getSS() << "this is a warning message!!!";
    //event->format("today is %d day of a week\n", 5);
>>>>>>> 89b4b96838d3ed428a5b78f9884e24180b63dc1d
    //logger->debug(event);
    //std::cout << Luck::GetThreadId() << std::endl;
    LUCK_LOG_DEBUG(logger) << "this is test for LUCK_LOG_DEBUG";

<<<<<<< HEAD
    //LUCK_LOG_FMT_DEBUG(logger, "this is test for LUCK_LOG_FMT_DEBUG: num = %d", 1000);

    LUCK_LOG_ERROR(logger) << "this is test for LUCK_LOG_ERROR";
    //LUCK_LOG_FATAL(logger) << "this is test for LUCK_LOG_FATAL";
    //LUCK_LOG_FMT_ERROR(logger, "this is test for LUCK_LOG_FMT_ERROR: num = %d", 2000);
    //LUCK_LOG_FMT_FATAL(logger, "this is test for LUCK_LOG_FMT_FATAL: num = %d", 3000);
=======
    LUCK_LOG_FMT_DEBUG(logger, "this is test for LUCK_LOG_FMT_DEBUG: num = %d", 1000);

    LUCK_LOG_ERROR(logger) << "this is test for LUCK_LOG_ERROR";
    LUCK_LOG_FATAL(logger) << "this is test for LUCK_LOG_FATAL";
    LUCK_LOG_FMT_ERROR(logger, "this is test for LUCK_LOG_FMT_ERROR: num = %d", 2000);
    LUCK_LOG_FMT_FATAL(logger, "this is test for LUCK_LOG_FMT_FATAL: num = %d", 3000);
>>>>>>> 89b4b96838d3ed428a5b78f9884e24180b63dc1d
    return 0;
}

