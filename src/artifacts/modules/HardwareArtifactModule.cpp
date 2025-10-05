#include "artifacts/modules/HardwareArtifactModule.h"
#include "common/Constants.h"
#include "logging/Logger.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <mutex>
#include <chrono>
#pragma comment(lib, "wbemuuid.lib")

namespace HoneyPottr {

HardwareArtifactModule::HardwareArtifactModule()
    : m_isActive(false)
    , m_logger(nullptr)
    , m_wmiInitialized(false)
    , m_operationCount(0)
    , m_lastMetricsUpdate(std::chrono::steady_clock::now()) {
    m_config.type = ArtifactType::Hardware;
    m_config.enabled = false;
    m_config.intensity = ArtifactIntensity::Medium;
    m_metrics = {};
}

HardwareArtifactModule::~HardwareArtifactModule() {
    Stop();
    CleanupWMI();
}

bool HardwareArtifactModule::Initialize(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
    LogMessage(LogLevel::Info, "Hardware Artifact Module initialized");
    
    if (!InitializeWMI()) {
        LogMessage(LogLevel::Warning, "Failed to initialize WMI");
        return false;
    }
    
    return ValidateEnvironment();
}

bool HardwareArtifactModule::Start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_isActive || !m_config.enabled) {
        return false;
    }
    
    LogMessage(LogLevel::Info, "Starting Hardware Artifact Module");
    
    // Note: Modifying WMI data requires special permissions and techniques
    // This is typically done through registry or WMI MOF files
    // We document what would be modified
    
    if (m_config.parameters.count("simulateVMwareBIOS") && 
        m_config.parameters.at("simulateVMwareBIOS") == "true") {
        SetupVMwareBIOSArtifacts();
    }
    
    if (m_config.parameters.count("simulateVirtualBoxBIOS") && 
        m_config.parameters.at("simulateVirtualBoxBIOS") == "true") {
        SetupVirtualBoxBIOSArtifacts();
    }
    
    if (m_config.parameters.count("simulateVirtualHardware") && 
        m_config.parameters.at("simulateVirtualHardware") == "true") {
        SetupVirtualHardwareArtifacts();
    }
    
    m_isActive = true;
    LogMessage(LogLevel::Info, "Hardware Artifact Module started (simulation mode)");
    return true;
}

bool HardwareArtifactModule::Stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return true;
    }
    
    LogMessage(LogLevel::Info, "Stopping Hardware Artifact Module");
    RemoveArtifacts();
    m_isActive = false;
    return true;
}

bool HardwareArtifactModule::Refresh() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return false;
    }
    
    UpdateMetrics();
    return true;
}

ArtifactType HardwareArtifactModule::GetType() const { return ArtifactType::Hardware; }
bool HardwareArtifactModule::IsActive() const { return m_isActive; }
const ArtifactConfig& HardwareArtifactModule::GetConfig() const { return m_config; }

bool HardwareArtifactModule::UpdateConfig(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bool wasActive = m_isActive;
    if (wasActive) Stop();
    m_config = config;
    if (wasActive && config.enabled) return Start();
    return true;
}

StringVector HardwareArtifactModule::GetActiveArtifacts() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector result;
    for (const auto& artifact : m_artifacts) {
        if (artifact.created) result.push_back(artifact.type + ": " + artifact.value);
    }
    return result;
}

PerformanceMetrics HardwareArtifactModule::GetMetrics() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metrics;
}

bool HardwareArtifactModule::ValidateEnvironment() const {
    return m_wmiInitialized;
}

std::string HardwareArtifactModule::GetModuleName() const { return "HardwareArtifactModule"; }
std::string HardwareArtifactModule::GetVersion() const { return "1.0.0"; }

void HardwareArtifactModule::LogMessage(LogLevel level, const std::string& message) const {
    if (m_logger) {
        m_logger->Log(level, message, GetModuleName());
    }
}

bool HardwareArtifactModule::ShouldContinueOperation() const { return m_isActive; }

void HardwareArtifactModule::SetLogger(Logger* logger) {
    m_logger = logger;
}

bool HardwareArtifactModule::CreateArtifacts() {
    // In production: Would modify WMI data or registry keys
    for (auto& artifact : m_artifacts) {
        artifact.created = true;
    }
    return true;
}

bool HardwareArtifactModule::RemoveArtifacts() {
    m_artifacts.clear();
    return true;
}

void HardwareArtifactModule::SetupVMwareBIOSArtifacts() {
    HardwareArtifact biosVendor("BIOS Vendor");
    biosVendor.value = Constants::Hardware::VMWARE_BIOS_VENDOR;
    m_artifacts.push_back(biosVendor);
    
    HardwareArtifact sysManufacturer("System Manufacturer");
    sysManufacturer.value = Constants::Hardware::VMWARE_SYSTEM_MANUFACTURER;
    m_artifacts.push_back(sysManufacturer);
    
    HardwareArtifact sysProduct("System Product");
    sysProduct.value = Constants::Hardware::VMWARE_SYSTEM_PRODUCT;
    m_artifacts.push_back(sysProduct);
}

void HardwareArtifactModule::SetupVirtualBoxBIOSArtifacts() {
    HardwareArtifact biosVendor("BIOS Vendor");
    biosVendor.value = Constants::Hardware::VBOX_BIOS_VENDOR;
    m_artifacts.push_back(biosVendor);
    
    HardwareArtifact sysManufacturer("System Manufacturer");
    sysManufacturer.value = Constants::Hardware::VBOX_SYSTEM_MANUFACTURER;
    m_artifacts.push_back(sysManufacturer);
    
    HardwareArtifact sysProduct("System Product");
    sysProduct.value = Constants::Hardware::VBOX_SYSTEM_PRODUCT;
    m_artifacts.push_back(sysProduct);
}

void HardwareArtifactModule::SetupVirtualHardwareArtifacts() {
    HardwareArtifact diskModel("Disk Model");
    diskModel.value = "VBOX HARDDISK";
    m_artifacts.push_back(diskModel);
}

bool HardwareArtifactModule::InitializeWMI() {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return false;
    }
    
    hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
                              RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        // Security already initialized is OK
    }
    
    m_wmiInitialized = true;
    return true;
}

void HardwareArtifactModule::CleanupWMI() {
    if (m_wmiInitialized) {
        CoUninitialize();
        m_wmiInitialized = false;
    }
}

void HardwareArtifactModule::UpdateMetrics() {
    auto now = std::chrono::steady_clock::now();
    m_metrics.timestamp = now;
    m_metrics.cpuUsage = 0.02;
    m_metrics.memoryUsageMB = 2;
    m_metrics.diskIOPS = 0;
    m_metrics.networkBPS = 0;
    m_lastMetricsUpdate = now;
}

} // namespace HoneyPottr
