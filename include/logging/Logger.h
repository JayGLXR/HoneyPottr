#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <windows.h>
#include "common/Types.h"

namespace HoneyPottr {

/**
 * @brief Thread-safe logging system with file and event log support
 * 
 * This logger supports writing to files, Windows Event Log, and console
 * with automatic log rotation and structured logging capabilities.
 */
class Logger {
public:
    Logger();
    ~Logger();

    // Non-copyable, non-movable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * @brief Initialize the logger
     * @param logFilePath Path to the log file
     * @param logLevel Minimum log level to record
     * @param maxFileSizeMB Maximum log file size before rotation
     * @param maxLogFiles Maximum number of rotated log files to keep
     * @param enableEventLog Enable Windows Event Log
     * @param enableConsole Enable console output
     * @return true if initialization succeeded
     */
    bool Initialize(const std::string& logFilePath,
                   LogLevel logLevel = LogLevel::Info,
                   size_t maxFileSizeMB = 10,
                   int maxLogFiles = 5,
                   bool enableEventLog = true,
                   bool enableConsole = false);

    /**
     * @brief Shutdown the logger and flush all pending messages
     */
    void Shutdown();

    /**
     * @brief Log a message
     * @param level Log level
     * @param message Message to log
     * @param source Source component (optional)
     */
    void Log(LogLevel level, const std::string& message, const std::string& source = "");

    /**
     * @brief Log a debug message
     * @param message Message to log
     * @param source Source component (optional)
     */
    void Debug(const std::string& message, const std::string& source = "");

    /**
     * @brief Log an info message
     * @param message Message to log
     * @param source Source component (optional)
     */
    void Info(const std::string& message, const std::string& source = "");

    /**
     * @brief Log a warning message
     * @param message Message to log
     * @param source Source component (optional)
     */
    void Warning(const std::string& message, const std::string& source = "");

    /**
     * @brief Log an error message
     * @param message Message to log
     * @param source Source component (optional)
     */
    void Error(const std::string& message, const std::string& source = "");

    /**
     * @brief Log a critical message
     * @param message Message to log
     * @param source Source component (optional)
     */
    void Critical(const std::string& message, const std::string& source = "");

    /**
     * @brief Set the minimum log level
     * @param level New minimum log level
     */
    void SetLogLevel(LogLevel level);

    /**
     * @brief Get the current log level
     * @return Current log level
     */
    LogLevel GetLogLevel() const;

    /**
     * @brief Flush all pending log messages
     */
    void Flush();

    /**
     * @brief Check if the logger is initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

private:
    /**
     * @brief Structure representing a log entry
     */
    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string source;
        TimePoint timestamp;
        DWORD threadId;
    };

    /**
     * @brief Worker thread function for asynchronous logging
     */
    void LogWorker();

    /**
     * @brief Write a log entry to file
     * @param entry Log entry to write
     */
    void WriteToFile(const LogEntry& entry);

    /**
     * @brief Write a log entry to Windows Event Log
     * @param entry Log entry to write
     */
    void WriteToEventLog(const LogEntry& entry);

    /**
     * @brief Write a log entry to console
     * @param entry Log entry to write
     */
    void WriteToConsole(const LogEntry& entry);

    /**
     * @brief Format a log entry as a string
     * @param entry Log entry to format
     * @return Formatted log string
     */
    std::string FormatLogEntry(const LogEntry& entry) const;

    /**
     * @brief Convert log level to string
     * @param level Log level
     * @return String representation
     */
    std::string LogLevelToString(LogLevel level) const;

    /**
     * @brief Convert log level to Windows Event Log type
     * @param level Log level
     * @return Event log type
     */
    WORD LogLevelToEventType(LogLevel level) const;

    /**
     * @brief Check if log file rotation is needed
     * @return true if rotation is needed
     */
    bool NeedsRotation() const;

    /**
     * @brief Rotate log files
     */
    void RotateLogFiles();

    /**
     * @brief Get current log file size in bytes
     * @return File size
     */
    size_t GetLogFileSize() const;

    /**
     * @brief Ensure log directory exists
     * @param filePath Path to log file
     * @return true if directory exists or was created
     */
    bool EnsureDirectoryExists(const std::string& filePath);

    /**
     * @brief Register Windows Event Log source
     * @return true if registration succeeded
     */
    bool RegisterEventLogSource();

    // Configuration
    std::string m_logFilePath;
    LogLevel m_logLevel;
    size_t m_maxFileSizeMB;
    int m_maxLogFiles;
    bool m_enableEventLog;
    bool m_enableConsole;
    bool m_isInitialized;

    // File handling
    std::ofstream m_logFile;
    mutable std::mutex m_fileMutex;

    // Event log handling
    HANDLE m_eventLogHandle;

    // Asynchronous logging
    std::queue<LogEntry> m_logQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    std::thread m_workerThread;
    std::atomic<bool> m_shouldStop;

    // Statistics
    std::atomic<size_t> m_messagesLogged;
    std::atomic<size_t> m_messagesDropped;
    static constexpr size_t MAX_QUEUE_SIZE = 1000;
};

} // namespace HoneyPottr
