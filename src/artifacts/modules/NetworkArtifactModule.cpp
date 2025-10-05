#include "artifacts/modules/NetworkArtifactModule.h"
#include "common/Constants.h"
#include "logging/Logger.h"
#include <iphlpapi.h>
#include <mutex>
#include <chrono>
#pragma comment(lib, "iphlpapi.lib")

namespace HoneyPottr {

NetworkArtifactModule::NetworkArtifactModule()
    : m_isActive(false)
    , m_logger(nullptr)
    , m_operationCount(0)
    , m_lastMetricsUpdate(std::chrono::steady_clock::now()) {
    m_config.type = ArtifactType::Network;
    m_config.enabled = false;
    m_config.intensity = ArtifactIntensity::Low;
    m_metrics = {};
}

NetworkArtifactModule::~NetworkArtifactModule() {
    Stop();
}

bool NetworkArtifactModule::Initialize(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
    LogMessage(LogLevel::Info, "Network Artifact Module initialized");
    return ValidateEnvironment();
}

bool NetworkArtifactModule::Start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_isActive || !m_config.enabled) {
        return false;
    }
    
    LogMessage(LogLevel::Info, "Starting Network Artifact Module");
    
    // Note: Creating virtual network adapters requires driver installation
    // This is typically done at installation time, not runtime
    // We simulate by documenting what would be present
    
    if (m_config.parameters.count("simulateVMwareNetwork") && 
        m_config.parameters.at("simulateVMwareNetwork") == "true") {
        SetupVMwareArtifacts();
    }
    
    if (m_config.parameters.count("simulateVirtualBoxNetwork") && 
        m_config.parameters.at("simulateVirtualBoxNetwork") == "true") {
        SetupVirtualBoxArtifacts();
    }
    
    m_isActive = true;
    LogMessage(LogLevel::Info, "Network Artifact Module started (simulation mode)");
    return true;
}

bool NetworkArtifactModule::Stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return true;
    }
    
    LogMessage(LogLevel::Info, "Stopping Network Artifact Module");
    RemoveArtifacts();
    m_isActive = false;
    return true;
}

bool NetworkArtifactModule::Refresh() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return false;
    }
    
    UpdateMetrics();
    return true;
}

ArtifactType NetworkArtifactModule::GetType() const { return ArtifactType::Network; }
bool NetworkArtifactModule::IsActive() const { return m_isActive; }
const ArtifactConfig& NetworkArtifactModule::GetConfig() const { return m_config; }

bool NetworkArtifactModule::UpdateConfig(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bool wasActive = m_isActive;
    if (wasActive) Stop();
    m_config = config;
    if (wasActive && config.enabled) return Start();
    return true;
}

StringVector NetworkArtifactModule::GetActiveArtifacts() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector result;
    for (const auto& artifact : m_artifacts) {
        if (artifact.created) result.push_back(artifact.adapterName);
    }
    return result;
}

PerformanceMetrics NetworkArtifactModule::GetMetrics() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metrics;
}

bool NetworkArtifactModule::ValidateEnvironment() const {
    return true;
}

std::string NetworkArtifactModule::GetModuleName() const { return "NetworkArtifactModule"; }
std::string NetworkArtifactModule::GetVersion() const { return "1.0.0"; }

void NetworkArtifactModule::LogMessage(LogLevel level, const std::string& message) const {
    if (m_logger) {
        m_logger->Log(level, message, GetModuleName());
    }
}

bool NetworkArtifactModule::ShouldContinueOperation() const { return m_isActive; }

void NetworkArtifactModule::SetLogger(Logger* logger) {
    m_logger = logger;
}

bool NetworkArtifactModule::CreateArtifacts() {
    // In production: Would create virtual adapters or configure existing ones
    for (auto& artifact : m_artifacts) {
        artifact.created = true;
    }
    return true;
}

bool NetworkArtifactModule::RemoveArtifacts() {
    m_artifacts.clear();
    return true;
}

void NetworkArtifactModule::SetupVMwareArtifacts() {
    NetworkArtifact adapter(Constants::Network::VMWARE_ADAPTER_NAME);
    adapter.macAddress = Constants::Network::VMWARE_MAC_PREFIX;
    m_artifacts.push_back(adapter);
}

void NetworkArtifactModule::SetupVirtualBoxArtifacts() {
    NetworkArtifact adapter(Constants::Network::VBOX_ADAPTER_NAME);
    adapter.macAddress = Constants::Network::VBOX_MAC_PREFIX;
    m_artifacts.push_back(adapter);
}

StringVector NetworkArtifactModule::GetNetworkAdapters() const {
    StringVector adapters;
    
    ULONG bufferSize = 15000;
    PIP_ADAPTER_ADDRESSES addresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(malloc(bufferSize));
    
    if (addresses && GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, addresses, &bufferSize) == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES current = addresses;
        while (current) {
            adapters.push_back(current->Description);
            current = current->Next;
        }
        free(addresses);
    } else if (addresses) {
        free(addresses);
    }
    
    return adapters;
}

void NetworkArtifactModule::UpdateMetrics() {
    auto now = std::chrono::steady_clock::now();
    m_metrics.timestamp = now;
    m_metrics.cpuUsage = 0.01;
    m_metrics.memoryUsageMB = 1;
    m_metrics.diskIOPS = 0;
    m_metrics.networkBPS = 0;
    m_lastMetricsUpdate = now;
}

} // namespace HoneyPottr
