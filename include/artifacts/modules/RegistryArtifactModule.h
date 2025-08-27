#pragma once

#include "artifacts/IArtifactModule.h"
#include <windows.h>
#include <vector>
#include <map>

namespace HoneyPottr {

/**
 * @brief Registry artifact module for creating honeypot registry entries
 * 
 * This module creates registry keys and values that are commonly checked
 * by malware to detect sandbox/honeypot environments.
 */
class RegistryArtifactModule : public IArtifactModule {
public:
    RegistryArtifactModule();
    ~RegistryArtifactModule() override;

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

protected:
    void LogMessage(LogLevel level, const std::string& message) const override;
    bool ShouldContinueOperation() const override;

private:
    /**
     * @brief Structure representing a registry key to create
     */
    struct RegistryKey {
        HKEY rootKey;
        std::string subKey;
        std::map<std::string, std::pair<DWORD, std::string>> values; // name -> (type, data)
        bool created;
        
        RegistryKey(HKEY root, const std::string& sub) 
            : rootKey(root), subKey(sub), created(false) {}
    };

    /**
     * @brief Create all configured registry artifacts
     * @return true if all artifacts were created successfully
     */
    bool CreateArtifacts();

    /**
     * @brief Remove all created registry artifacts
     * @return true if all artifacts were removed successfully
     */
    bool RemoveArtifacts();

    /**
     * @brief Create a single registry key with its values
     * @param key Registry key structure to create
     * @return true if creation succeeded
     */
    bool CreateRegistryKey(RegistryKey& key);

    /**
     * @brief Remove a single registry key
     * @param key Registry key structure to remove
     * @return true if removal succeeded
     */
    bool RemoveRegistryKey(const RegistryKey& key);

    /**
     * @brief Set up VMware-related registry artifacts
     */
    void SetupVMwareArtifacts();

    /**
     * @brief Set up VirtualBox-related registry artifacts
     */
    void SetupVirtualBoxArtifacts();

    /**
     * @brief Set up Sandboxie-related registry artifacts
     */
    void SetupSandboxieArtifacts();

    /**
     * @brief Set up analysis tool-related registry artifacts
     */
    void SetupAnalysisToolArtifacts();

    /**
     * @brief Convert string to registry data based on type
     * @param type Registry value type (REG_SZ, REG_DWORD, etc.)
     * @param value String representation of the value
     * @return Pair of data pointer and size
     */
    std::pair<std::vector<BYTE>, DWORD> ConvertToRegistryData(DWORD type, const std::string& value) const;

    /**
     * @brief Parse HKEY string to HKEY handle
     * @param hkeyString String representation (e.g., "HKEY_LOCAL_MACHINE")
     * @return HKEY handle or nullptr if invalid
     */
    HKEY ParseHKeyString(const std::string& hkeyString) const;

    /**
     * @brief Check if a registry key already exists
     * @param rootKey Root registry key
     * @param subKey Subkey path
     * @return true if key exists
     */
    bool RegistryKeyExists(HKEY rootKey, const std::string& subKey) const;

    /**
     * @brief Update performance metrics
     */
    void UpdateMetrics();

    // Configuration and state
    ArtifactConfig m_config;
    bool m_isActive;
    std::vector<RegistryKey> m_registryKeys;
    
    // Performance tracking
    mutable PerformanceMetrics m_metrics;
    TimePoint m_lastMetricsUpdate;
    size_t m_operationCount;
    
    // Synchronization
    mutable std::mutex m_mutex;
};

} // namespace HoneyPottr
