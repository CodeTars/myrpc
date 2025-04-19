#include "logger.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

void Logger::log(const std::string& data, LogLevel level) {
    // 格式化时间
    char buffer[80];
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", this->getLocalTime());

    // 根据日志级别添加前缀
    std::string logEntry(buffer);
    switch (level) {
        case INFO:
            logEntry += " [INFO] ";
            break;
        case ERROR:
            logEntry += " [ERROR] ";
            break;
    }

    // 将日志信息放入队列
    logEntry += data;
    queue.Push(logEntry);
}

Logger::Logger() {
    std::thread write_log([this]() {
        for (;;) {
            // 格式化时间
            tm* local_time = this->getLocalTime();
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d-log.txt", local_time);
            // 打开日志文件
            std::ofstream logFile(buf, std::ios::app);
            if (!logFile.is_open()) {
                std::cerr << "Failed to open log file: " << buf << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string logEntry = this->queue.Pop();
            logFile << logEntry << std::endl;
            logFile.close();
        }
    });
    write_log.detach();
    std::cout << "Logger initialized." << std::endl;
}

tm* Logger::getLocalTime() {
    time_t now = time(nullptr);
    tm* local_time = localtime(&now);
    return local_time;
}