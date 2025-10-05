#include "monitoring/ResourceMonitor.h"
#include "logging/Logger.h"
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>

namespace HoneyPottr {

ResourceMonitor::ResourceMonitor()
    : m_logger(nullptr)
    , m_isMonitoring(false)
    , m_shouldStop(false)
    , m_processHandle(nullptr)
    , m_pdhQuery(nullptr)
    , m_cpuCounter(nullptr)
    , m_lastCpuTime(0)
    , m_lastSystemTime(0)
    , m_lastDiskReads(0)
    , m_lastDiskWrites(0)
    , m_lastNetworkBytes(0)
    , m_lastUpdateTime(std::chrono::steady_clock::now()) {
    
    m_limits.maxMemoryMB = 50;
    m_limits.maxCpuPercent = 1.0;
    m_limits.maxDiskIOPS = 100;
    m_limits.maxNetworkBPS = 1024;
    
    m_metrics.cpuUsage = 0.0;
    m_metrics.memoryUsageMB = 0;
    m_metrics.diskIOPS = 0;
    m_metrics.networkBPS = 0;
    m_metrics.timestamp = std::chrono::steady_clock::now();
}

ResourceMonitor::~ResourceMonitor() {
    Stop();
    CleanupCounters();
}

bool ResourceMonitor::Initialize(const ResourceLimits& limits, Logger* logger) {
    m_limits = limits;
    m_logger = logger;
    
    // Get handle to current process
    m_processHandle = GetCurrentProcess();
    
    // Initialize performance counters
    if (!InitializeCounters()) {
        if (m_logger) {
            m_logger->Warning("Failed to initialize performance counters", "ResourceMonitor");
        }
        // Non-fatal, we can still monitor some metrics
    }
    
    if (m_logger) {
        m_logger->Info("ResourceMonitor initialized", "ResourceMonitor");
    }
    
    return true;
}

bool ResourceMonitor::Start() {
    if (m_isMonitoring) {
        return true;
    }
    
    m_shouldStop = false;
    m_isMonitoring = true;
    m_monitoringThread = std::thread(&ResourceMonitor::MonitoringLoop, this);
    
    if (m_logger) {
        m_logger->Info("Resource monitoring started", "ResourceMonitor");
    }
    
    return true;
}

void ResourceMonitor::Stop() {
    if (!m_isMonitoring) {
        return;
    }
    
    m_shouldStop = true;
    m_condition.notify_all();
    
    if (m_monitoringThread.joinable()) {
        m_monitoringThread.join();
    }
    
    m_isMonitoring = false;
    
    if (m_logger) {
        m_logger->Info("Resource monitoring stopped", "ResourceMonitor");
    }
}

PerformanceMetrics ResourceMonitor::GetMetrics() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    return m_metrics;
}

bool ResourceMonitor::IsResourceLimitExceeded() const {
    auto metrics = GetMetrics();
    
    return (metrics.cpuUsage > m_limits.maxCpuPercent) ||
           (metrics.memoryUsageMB > m_limits.maxMemoryMB) ||
           (metrics.diskIOPS > m_limits.maxDiskIOPS) ||
           (metrics.networkBPS > m_limits.maxNetworkBPS);
}

double ResourceMonitor::GetCpuUsage() const {
    return GetMetrics().cpuUsage;
}

size_t ResourceMonitor::GetMemoryUsage() const {
    return GetMetrics().memoryUsageMB;
}

size_t ResourceMonitor::GetDiskIOPS() const {
    return GetMetrics().diskIOPS;
}

size_t ResourceMonitor::GetNetworkBPS() const {
    return GetMetrics().networkBPS;
}

void ResourceMonitor::SetResourceLimits(const ResourceLimits& limits) {
    m_limits = limits;
    
    if (m_logger) {
        m_logger->Info("Resource limits updated", "ResourceMonitor");
    }
}

ResourceLimits ResourceMonitor::GetResourceLimits() const {
    return m_limits;
}

bool ResourceMonitor::IsMonitoring() const {
    return m_isMonitoring;
}

void ResourceMonitor::MonitoringLoop() {
    while (!m_shouldStop) {
        // Update all metrics
        UpdateCpuUsage();
        UpdateMemoryUsage();
        UpdateDiskIO();
        UpdateNetwork();
        
        // Update timestamp
        {
            std::lock_guard<std::mutex> lock(m_metricsMutex);
            m_metrics.timestamp = std::chrono::steady_clock::now();
        }
        
        // Check if limits are exceeded
        if (IsResourceLimitExceeded() && m_logger) {
            auto metrics = GetMetrics();
            std::string warning = "Resource limit exceeded - CPU: " + 
                                std::to_string(metrics.cpuUsage) + "%, Memory: " +
                                std::to_string(metrics.memoryUsageMB) + "MB";
            m_logger->Warning(warning, "ResourceMonitor");
        }
        
        // Sleep for 1 second
        std::unique_lock<std::mutex> lock(m_conditionMutex);
        m_condition.wait_for(lock, std::chrono::seconds(1), [this]() {
            return m_shouldStop.load();
        });
    }
}

void ResourceMonitor::UpdateCpuUsage() {
    // Get process times
    FILETIME createTime, exitTime, kernelTime, userTime;
    if (!GetProcessTimes(m_processHandle, &createTime, &exitTime, &kernelTime, &userTime)) {
        return;
    }
    
    // Convert to ULONGLONG
    ULARGE_INTEGER kernel, user;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;
    
    ULONGLONG processCpuTime = kernel.QuadPart + user.QuadPart;
    
    // Get system time
    FILETIME systemTimeAsFileTime;
    GetSystemTimeAsFileTime(&systemTimeAsFileTime);
    ULARGE_INTEGER systemTime;
    systemTime.LowPart = systemTimeAsFileTime.dwLowDateTime;
    systemTime.HighPart = systemTimeAsFileTime.dwHighDateTime;
    
    // Calculate CPU usage as percentage
    if (m_lastCpuTime > 0) {
        ULONGLONG cpuDelta = processCpuTime - m_lastCpuTime;
        ULONGLONG systemDelta = systemTime.QuadPart - m_lastSystemTime;
        
        if (systemDelta > 0) {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            
            double cpuUsage = (static_cast<double>(cpuDelta) / static_cast<double>(systemDelta)) * 
                            100.0 * sysInfo.dwNumberOfProcessors;
            
            std::lock_guard<std::mutex> lock(m_metricsMutex);
            m_metrics.cpuUsage = std::min(cpuUsage, 100.0);
        }
    }
    
    m_lastCpuTime = processCpuTime;
    m_lastSystemTime = systemTime.QuadPart;
}

void ResourceMonitor::UpdateMemoryUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(m_processHandle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        std::lock_guard<std::mutex> lock(m_metricsMutex);
        m_metrics.memoryUsageMB = pmc.WorkingSetSize / (1024 * 1024);
    }
}

void ResourceMonitor::UpdateDiskIO() {
    IO_COUNTERS ioCounters;
    if (GetProcessIoCounters(m_processHandle, &ioCounters)) {
        ULONGLONG totalIO = ioCounters.ReadOperationCount + ioCounters.WriteOperationCount;
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastUpdateTime);
        
        if (duration.count() > 0 && m_lastDiskReads > 0) {
            ULONGLONG ioDelta = totalIO - (m_lastDiskReads + m_lastDiskWrites);
            size_t iops = static_cast<size_t>(ioDelta / duration.count());
            
            std::lock_guard<std::mutex> lock(m_metricsMutex);
            m_metrics.diskIOPS = iops;
        }
        
        m_lastDiskReads = ioCounters.ReadOperationCount;
        m_lastDiskWrites = ioCounters.WriteOperationCount;
        m_lastUpdateTime = now;
    }
}

void ResourceMonitor::UpdateNetwork() {
    // Network monitoring is more complex and would require WMI or specific counters
    // For now, we'll provide a placeholder implementation
    // In a production system, this would track actual network usage
    
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    m_metrics.networkBPS = 0; // Placeholder
}

bool ResourceMonitor::InitializeCounters() {
    // Initialize PDH query for CPU monitoring
    PDH_STATUS status = PdhOpenQuery(nullptr, 0, &m_pdhQuery);
    if (status != ERROR_SUCCESS) {
        return false;
    }
    
    // Add CPU counter
    status = PdhAddCounterA(m_pdhQuery, 
                           "\\Process(HoneyPottr)\\% Processor Time", 
                           0, 
                           &m_cpuCounter);
    
    if (status != ERROR_SUCCESS) {
        // Try alternative counter name
        status = PdhAddCounterA(m_pdhQuery, 
                               "\\Processor(_Total)\\% Processor Time", 
                               0, 
                               &m_cpuCounter);
    }
    
    if (status != ERROR_SUCCESS) {
        PdhCloseQuery(m_pdhQuery);
        m_pdhQuery = nullptr;
        return false;
    }
    
    // Collect initial data
    PdhCollectQueryData(m_pdhQuery);
    
    return true;
}

void ResourceMonitor::CleanupCounters() {
    if (m_pdhQuery) {
        PdhCloseQuery(m_pdhQuery);
        m_pdhQuery = nullptr;
    }
    m_cpuCounter = nullptr;
}

} // namespace HoneyPottr
