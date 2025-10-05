#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <windows.h>
#include <pdh.h>
#include <psapi.h>
#include "common/Types.h"

namespace HoneyPottr {

// Forward declaration
class Logger;

/**
 * @brief System resource monitor for tracking CPU, memory, disk, and network usage
 * 
 * This class monitors system resources used by the HoneyPottr service and
 * can enforce resource limits to prevent excessive consumption.
 */
class ResourceMonitor {
public:
    ResourceMonitor();
    ~ResourceMonitor();

    // Non-copyable, non-movable
    ResourceMonitor(const ResourceMonitor&) = delete;
    ResourceMonitor& operator=(const ResourceMonitor&) = delete;
    ResourceMonitor(ResourceMonitor&&) = delete;
    ResourceMonitor& operator=(ResourceMonitor&&) = delete;

    /**
     * @brief Initialize the resource monitor
     * @param limits Resource limits to enforce
     * @param logger Logger instance for reporting
     * @return true if initialization succeeded
     */
    bool Initialize(const ResourceLimits& limits, Logger* logger = nullptr);

    /**
     * @brief Start monitoring resources
     * @return true if monitoring started successfully
     */
    bool Start();

    /**
     * @brief Stop monitoring resources
     */
    void Stop();

    /**
     * @brief Get current performance metrics
     * @return Performance metrics structure
     */
    PerformanceMetrics GetMetrics() const;

    /**
     * @brief Check if resource limits are being exceeded
     * @return true if limits are exceeded
     */
    bool IsResourceLimitExceeded() const;

    /**
     * @brief Get CPU usage percentage
     * @return CPU usage (0-100%)
     */
    double GetCpuUsage() const;

    /**
     * @brief Get memory usage in MB
     * @return Memory usage in megabytes
     */
    size_t GetMemoryUsage() const;

    /**
     * @brief Get disk I/O operations per second
     * @return Disk IOPS
     */
    size_t GetDiskIOPS() const;

    /**
     * @brief Get network bandwidth in bytes per second
     * @return Network BPS
     */
    size_t GetNetworkBPS() const;

    /**
     * @brief Update resource limits
     * @param limits New resource limits
     */
    void SetResourceLimits(const ResourceLimits& limits);

    /**
     * @brief Get current resource limits
     * @return Resource limits structure
     */
    ResourceLimits GetResourceLimits() const;

    /**
     * @brief Check if monitoring is active
     * @return true if monitoring is active
     */
    bool IsMonitoring() const;

private:
    /**
     * @brief Monitoring thread function
     */
    void MonitoringLoop();

    /**
     * @brief Update CPU usage metrics
     */
    void UpdateCpuUsage();

    /**
     * @brief Update memory usage metrics
     */
    void UpdateMemoryUsage();

    /**
     * @brief Update disk I/O metrics
     */
    void UpdateDiskIO();

    /**
     * @brief Update network metrics
     */
    void UpdateNetwork();

    /**
     * @brief Initialize performance counters
     * @return true if initialization succeeded
     */
    bool InitializeCounters();

    /**
     * @brief Cleanup performance counters
     */
    void CleanupCounters();

    // Configuration
    ResourceLimits m_limits;
    Logger* m_logger;
    bool m_isMonitoring;

    // Metrics
    mutable std::mutex m_metricsMutex;
    PerformanceMetrics m_metrics;

    // Monitoring thread
    std::thread m_monitoringThread;
    std::atomic<bool> m_shouldStop;
    std::mutex m_conditionMutex;
    std::condition_variable m_condition;

    // Windows-specific handles
    HANDLE m_processHandle;
    PDH_HQUERY m_pdhQuery;
    PDH_HCOUNTER m_cpuCounter;
    
    // Baseline metrics for delta calculations
    ULONGLONG m_lastCpuTime;
    ULONGLONG m_lastSystemTime;
    ULONGLONG m_lastDiskReads;
    ULONGLONG m_lastDiskWrites;
    ULONGLONG m_lastNetworkBytes;
    TimePoint m_lastUpdateTime;
};

} // namespace HoneyPottr
