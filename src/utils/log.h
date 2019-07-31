#ifndef __LOG_H__
#define __LOG_H__

#include <list>
#include <stdint.h>
#include <memory>

namespace Luck{

class Logger;

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

/* 日志事件 */
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;

    /* 构造函数 */
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
               const char* file, uint32_t line, uint32_t elapse, 
               uint32_t thread_id, uint32_t fiber_id, uint64_t time,
               std::string& thread_name);
    
    /* 返回文件名 */
    const char* getFile() const { return m_file; }

    /* 返回行号 */
    uint32_t getLine() const { return m_line; }

    /* 返回毫秒数 */
    uint32_t getElapse() const { return m_elapse; }

    /* 返回线程ID */
    uint32_t getThreadId() const { return m_threadId; }

    /* 返回协程ID */
    uint32_t getFiberId() const { return m_fiberId; }

    /* 返回时间戳 */
    uint64_t getTime() const { return m_time; }

    /* 返回线程名称 */
    const std::string& getThreadName() const { return m_threadName; }

    /* 返回日志内容 */
    std::string getContent() const { return m_ss.str(); }

    /* 返回日志器 */
    std::shared_ptr<Logger> getLogger() const { return m_logger; }

    /* 返回日志级别 */
    LogLevel::Level getLevel() const { return m_level; }

    /* 返回日志内容字符串流 */
    std::stringstream& getSS() { return m_ss; }

    /* 格式化写入日志内容 */
    void format(const char* fmt, ...);

    /* 格式化写入日志内容 */
    void format(const char* fmt, va_list al);

private:
    /* 文件名 */
    const char* m_file = nullptr;

    /* 行号 */
    uint32_t m_line = 0;

    /* 程序启动开始到现在的毫秒数 */
    uint32_t m_elapse = 0;

    /* 线程ID */
    uint32_5t m_threadId = 0;

    /* 协程ID */
    uint32_t m_fiberId = 0;

    /* 时间戳 */
    uint64_t m_time = 0;

    /* 线程名称 */
    std::string m_threadName;

    /* 日志内容流 */
    std::stringstream m_ss;

    /* 日志器 */
    std::shared_ptr<Logger> m_logger;

    /* 日志等级 */
    LogLevel::Level m_level;    
};

class LogFormatter {
public:
    typedef std::shared_ptr<Formatter> ptr;

    /*
     *  构造函数
     *  格式模板：
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
     LogFormatter(const std::string& pattern);

     /* 返回格式化日志文本 */
     std::string& format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
     std::ostream& format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

public:
    /* 日志内容项格式化 */
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;

        /* 析构函数 */
        virtual ~FormatItem() {}

        /* 格式化日志到流 */
        virtual void format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    /* 初始化，解析日志模板 */
    void init();

    /* 是否有错误 */
    bool isError() const { return m_error; }

    /* 返回日志模板 */
    const std::string& getPattern() const { return m_pattern; }
    
private:
    /* 日志格式模板 */
    std::string m_pattern;

    /* 日志格式解析后的格式 */
    std::vector<FormatItem::ptr> m_items;

    /* 是否有错误 */
    bool m_error = false;

};

class LogAppender {
public:
    typedef std::shared_ptr<Appender> ptr;

    /* Appender会有派生类，所以需要使用虚析构函数 */
    virtual ~LogAppender() {}

    /* 写日志 */
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    /* 更改日志格式器 */
    void setFormatter(LogFormatter::ptr val);

    /* 获取日志格式器 */
    LogFormatter::ptr getFormatter();

    /* 设置日志级别 */
    void setLevel(LogLevel::Level val) { m_level = val; }

    /* 获取日志级别 */
    LogLevel::Level getLevel() { return m_level; }
    
protected:
    /* 日志级别 */
    LogLevel::Level m_level = LogLevel::DEBUG;

    /* 是否有自己的日志格式器 */
    bool m_hasFormmter = false;

    /* 日志格式器 */
    LogFormatter::ptr m_formatter;

};

/*
    日志器，包括一个日志格式器，一个logger root,N个Appenders
    提供日志写入方法，根据格式，解析并写入到对应位置

    继承std::enable_shared_from_this<Logger>，
*/
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;

    /* 构造函数 */
    Logger(const std::string& name = "root");

    /* 写日志 */
    void log(LogLevel::Level level, LogEvent::ptr event);

    /* 写debug日志 */
    void debug(LogEvent::ptr event);

    /* 写info日志 */
    void info(LogEvent::ptr event);

    /* 写warning日志 */
    void warning(LogEvent::ptr event);

    /* 写error日志 */
    void error(LogEvent::ptr event);

    /* 写fatal日志 */
    void fatal(LogEvent::ptr event);

    /* 添加日志输出器 */
    void AddAppender(LogAppender::ptr appender);

    /* 删除一个日志输出器 */
    void DelAppender(LogAppender::ptr appender);

    /* 清空日志输出器集合 */
    void ClearAppenders();

    /* 获取日志名称 */
    std::string& getName() { return m_name; }

    /* 获取日志级别 */
    LogLevel::Level getLevel() { return m_level; }

    /* 设置日志级别 */
    void setLevel(LogLevel::Level val) { m_level = val; } 

    /* 设置日志格式器模板 */
    void setFormatter(const std::string& format);

    /* 获取日志格式器 */
    LogFormatter::ptr getFormatter() { return m_formatter; }
    

private:
    /* 支持一个loger输出到不同的目的地的日志输出器集合 */
    std::list<Appender::ptr> m_appenders; 

    /* 日志级别 */
    LogLevel::Level m_level;

    /* 日志名称 */
    std::string m_name; 

    /* 日志格式器 */
    LogFormatter::ptr m_formatter; 

    /*主日志器，如果当前日志未定义，使用主日志器输出*/
    Logger::ptr m_root;
};


/* Appender类的子类，输出到控制台 */
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    /* 打印日志 */
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    
};


/* Appender类的子类，输出到文件 */
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    /* 构造函数 */
    FileLogAppender(const std::string& filename);
    
    /* 打印日志 */
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    /* 重新打开日志文件 */
    bool reopen();
    
private:
    /* 文件路径 */
    std::string m_filename;
    
    /* 文件流 */
    std::ofstream m_filestream;

    /* 上次重新打开的时间 */
    uint64_t m_lasttime = 0;
};

}
#endif // !__LOG_H__
