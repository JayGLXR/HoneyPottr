#include "logging/Logger.h"
#include "common/Constants.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace fs = std::filesystem;

namespace HoneyPottr {

Logger::Logger()
    : m_logLevel(LogLevel::Info)
    , m_maxFileSizeMB(10)
    , m_maxLogFiles(5)
    , m_enableEventLog(false)
    , m_enableConsole(false)
    , m_isInitialized(false)
    , m_eventLogHandle(nullptr)
    , m_shouldStop(false)
    , m_messagesLogged(0)
    , m_messagesDropped(0) {
}

Logger::~Logger() {
    Shutdown();
}

bool Logger::Initialize(const std::string& logFilePath,
                       LogLevel logLevel,
                       size_t maxFileSizeMB,
                       int maxLogFiles,
                       bool enableEventLog,
                       bool enableConsole) {
    if (m_isInitialized) {
        return true;
    }

    m_logFilePath = logFilePath;
    m_logLevel = logLevel;
    m_maxFileSizeMB = maxFileSizeMB;
    m_maxLogFiles = maxLogFiles;
    m_enableEventLog = enableEventLog;
    m_enableConsole = enableConsole;

    // Ensure log directory exists
    if (!EnsureDirectoryExists(m_logFilePath)) {
        std::cerr << "Failed to create log directory" << std::endl;
        return false;
    }

    // Open log file
    try {
        m_logFile.open(m_logFilePath, std::ios::app);
        if (!m_logFile.is_open()) {
            std::cerr << "Failed to open log file: " << m_logFilePath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception opening log file: " << e.what() << std::endl;
        return false;
    }

    // Register event log source if enabled
    if (m_enableEventLog) {
        RegisterEventLogSource();
    }

    // Start worker thread
    m_shouldStop = false;
    m_workerThread = std::thread(&Logger::LogWorker, this);

    m_isInitialized = true;
    Info("Logger initialized", "Logger");
    
    return true;
}

void Logger::Shutdown() {
    if (!m_isInitialized) {
        return;
    }

    Info("Logger shutting down", "Logger");

    // Stop worker thread
    m_shouldStop = true;
    m_queueCondition.notify_all();
    
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }

    // Close log file
    {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        if (m_logFile.is_open()) {
            m_logFile.flush();
            m_logFile.close();
        }
    }

    // Close event log
    if (m_eventLogHandle) {
        DeregisterEventSource(m_eventLogHandle);
        m_eventLogHandle = nullptr;
    }

    m_isInitialized = false;
}

void Logger::Log(LogLevel level, const std::string& message, const std::string& source) {
    if (!m_isInitialized || level < m_logLevel) {
        return;
    }

    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.source = source;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.threadId = GetCurrentThreadId();

    // Add to queue
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        if (m_logQueue.size() >= MAX_QUEUE_SIZE) {
            // Queue is full, drop message
            m_messagesDropped++;
            return;
        }
        
        m_logQueue.push(entry);
    }
    
    m_queueCondition.notify_one();
}

void Logger::Debug(const std::string& message, const std::string& source) {
    Log(LogLevel::Debug, message, source);
}

void Logger::Info(const std::string& message, const std::string& source) {
    Log(LogLevel::Info, message, source);
}

void Logger::Warning(const std::string& message, const std::string& source) {
    Log(LogLevel::Warning, message, source);
}

void Logger::Error(const std::string& message, const std::string& source) {
    Log(LogLevel::Error, message, source);
}

void Logger::Critical(const std::string& message, const std::string& source) {
    Log(LogLevel::Critical, message, source);
}

void Logger::SetLogLevel(LogLevel level) {
    m_logLevel = level;
}

LogLevel Logger::GetLogLevel() const {
    return m_logLevel;
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    if (m_logFile.is_open()) {
        m_logFile.flush();
    }
}

bool Logger::IsInitialized() const {
    return m_isInitialized;
}

void Logger::LogWorker() {
    while (!m_shouldStop) {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        
        m_queueCondition.wait_for(lock, std::chrono::milliseconds(100), [this]() {
            return !m_logQueue.empty() || m_shouldStop;
        });

        while (!m_logQueue.empty()) {
            LogEntry entry = m_logQueue.front();
            m_logQueue.pop();
            lock.unlock();

            // Write to all enabled outputs
            WriteToFile(entry);
            
            if (m_enableEventLog) {
                WriteToEventLog(entry);
            }
            
            if (m_enableConsole) {
                WriteToConsole(entry);
            }

            m_messagesLogged++;

            lock.lock();
        }
    }

    // Process any remaining messages
    std::lock_guard<std::mutex> lock(m_queueMutex);
    while (!m_logQueue.empty()) {
        LogEntry entry = m_logQueue.front();
        m_logQueue.pop();
        
        WriteToFile(entry);
        if (m_enableEventLog) {
            WriteToEventLog(entry);
        }
        if (m_enableConsole) {
            WriteToConsole(entry);
        }
        
        m_messagesLogged++;
    }
}

void Logger::WriteToFile(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    
    if (!m_logFile.is_open()) {
        return;
    }

    // Check if rotation is needed
    if (NeedsRotation()) {
        m_logFile.close();
        RotateLogFiles();
        m_logFile.open(m_logFilePath, std::ios::app);
    }

    m_logFile << FormatLogEntry(entry) << std::endl;
    m_logFile.flush();
}

void Logger::WriteToEventLog(const LogEntry& entry) {
    if (!m_eventLogHandle) {
        return;
    }

    WORD eventType = LogLevelToEventType(entry.level);
    LPCSTR messages[1] = { entry.message.c_str() };
    
    ReportEventA(m_eventLogHandle, eventType, 0, 0, nullptr, 1, 0, messages, nullptr);
}

void Logger::WriteToConsole(const LogEntry& entry) {
    std::string formattedEntry = FormatLogEntry(entry);
    
    // Use different output streams based on log level
    if (entry.level >= LogLevel::Error) {
        std::cerr << formattedEntry << std::endl;
    } else {
        std::cout << formattedEntry << std::endl;
    }
}

std::string Logger::FormatLogEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    
    // Format: [YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] [Thread:ID] [Source] Message
    oss << "[";
    oss << std::setfill('0') << std::setw(4) << (timeinfo.tm_year + 1900) << "-"
        << std::setw(2) << (timeinfo.tm_mon + 1) << "-"
        << std::setw(2) << timeinfo.tm_mday << " "
        << std::setw(2) << timeinfo.tm_hour << ":"
        << std::setw(2) << timeinfo.tm_min << ":"
        << std::setw(2) << timeinfo.tm_sec << "."
        << std::setw(3) << ms.count();
    oss << "] ";
    
    oss << "[" << LogLevelToString(entry.level) << "] ";
    oss << "[Thread:" << entry.threadId << "] ";
    
    if (!entry.source.empty()) {
        oss << "[" << entry.source << "] ";
    }
    
    oss << entry.message;
    
    return oss.str();
}

std::string Logger::LogLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        default: return "UNKN ";
    }
}

WORD Logger::LogLevelToEventType(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:
        case LogLevel::Info:
            return EVENTLOG_INFORMATION_TYPE;
        case LogLevel::Warning:
            return EVENTLOG_WARNING_TYPE;
        case LogLevel::Error:
        case LogLevel::Critical:
            return EVENTLOG_ERROR_TYPE;
        default:
            return EVENTLOG_INFORMATION_TYPE;
    }
}

bool Logger::NeedsRotation() const {
    return GetLogFileSize() >= (m_maxFileSizeMB * 1024 * 1024);
}

void Logger::RotateLogFiles() {
    try {
        fs::path logPath(m_logFilePath);
        std::string basePath = logPath.parent_path().string() + "/" + 
                              logPath.stem().string();
        std::string extension = logPath.extension().string();

        // Remove oldest log file if we have max files
        std::string oldestLog = basePath + "." + std::to_string(m_maxLogFiles - 1) + extension;
        if (fs::exists(oldestLog)) {
            fs::remove(oldestLog);
        }

        // Rotate existing log files
        for (int i = m_maxLogFiles - 2; i >= 0; --i) {
            std::string oldName = (i == 0) ? m_logFilePath : 
                                 basePath + "." + std::to_string(i) + extension;
            std::string newName = basePath + "." + std::to_string(i + 1) + extension;
            
            if (fs::exists(oldName)) {
                fs::rename(oldName, newName);
            }
        }
    } catch (const std::exception& e) {
        // Log rotation failure is non-fatal
        std::cerr << "Log rotation failed: " << e.what() << std::endl;
    }
}

size_t Logger::GetLogFileSize() const {
    try {
        if (fs::exists(m_logFilePath)) {
            return fs::file_size(m_logFilePath);
        }
    } catch (...) {
        // Ignore errors
    }
    return 0;
}

bool Logger::EnsureDirectoryExists(const std::string& filePath) {
    try {
        fs::path logPath(filePath);
        fs::path directory = logPath.parent_path();
        
        if (!directory.empty() && !fs::exists(directory)) {
            return fs::create_directories(directory);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create directory: " << e.what() << std::endl;
        return false;
    }
}

bool Logger::RegisterEventLogSource() {
    m_eventLogHandle = RegisterEventSourceA(nullptr, Constants::Logging::EVENT_LOG_SOURCE);
    return m_eventLogHandle != nullptr;
}

} // namespace HoneyPottr
