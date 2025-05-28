#pragma once

#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

class Logger {
public:
    /// Globally enable or disable logging
    static bool enabled;

    /// Log a message with timestamp
    static void log(const std::string& msg) {
        if (!enabled) return;
        std::lock_guard<std::mutex> guard(mutex_);

        // Get current time
        auto now    = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto ms     = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S")
            << '.' << std::setw(3) << std::setfill('0') << ms.count()
            << " [BumpAllocator] " << msg;

        std::cout << oss.str() << "\n";
    }

private:
    static std::mutex mutex_;
};
