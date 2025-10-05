#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include "common/Types.h"
#include "artifacts/IArtifactModule.h"

namespace HoneyPottr {

// Forward declarations
class ConfigurationManager;
class Logger;

/**
 * @brief Main engine for managing all artifact generation modules
 * 
 * This class orchestrates multiple artifact modules (registry, filesystem, etc.)
 * and ensures they operate within resource constraints.
 */
class ArtifactEngine {
public:
    ArtifactEngine();
    ~ArtifactEngine();

    // Non-copyable, non-movable
    ArtifactEngine(const ArtifactEngine&) = delete;
    ArtifactEngine& operator=(const ArtifactEngine&) = delete;
    ArtifactEngine(ArtifactEngine&&) = delete;
    ArtifactEngine& operator=(ArtifactEngine&&) = delete;

    /**
     * @brief Initialize the artifact engine
     * @param configManager Configuration manager instance
     * @param logger Logger instance
     * @return true if initialization succeeded
     */
    bool Initialize(ConfigurationManager* configManager, Logger* logger);

    /**
     * @brief Start all enabled artifact modules
     * @return true if start succeeded
     */
    bool Start();

    /**
     * @brief Stop all artifact modules
     */
    void Stop();

    /**
     * @brief Refresh all active artifacts
     * @return true if refresh succeeded
     */
    bool Refresh();

    /**
     * @brief Check if the engine is running
     * @return true if running
     */
    bool IsRunning() const;

    /**
     * @brief Get list of active modules
     * @return Vector of artifact types
     */
    std::vector<ArtifactType> GetActiveModules() const;

    /**
     * @brief Get a specific module
     * @param type Artifact type
     * @return Pointer to module or nullptr
     */
    IArtifactModule* GetModule(ArtifactType type) const;

    /**
     * @brief Get combined metrics from all modules
     * @return Performance metrics
     */
    PerformanceMetrics GetMetrics() const;

private:
    /**
     * @brief Register all built-in artifact modules
     */
    void RegisterModules();

    /**
     * @brief Initialize and configure modules based on configuration
     * @return true if successful
     */
    bool InitializeModules();

    /**
     * @brief Cleanup all modules
     */
    void CleanupModules();

    /**
     * @brief Refresh loop thread function
     */
    void RefreshLoop();

    // Components
    ConfigurationManager* m_configManager;
    Logger* m_logger;

    // Modules
    std::map<ArtifactType, std::unique_ptr<IArtifactModule>> m_modules;
    
    // State
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_shouldStop;
    std::thread m_refreshThread;
    
    // Synchronization
    mutable std::mutex m_modulesMutex;
};

} // namespace HoneyPottr
