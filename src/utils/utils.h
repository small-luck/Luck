/*
    @file   utils.h
    @brief  工具模块封装  
    @author perry.liu
    @date   2019-08-02
*/
#ifndef __LUCK_UTILS_H__
#define __LUCK_UTILS_H__

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


}



#endif /*__LUCK_UTILS_H__*/