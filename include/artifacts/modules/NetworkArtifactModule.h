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
 * @brief Network artifact module for creating honeypot network artifacts
 * 
 * This module creates network-related artifacts that malware may check
 * to detect sandbox/honeypot environments.
 */
class NetworkArtifactModule : public IArtifactModule {
public:
    NetworkArtifactModule();
    ~NetworkArtifactModule() override;

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
    struct NetworkArtifact {
        std::string adapterName;
        std::string macAddress;
        bool created;
        
        NetworkArtifact(const std::string& n) 
            : adapterName(n), created(false) {}
    };

    bool CreateArtifacts();
    bool RemoveArtifacts();
    void SetupVMwareArtifacts();
    void SetupVirtualBoxArtifacts();
    StringVector GetNetworkAdapters() const;
    void UpdateMetrics();

    // Configuration and state
    ArtifactConfig m_config;
    bool m_isActive;
    std::vector<NetworkArtifact> m_artifacts;
    Logger* m_logger;
    
    // Performance tracking
    mutable PerformanceMetrics m_metrics;
    TimePoint m_lastMetricsUpdate;
    size_t m_operationCount;
    
    // Synchronization
    mutable std::mutex m_mutex;
};

} // namespace HoneyPottr
