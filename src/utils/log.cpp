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

/* 构造函数 */
LogEvent::LogEvent(std :: shared_ptr < Logger > logger, LogLevel :: Level level, 
                   const char * file, uint32_t line, uint32_t elapse, 
                   uint32_t thread_id, uint32_t fiber_id, uint64_t time,
                   std :: string & thread_name)
   :m_file(file)
   ,m_line(line)
   ,m_elapse(elapse)
   ,m_threadId(thread_id)
   ,m_fiberId(fiber_id)
   ,m_time(time)
   ,m_threadName(thread_name)
   ,m_logger(logger)
   ,m_level(level) {
}

/* 格式化写入日志内容 */
void LogEvent::format(const char * fmt, ...)
{

}

/* 格式化写入日志内容 */
void LogEvent::format(const char * fmt, va_list al)
{

}

/* 更改日志格式器 */
void LogAppender::setFormatter(LogFormatter::ptr val)
{
    m_formatter = val;
    if (m_formatter) {
        m_hasFormmter = true;
    } else {
        m_hasFormmter = false;
    }
}

/* 获取日志格式器 */
LogFormatter::ptr LogAppender::getFormatter()
{
    return m_formatter;
}

/* 构造函数 */
FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename) 
{
}
    
/* 打印日志 */
void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
{
    if (level >= m_level) {
        uint64_t now = event->getTime();
        if (now >= (m_lasttime + 3)) {
            reopen();
            m_lasttime = now;
        }

        if (!m_formatter.format(m_filestream, logger, level, event)) {
            std::cout << "error" << std::endl;
        }
    }
}

/* 重新打开日志文件 */
bool FileLogAppender::reopen()
{
    if (m_filestream) {
        m_filestream.close();
    }

    m_filestream.open(m_filename.c_str(), std::ios::app);
}

/* 返回消息类 */
class MessageFormatItem : public LogFormatter::FormatItem {

};

/* 返回日志级别类 */
class LevelFormatItem : public LogFormatter::FormatItem {

};

/* 返回从程序开始到现在的耗时类 */
class ElapseFormatItem : public LogFormatter::FormatItem {

};

/* 返回日志名称类 */
class NameFormatItem : public LogFormatter::FormatItem {

};

/* 返回线程ID类 */
class ThreadIdFormatItem : public LogFormatter::FormatItem {

};

/* 返回协程ID类 */
class FiberIdFormatItem : public LogFormatter::FormatItem {

};

/* 返回线程名称类 */
class ThreadNameFormatItem : public LogFormatter::FormatItem {

};

/* 返回时间戳类 */
class DateTimeFormatItem : public LogFormatter::FormatItem {

};

/* 返回文件名类 */
class FileNameFormatItem : public LogFormatter::FormatItem {

};

/* 返回行号类 */
class LineFormatItem : public LogFormatter::FormatItem {

};


/* 返回换行类 */
class NewLineFormatItem : public LogFormatter::FormatItem {

};

/* 返回字符串类 */
class StringFormatItem : public LogFormatter::FormatItem {

};

/* 返回Tab空格类 */
class TabFormatItem : public LogFormatter::FormatItem {

};






LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern)
{
    init();
}

/* 返回格式化日志文本 */
std::string& LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{

}

/* 返回格式化日志文本 */
std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{

}

/* 初始化，解析日志模板 

   类型：
     1. %xxx
     2. %xxx{xxx}
     3. %%

     %d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
 */
void LogFormatter::init()
{
    //str, format, type 
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    size_t i;

    for (i = 0; i < m_pattern.size(); i++) {
        if (m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;    
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' & m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i -1);
                break;
            }
            if (fmt_status == 0) {
                if (m_pattern[i] == '{') {
                    str = m_pattern.substr(i + 1, n - i -1);
                    fmt_status = 1;
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    continue;
                }
            }

            ++n;
            if (n == m_pattern.size()) {
                if (str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if (fmt_status == 0) {
            if (!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if (fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;  
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
    }
    
}


/* 构造函数 */
Logger::Logger(const std::string& name = "root") : m_name(name), m_level(LogLevel::DEBUG)
{
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

/* 写日志 */
void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
    /* 只有满足level级别，才能输出 */
    if (level >= m_level) {
        auto self = shared_from_this();
        if (!m_appenders.empty()) {
            for (auto& i : m_appenders) {
                i->log(self, level, event);
            }
        } else if (m_root) {
            m_root->log(level, event);
        }
    }
}

/* 写debug日志 */
void Logger::debug(LogEvent::ptr event)
{
    log(LogLevel::DEBUG, event);
}

/* 写info日志 */
void Logger::info(LogEvent::ptr event)
{
    log(LogLevel::INFO, event);
}

/* 写warning日志 */
void Logger::warning(LogEvent::ptr event)
{
    log(LogLevel::WARNING, event);
}

/* 写error日志 */
void Logger::error(LogEvent::ptr event)
{
    log(LogLevel::ERROR, event);
}

/* 写fatal日志 */
void Logger::fatal(LogEvent::ptr event)
{
    log(LogLevel::FATAL, event);
}

/* 添加日志输出器 */
void Logger::AddAppender(LogAppender::ptr appender)
{
    m_appenders.push_back(appender);
}

/* 删除一个日志输出器 */
void Logger::DelAppender(LogAppender::ptr appender)
{
    for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

/* 清空日志输出器集合 */
void Logger::ClearAppenders()
{
    m_appenders.clear();
}

/* 设置日志格式器模板 */
void Logger::setFormatter(const std::string& format)
{
    LogFormatter::ptr new_val(new LogFormatter(format));
    m_formatter = new_val;
}





}
