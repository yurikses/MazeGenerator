#pragma once
#include <iostream>
#include <string>
#include <mutex>
class AppLogger {
    std::mutex mtx;
    AppLogger() {}
public:
    static AppLogger& instance();
    AppLogger(const AppLogger&) = delete; AppLogger& operator=(const AppLogger&) = delete;
    void log(const std::string& m);
};
