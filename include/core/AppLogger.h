#pragma once

#include <fstream>
#include <mutex>
#include <string>

enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

class AppLogger {
public:
    static AppLogger& getInstance();

    void log(LogLevel level, const std::string& message);

    AppLogger(const AppLogger&) = delete;
    AppLogger& operator=(const AppLogger&) = delete;

private:
    AppLogger() = default;

    static const char* logLevelToString(LogLevel level);
    static std::string timestamp();

    std::ofstream file_;
    std::mutex mutex_;
};
