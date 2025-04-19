#pragma once
#include <string>
#include "lockqueue.h"

enum LogLevel { INFO, ERROR };

class Logger {
public:
    // 单例模式
    static Logger& GetInstance();
    // 记录日志
    void log(const std::string& data, LogLevel level = INFO);

private:
    LockQueue<std::string> queue;
    tm* getLocalTime();

    // 私有的构造函数，禁止外部实例化
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

template <typename... Args>
void logInfo(const char* fmt, Args... args) {
    char buf[1024] = {0};
    snprintf(buf, sizeof(buf), fmt, args...);
    Logger::GetInstance().log(buf, INFO);
}

template <typename... Args>
void logError(const char* fmt, Args... args) {
    char buf[1024] = {0};
    snprintf(buf, sizeof(buf), fmt, args...);
    Logger::GetInstance().log(buf, ERROR);
}