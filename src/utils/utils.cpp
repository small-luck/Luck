#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include "utils.h"

namespace Luck {

std::string g_threadName = "UNKNOW";

/* 获取线程ID */
pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}

/* 获取协程ID */
uint32_t GetFiberId()
{
    return 0;
}

/* 获取线程名称 */
std::string& GetThreadName()
{
    return g_threadName;
}

/* 获取当前时间，以20190905172700格式返回 */
std::string GetCurrentTime()
{
   char         tformat[64];
   time_t       timep;
   struct tm    p;

   time(&timep);
   gmtime_r(&timep, &p);

   snprintf(tformat, 64, "%04d%02d%02d%02d%02d%02d",
           (1900 + p.tm_year), (1 + p.tm_mon), p.tm_mday,
           p.tm_hour, p.tm_min, p.tm_sec);

   return tformat;
}

}
