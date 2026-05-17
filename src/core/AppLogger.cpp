#include "AppLogger.h"
AppLogger& AppLogger::instance() { 
    static AppLogger i; 
    return i; 
}

void AppLogger::log(const std::string& m) {
    std::lock_guard<std::mutex> l(mtx); 
    std::cout << "[LOG] " << m << "\n"; 
}
