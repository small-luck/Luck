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



}
