#include <iostream>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include "../src/log.h"
#include "../src/util.h"


using namespace Luck;

int main()
{
   //uint64_t now = time(NULL);
    //std::string func_name = "main";
    
    Logger::ptr logger(new Logger);
    //LogAppender::ptr appender(new StdoutLogAppender);
    //std::cout << appender->hasFormmater() << std::endl;
    logger->AddAppender(LogAppender::ptr(new StdoutLogAppender));

    /*设置一个文件目的地，只有error才放进去*/
    LogAppender::ptr errfileAppend(new FileLogAppender("luck_error.log"));
    errfileAppend->setLevel(LogLevel::ERROR);
    logger->AddAppender(errfileAppend);
    
    //LogEvent::ptr event(new LogEvent(logger, LogLevel::INFO, "test_log.cpp", 12, 1, 2, 3, now, func_name));
    //event->getSS() << "hello luck log";
    
    //event->format("hello world id = %d\n", 1);
    //std::cout << event->getContent() << std::endl;
    //logger->info(event);
    //event->getSS() << "this is a warning message!!!";
    //event->format("today is %d day of a week\n", 5);
    //logger->debug(event);
    //std::cout << Luck::GetThreadId() << std::endl;
    //LUCK_LOG_DEBUG(logger) << "this is test for LUCK_LOG_DEBUG";


    //LUCK_LOG_FMT_DEBUG(logger, "this is test for LUCK_LOG_FMT_DEBUG: num = %d", 1000);
    for (int i = 0; i < 200000; i++) {
        //usleep(300000);
        LUCK_LOG_FMT_ERROR(logger, "this is test for ERROR filelog, id = %d", i + 1);
    }
    
    //LUCK_LOG_FMT_ERROR(logger, "this is test for LUCK_LOG_FMT_ERROR: num = %d", 2000);
    //LUCK_LOG_FMT_FATAL(logger, "this is test for LUCK_LOG_FMT_FATAL: num = %d", 3000);


    
    
    return 0;
}

