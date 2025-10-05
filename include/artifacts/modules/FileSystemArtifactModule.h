#pragma once

#include "artifacts/IArtifactModule.h"
#include <windows.h>
#include <vector>
#include <string>
#include <mutex>

namespace HoneyPottr {

// Forward declaration
class Logger;

/**
 * @brief File system artifact module for creating honeypot files
 * 
 * This module creates files and directories that are commonly checked
 * by malware to detect sandbox/honeypot environments.
 */
class FileSystemArtifactModule : public IArtifactModule {
public:
    FileSystemArtifactModule();
    ~FileSystemArtifactModule() override;

    // IArtifactModule implementation
    bool Initialize(const ArtifactConfig& config) override;
    bool Start() override;
    bool Stop() override;
    bool Refresh() override;
    ArtifactType GetType() const override;
    bool IsActive() const override;
    const ArtifactConfig& GetConfig() const override;
    bool UpdateConfig(const ArtifactConfig& config) override;
    StringVector GetActiveArtifacts() const override;
    PerformanceMetrics GetMetrics() const override;
    bool ValidateEnvironment() const override;
    std::string GetModuleName() const override;
    std::string GetVersion() const override;
    void SetLogger(Logger* logger) override;

protected:
    void LogMessage(LogLevel level, const std::string& message) const override;
    bool ShouldContinueOperation() const override;

private:
    struct FileArtifact {
        std::string path;
        std::string content;
        DWORD attributes;
        bool created;
        
        FileArtifact(const std::string& p) 
            : path(p), attributes(FILE_ATTRIBUTE_NORMAL), created(false) {}
    };

    bool CreateArtifacts();
    bool RemoveArtifacts();
    bool CreateFileArtifact(FileArtifact& artifact);
    bool RemoveFileArtifact(const FileArtifact& artifact);
    void SetupVMwareArtifacts();
    void SetupVirtualBoxArtifacts();
    void SetupAnalysisToolArtifacts();
    void SetupDecoyArtifacts();
    bool FileExists(const std::string& path) const;
    bool EnsureDirectoryExists(const std::string& path);
    void UpdateMetrics();

    // Configuration and state
    ArtifactConfig m_config;
    bool m_isActive;
    std::vector<FileArtifact> m_artifacts;
    Logger* m_logger;
    
    // Performance tracking
    mutable PerformanceMetrics m_metrics;
    TimePoint m_lastMetricsUpdate;
    size_t m_operationCount;
    
    // Synchronization
    mutable std::mutex m_mutex;
};

} // namespace HoneyPottr
