#include "logging/Logger.h"
#include <iostream>

namespace HoneyPottr {
// Stub implementation for Logger
class Logger {
public:
    Logger() = default;
    ~Logger() = default;
    
    void Log(LogLevel /*level*/, const std::string& message) {
        std::cout << "Logger: " << message << std::endl;
    }
};
}
