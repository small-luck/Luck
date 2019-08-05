#include "log.h"
#include <iostream>
#include <functional>
#include <string.h>
#include <unistd.h>

namespace Luck {

#define MAX_LOGFILE_SIZE    (4*1024)
#define MAX_LOGFILE_NUM     (3)

const char* LogLevel::to_string(LogLevel::Level level) 
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

LogLevel::Level LogLevel::from_string(const std::string& str)
{
    if (str.empty()) 
        return LogLevel::Level::UNKNOW;

    if (str.compare("DEBUG") == 0) {
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
LogEvent::LogEvent(std::shared_ptr < Logger > logger, LogLevel::Level level, 
                   const char * file, uint32_t line, uint32_t elapse, 
                   uint32_t thread_id, uint32_t fiber_id, uint64_t time,
                   std::string & thread_name)
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
    va_list list;
    char* buff = nullptr;
    int len;

    va_start(list, fmt);
    len = vasprintf(&buff, fmt, list);
    if (len != -1) {
        m_ss << std::string(buff, len);
        free(buff);
    }
    va_end(list);
}

/* 构造函数 */
LogEventWrap::LogEventWrap(LogEvent::ptr event) : m_event(event) {

}

/* 析构函数,利用对象离开作用域自动析构，自动执行log */
LogEventWrap::~LogEventWrap() 
{
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrap::getSS()
{
    return m_event->getSS();
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
void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level) {
        uint64_t now = event->getTime();
        if (now >= (m_lasttime + 3)) {
            reopen();
            m_lasttime = now;
        }

        if (!m_hasFormmter) {
            setFormatter(logger->getFormatter());
            m_hasFormmter = true;
        }

        if (!m_formatter->format(m_filestream, logger, level, event)) {
            std::cout << "error" << std::endl;
        }
        
        m_filestream.seekp(0, m_filestream.end);
        m_openfilesize = m_filestream.tellp();
        
        if (m_openfilesize < MAX_LOGFILE_SIZE) {
            return;            
        }        

        std::string new_path;
        new_path = m_filename + "-" + m_firstopentime;
        FileLogInfo::ptr info(new FileLogInfo(new_path, m_firstopentime));

        if (rename(m_filename.c_str(), new_path.c_str()) < 0) {
            std::cout << "rename file failed, file = " << m_filename << std::endl;
            return;
        }

        m_isfirstopend = true;
        reopen();
        
        addFileToList(info);     
    }
}

/* 重新打开日志文件 */
bool FileLogAppender::reopen()
{    
    if (m_isfirstopend) {
        m_firstopentime = GetCurrentTime();
        m_isfirstopend = false;
        std::cout << "m_firstopentime = " << m_firstopentime << std::endl;
        
    }
    if (m_filestream) {
        m_filestream.close();
    }

    m_filestream.open(m_filename.c_str(), std::ios::app);    

    return true;
}

/* 将日志文件放入链表中 */
void FileLogAppender::addFileToList(FileLogInfo::ptr info)
{
    std::cout << "size = " << m_filelist.size() << std::endl;
    if (m_filelist.size() >= MAX_LOGFILE_NUM - 1) { 
        std::string filename = m_filelist.back()->getFilename();
        std::cout << "filename = " << filename << std::endl;
        if (unlink(filename.c_str()) == 0) {
            m_filelist.pop_back();
        }              
    }
    
    for (auto it = m_filelist.begin(); it != m_filelist.end(); it++) {
        if (info->getCreateTime().compare((*it)->getCreateTime()) > 0) {
            m_filelist.insert(it, info);
            return;
        }
    }

    m_filelist.push_back(info);    
}

/* 打印日志 */
void StdoutLogAppender::log(std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level) {
        if (!m_hasFormmter) {
            setFormatter(logger->getFormatter());
            m_hasFormmter = true;
        }
        m_formatter->format(std::cout, logger, level, event);
    }
}

/* 返回消息类 */
class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getContent();
    }
};

/* 返回日志级别类 */
class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "" ) {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << LogLevel::to_string(level);
    }
};

/* 返回从程序开始到现在的耗时类 */
class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getElapse();
    }
};

/* 返回日志名称类 */
class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getLogger()->getName();
    }
};

/* 返回线程ID类 */
class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getThreadId();
    }
};

/* 返回协程ID类 */
class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getFiberId();
    }
};

/* 返回线程名称类 */
class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getThreadName();
    }
};

/* 返回时间戳类 */
class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format) : m_dateformat(format) {
        if (m_dateformat.empty()) {
            m_dateformat = "%Y-%m-%d %H-%M:%S";
        }
    }

    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_dateformat.c_str(), &tm);
        os << buf;
    }

private:
    std::string m_dateformat;
};

/* 返回文件名类 */
class FileNameFormatItem : public LogFormatter::FormatItem {
public:
    FileNameFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getFile();
    }
};

/* 返回行号类 */
class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << event->getLine();
    }
};


/* 返回换行类 */
class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << std::endl;
    }
};

/* 返回字符串类 */
class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str) : m_str(str) {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << m_str;
    }
private:
    std::string m_str;
};

/* 返回Tab空格类 */
class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream & os, std::shared_ptr < Logger > logger, LogLevel::Level level, LogEvent::ptr event) {
        os << "\t";
    }
};


/* 构造函数 */
Logger::Logger(const std::string& name) 
    : m_name(name)
    , m_level(LogLevel::DEBUG) {
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
        } else {
            std::cout << "no appenders to log" << std::endl;
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


LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern)
{
    init();
}

/* 返回格式化日志文本 */
std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for(auto& i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

/* 返回格式化日志文本 */
std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    for (auto& i : m_items) {
        i->format(ofs, logger, level, event);
    }

    return ofs;
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

    for (i = 0; i < m_pattern.size(); ++i){
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
        size_t fmt_begin = i;

        std::string str;    
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - fmt_begin - 1);
                break;
            }
            if (fmt_status == 0) {
                if (m_pattern[n] == '{') {
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
                    break;
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
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if (fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;  
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
   
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) {return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),       //m:消息
        XX(p, LevelFormatItem),         //p:日志级别
        XX(r, ElapseFormatItem),        //r:累计毫秒数
        XX(c, NameFormatItem),          //c:日志名称
        XX(t, ThreadIdFormatItem),      //t:线程ID
        XX(n, NewLineFormatItem),       //n:换行
        XX(d, DateTimeFormatItem),      //d:时间
        XX(f, FileNameFormatItem),      //f:文件名
        XX(l, LineFormatItem),          //l:行号
        XX(T, TabFormatItem),           //T:Tab
        XX(F, FiberIdFormatItem),       //F:协程id
        XX(N, ThreadNameFormatItem),    //N:线程名称
#undef XX 
    };   

    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format % " + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
}



}


