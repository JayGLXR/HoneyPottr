#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <windows.h>

namespace HoneyPottr {

// Forward declarations
class IArtifactModule;
class ConfigurationManager;
class ResourceMonitor;
class Logger;

// Type aliases
using ArtifactModulePtr = std::unique_ptr<IArtifactModule>;
using ConfigMap = std::map<std::string, std::string>;
using StringVector = std::vector<std::string>;
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::milliseconds;

// Enumerations
enum class ServiceState {
    Stopped,
    Starting,
    Running,
    Stopping,
    Paused
};

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

enum class ArtifactType {
    Registry,
    FileSystem,
    Process,
    Network,
    Hardware
};

enum class ArtifactIntensity {
    Low,
    Medium,
    High,
    Maximum
};

// Structures
struct ServiceConfig {
    std::string serviceName;
    std::string displayName;
    std::string description;
    DWORD serviceType;
    DWORD startType;
    DWORD errorControl;
    std::string binaryPath;
    std::string dependencies;
    std::string account;
    std::string password;
};

struct ResourceLimits {
    size_t maxMemoryMB;
    double maxCpuPercent;
    size_t maxDiskIOPS;
    size_t maxNetworkBPS;
};

struct ArtifactConfig {
    ArtifactType type;
    ArtifactIntensity intensity;
    bool enabled;
    ConfigMap parameters;
    Duration refreshInterval;
};

struct PerformanceMetrics {
    double cpuUsage;
    size_t memoryUsageMB;
    size_t diskIOPS;
    size_t networkBPS;
    TimePoint timestamp;
};

// Callback types
using ServiceStateCallback = std::function<void(ServiceState)>;
using ErrorCallback = std::function<void(const std::string&)>;
using MetricsCallback = std::function<void(const PerformanceMetrics&)>;

// Constants
constexpr size_t DEFAULT_MAX_MEMORY_MB = 50;
constexpr double DEFAULT_MAX_CPU_PERCENT = 1.0;
constexpr Duration DEFAULT_UPDATE_INTERVAL = std::chrono::seconds(3600);
constexpr Duration DEFAULT_MONITORING_INTERVAL = std::chrono::seconds(30);

} // namespace HoneyPottr
