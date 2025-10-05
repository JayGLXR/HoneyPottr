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
 * @brief Process artifact module for simulating honeypot processes
 * 
 * This module creates process artifacts that malware may check for
 * to detect sandbox/honeypot environments.
 */
class ProcessArtifactModule : public IArtifactModule {
public:
    ProcessArtifactModule();
    ~ProcessArtifactModule() override;

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
    struct ProcessArtifact {
        std::string name;
        std::string imagePath;
        HANDLE handle;
        DWORD processId;
        bool created;
        
        ProcessArtifact(const std::string& n) 
            : name(n), handle(nullptr), processId(0), created(false) {}
    };

    bool CreateArtifacts();
    bool RemoveArtifacts();
    bool CreateProcessArtifact(ProcessArtifact& artifact);
    bool RemoveProcessArtifact(const ProcessArtifact& artifact);
    void SetupVMwareArtifacts();
    void SetupVirtualBoxArtifacts();
    void SetupAnalysisToolArtifacts();
    bool IsProcessRunning(const std::string& name) const;
    void UpdateMetrics();

    // Configuration and state
    ArtifactConfig m_config;
    bool m_isActive;
    std::vector<ProcessArtifact> m_artifacts;
    Logger* m_logger;
    
    // Performance tracking
    mutable PerformanceMetrics m_metrics;
    TimePoint m_lastMetricsUpdate;
    size_t m_operationCount;
    
    // Synchronization
    mutable std::mutex m_mutex;
};

} // namespace HoneyPottr
