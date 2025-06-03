#pragma once
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>

namespace util {

class Logger {
public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& entry) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!ofs_.is_open()) {
            ofs_.open("logs/operations.log", std::ios::out | std::ios::app);
            if (!ofs_.is_open()) {
                std::cerr << "✗ Could not open log file." << std::endl;
                return;
            }
        }
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char timebuf[20];
        std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

        ofs_ << "[" << timebuf << "] " << entry << std::endl;
    }

private:
    Logger() = default;
    ~Logger() {
        if (ofs_.is_open()) ofs_.close();
    }
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream ofs_;
    std::mutex mtx_;
};

} // namespace util
