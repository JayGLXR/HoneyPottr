#pragma once

#include "common/Types.h"
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>

namespace HoneyPottr {

// Forward declarations
class ConfigurationManager;
class ArtifactEngine;
class ResourceMonitor;
class Logger;

/**
 * @brief Main service controller that orchestrates all subsystems
 * 
 * This class is responsible for managing the lifecycle of all HoneyPottr
 * subsystems and coordinating their interactions.
 */
class ServiceController {
public:
    ServiceController();
    ~ServiceController();

    // Non-copyable, non-movable
    ServiceController(const ServiceController&) = delete;
    ServiceController& operator=(const ServiceController&) = delete;
    ServiceController(ServiceController&&) = delete;
    ServiceController& operator=(ServiceController&&) = delete;

    /**
     * @brief Initialize the service controller and all subsystems
     * @param configPath Path to the configuration file
     * @return true if initialization succeeded, false otherwise
     */
    bool Initialize(const std::string& configPath = "");

    /**
     * @brief Start the service and all subsystems
     * @return true if start succeeded, false otherwise
     */
    bool Start();

    /**
     * @brief Stop the service and all subsystems
     * @return true if stop succeeded, false otherwise
     */
    bool Stop();

    /**
     * @brief Pause the service (suspend artifact generation)
     * @return true if pause succeeded, false otherwise
     */
    bool Pause();

    /**
     * @brief Resume the service from paused state
     * @return true if resume succeeded, false otherwise
     */
    bool Resume();

    /**
     * @brief Get the current service state
     * @return Current service state
     */
    ServiceState GetState() const;

    /**
     * @brief Check if the service is running
     * @return true if running, false otherwise
     */
    bool IsRunning() const;

    /**
     * @brief Reload configuration without restarting the service
     * @return true if reload succeeded, false otherwise
     */
    bool ReloadConfiguration();

    /**
     * @brief Get current performance metrics
     * @return Performance metrics structure
     */
    PerformanceMetrics GetMetrics() const;

    /**
     * @brief Set callback for service state changes
     * @param callback Callback function to call on state changes
     */
    void SetStateCallback(ServiceStateCallback callback);

    /**
     * @brief Set callback for error notifications
     * @param callback Callback function to call on errors
     */
    void SetErrorCallback(ErrorCallback callback);

    /**
     * @brief Set callback for performance metrics updates
     * @param callback Callback function to call with metrics
     */
    void SetMetricsCallback(MetricsCallback callback);

    /**
     * @brief Get the configuration manager instance
     * @return Pointer to configuration manager
     */
    ConfigurationManager* GetConfigurationManager() const;

    /**
     * @brief Get the artifact engine instance
     * @return Pointer to artifact engine
     */
    ArtifactEngine* GetArtifactEngine() const;

    /**
     * @brief Get the resource monitor instance
     * @return Pointer to resource monitor
     */
    ResourceMonitor* GetResourceMonitor() const;

    /**
     * @brief Get the logger instance
     * @return Pointer to logger
     */
    Logger* GetLogger() const;

private:
    /**
     * @brief Main service loop
     */
    void ServiceLoop();

    /**
     * @brief Initialize all subsystems
     * @return true if all subsystems initialized successfully
     */
    bool InitializeSubsystems();

    /**
     * @brief Shutdown all subsystems
     */
    void ShutdownSubsystems();

    /**
     * @brief Update service state and notify callbacks
     * @param newState New service state
     */
    void UpdateState(ServiceState newState);

    /**
     * @brief Handle error and notify callbacks
     * @param error Error message
     */
    void HandleError(const std::string& error);

    /**
     * @brief Perform periodic maintenance tasks
     */
    void PerformMaintenance();

    /**
     * @brief Check resource usage and throttle if necessary
     */
    void CheckResourceUsage();

    // Subsystem instances (using raw pointers to avoid incomplete type issues)
    ConfigurationManager* m_configManager;
    ArtifactEngine* m_artifactEngine;
    ResourceMonitor* m_resourceMonitor;
    Logger* m_logger;

    // Service state
    std::atomic<ServiceState> m_state;
    std::atomic<bool> m_shouldStop;
    std::thread m_serviceThread;

    // Callbacks
    ServiceStateCallback m_stateCallback;
    ErrorCallback m_errorCallback;
    MetricsCallback m_metricsCallback;

    // Configuration
    std::string m_configPath;
    TimePoint m_lastMaintenanceTime;
    TimePoint m_lastMetricsTime;

    // Synchronization
    mutable std::mutex m_stateMutex;
    mutable std::mutex m_callbackMutex;
};

} // namespace HoneyPottr
