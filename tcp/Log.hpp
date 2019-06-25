/***********************************
 * 文件名称: Log.hpp 
 * 文件描述: 日志
 * 编译环境: Linux
 * 作者相关: 心文花雨
***********************************/
#ifndef __LOG_HPP__
#define  __LOG_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>

#define INFO 0
#define DEBUG 1
#define WARING 2
#define ERROR 3

std::vector<std::string> g_error_level=
{
    "INFO",
    "DEBUG",
    "WARING",
    "ERROR",
};

class TimeUtil
{
public:
    // 获取当前时间戳
    static int64_t TimeStamp()
    {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        return tv.tv_sec;
    }

    static int64_t TimeStampMS()
    {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
};

void Log(int level, std::string message, std::string file, int line)
{
    std::cout << "[" << TimeUtil::TimeStamp()  << "]" << "[" << g_error_level[level] << "][" << file << line << "] " << message << std::endl; 
}

#define LOG(level, message) Log(level, message, __FILE__, __LINE__)

#endif
