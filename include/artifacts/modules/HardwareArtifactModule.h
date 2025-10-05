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
 * @brief Hardware artifact module for creating honeypot hardware info
 * 
 * This module creates hardware-related artifacts via WMI that malware
 * may check to detect sandbox/honeypot environments.
 */
class HardwareArtifactModule : public IArtifactModule {
public:
    HardwareArtifactModule();
    ~HardwareArtifactModule() override;

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
    struct HardwareArtifact {
        std::string type;
        std::string value;
        bool created;
        
        HardwareArtifact(const std::string& t) 
            : type(t), created(false) {}
    };

    bool CreateArtifacts();
    bool RemoveArtifacts();
    void SetupVMwareBIOSArtifacts();
    void SetupVirtualBoxBIOSArtifacts();
    void SetupVirtualHardwareArtifacts();
    bool InitializeWMI();
    void CleanupWMI();
    void UpdateMetrics();

    // Configuration and state
    ArtifactConfig m_config;
    bool m_isActive;
    std::vector<HardwareArtifact> m_artifacts;
    Logger* m_logger;
    
    // WMI support
    bool m_wmiInitialized;
    
    // Performance tracking
    mutable PerformanceMetrics m_metrics;
    TimePoint m_lastMetricsUpdate;
    size_t m_operationCount;
    
    // Synchronization
    mutable std::mutex m_mutex;
};

} // namespace HoneyPottr
