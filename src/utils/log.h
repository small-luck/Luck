#ifndef __LOG_H__
#define __LOG_H__

#include <list>
#include <stdint.h>

namespace Luck{

/*日志级别*/
class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    /*将日志级别转换为字符串*/
    static const lc_s8_t *to_string(LogLevel::Level level);

    /*将字符串转换为日志级别*/
    static LogLevel::Level from_string(const std::string& str);
};

class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent();
    

private:
    
};

class LogFormatter {
public:
    typedef std::shared_ptr<Formatter> ptr;
    
private:

};

class LogAppender {
public:
    typedef std::shared_ptr<Appender> ptr;
    LogAppender();
    virtual ~LogAppender();
    
private:

};

class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger();

    void log(LogLevel::Level level, LogEvent::ptr event); //逐个打印appenders的log

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warning(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    

private:
    std::list<Appender::ptr> m_appenders; //支持一个loger输出到不同的目的地
    LogLevel::Level m_level;    //日志界别
    std::string m_name; //日志名称
    LogFormatter::ptr m_formatter;  //日志格式器
    

};


}
#endif // !__LOG_H__