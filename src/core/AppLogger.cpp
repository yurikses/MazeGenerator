#include "core/AppLogger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

AppLogger& AppLogger::getInstance() {
    static AppLogger instance;
    return instance;
}

void AppLogger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!file_.is_open()) {
        file_.open("maze_app.log", std::ios::app);
    }
    if (!file_.is_open()) {
        return;
    }

    file_ << '[' << timestamp() << "] [" << logLevelToString(level) << "] " << message << '\n';
    file_.flush();
}

const char* AppLogger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
    }
    return "UNKNOWN";
}

std::string AppLogger::timestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}
