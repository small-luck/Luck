#include "log.h"
#include <string>
#include <iostream>

namespace Luck {

static const char *LogLevel::to_string(LogLevel::Level level) 
{
    switch (level) {
    case Level::DEBUG:
        return "DEBUG";
    case Level::INFO:
        return "INFO";
    case Level::WARNING:
        return "WARNING";
    case Level::ERROR:
        return "ERROR";
    case Level::FATAL:
        return "FATAL";    
    default:
        return "UNKNOW";
    }

    return "UNKNOW";
}

static LogLevel::Level Loglevel::from_string(const std::string& str)
{
    if (str.empty()) 
        return LogLevel::Level::UNKNOW;

    if (str.conpare("DEBUG") == 0) {
        return LogLevel::Level::DEBUG;
    } else if (str.compare("INFO") == 0) {
        return LogLevel::Level::INFO;
    } else if (str.compare("WARNING") == 0) {
        return LogLevel::Level::WARNING;
    } else if (str.compare("ERROR") == 0) {
        return LogLevel::Level::ERROR;
    } else if (str.compare("FATAL") == 0) {
        return LogLevel::Level::FATAL;
    } else {
        return LogLevel::Level::UNKNOW;
    }
}

Logger::Logger() : m_level(LogLevel::Level::DEBUG) {}

void Logger::log(LogLevel::Level level) 
{
    if (level >= m_level) {
        for (auto it )
    }
}

void Logger::debug() 
{

}

void Logger::info()
{

}

void Logger::warning()
{

}

void Logger::error()
{

}

void Logger::fatal()
{

}





}