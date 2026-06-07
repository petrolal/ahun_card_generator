#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

namespace ahun {

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    SUCCESS
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message) {
        switch (level) {
            case LogLevel::INFO:
                std::cout << "[INFO] " << message << std::endl;
                break;
            case LogLevel::WARNING:
                std::cout << "\x1b[33m[WARNING]\x1b[0m " << message << std::endl;
                break;
            case LogLevel::ERROR:
                std::cerr << "\x1b[31m[ERROR]\x1b[0m " << message << std::endl;
                break;
            case LogLevel::SUCCESS:
                std::cout << "\x1b[32m[SUCCESS]\x1b[0m " << message << std::endl;
                break;
        }
    }

    static void info(const std::string& message) { log(LogLevel::INFO, message); }
    static void warn(const std::string& message) { log(LogLevel::WARNING, message); }
    static void error(const std::string& message) { log(LogLevel::ERROR, message); }
    static void success(const std::string& message) { log(LogLevel::SUCCESS, message); }
};

} // namespace ahun

#endif // LOGGER_HPP
