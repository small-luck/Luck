/*
    @file   utils.h
    @brief  工具模块封装  
    @author perry.liu
    @date   2019-08-02
*/
#ifndef __LUCK_UTIL_H__
#define __LUCK_UTIL_H__

#include <stdint.h>
#include <string>
#include <sys/types.h>

namespace Luck {

/* 获取线程ID */
pid_t GetThreadId();

/* 获取协程ID */
uint32_t GetFiberId();

/* 获取线程名称 */
std::string& GetThreadName();

/* 获取当前时间，以20190905172700格式返回 */
std::string GetCurrentTime();

}

#endif /*__LUCK_UTILS_H__*/
