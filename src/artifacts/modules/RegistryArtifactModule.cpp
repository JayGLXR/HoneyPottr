#include "artifacts/modules/RegistryArtifactModule.h"
#include "common/Constants.h"
#include "logging/Logger.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <mutex>
#include <chrono>

namespace HoneyPottr {

RegistryArtifactModule::RegistryArtifactModule() 
    : m_isActive(false)
    , m_logger(nullptr)
    , m_operationCount(0)
    , m_lastMetricsUpdate(std::chrono::steady_clock::now()) {
    m_config.type = ArtifactType::Registry;
    m_config.enabled = false;
    m_config.intensity = ArtifactIntensity::Medium;
    m_metrics = {};
}

RegistryArtifactModule::~RegistryArtifactModule() {
    Stop();
}

bool RegistryArtifactModule::Initialize(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_config = config;
    LogMessage(LogLevel::Info, "Registry Artifact Module initialized");
    
    return ValidateEnvironment();
}

bool RegistryArtifactModule::Start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_isActive) {
        return true;
    }
    
    if (!m_config.enabled) {
        LogMessage(LogLevel::Info, "Registry module is disabled in configuration");
        return false;
    }
    
    LogMessage(LogLevel::Info, "Starting Registry Artifact Module");
    
    // Set up artifacts based on configuration
    if (m_config.parameters.count("createVMwareKeys") && 
        m_config.parameters.at("createVMwareKeys") == "true") {
        SetupVMwareArtifacts();
    }
    
    if (m_config.parameters.count("createVirtualBoxKeys") && 
        m_config.parameters.at("createVirtualBoxKeys") == "true") {
        SetupVirtualBoxArtifacts();
    }
    
    if (m_config.parameters.count("createSandboxieKeys") && 
        m_config.parameters.at("createSandboxieKeys") == "true") {
        SetupSandboxieArtifacts();
    }
    
    if (m_config.parameters.count("createAnalysisToolKeys") && 
        m_config.parameters.at("createAnalysisToolKeys") == "true") {
        SetupAnalysisToolArtifacts();
    }
    
    // Create all artifacts
    if (!CreateArtifacts()) {
        LogMessage(LogLevel::Error, "Failed to create registry artifacts");
        return false;
    }
    
    m_isActive = true;
    LogMessage(LogLevel::Info, "Registry Artifact Module started successfully");
    
    return true;
}

bool RegistryArtifactModule::Stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return true;
    }
    
    LogMessage(LogLevel::Info, "Stopping Registry Artifact Module");
    
    // Remove all created artifacts
    RemoveArtifacts();
    
    m_isActive = false;
    LogMessage(LogLevel::Info, "Registry Artifact Module stopped");
    
    return true;
}

bool RegistryArtifactModule::Refresh() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return false;
    }
    
    LogMessage(LogLevel::Debug, "Refreshing registry artifacts");
    
    // Verify all artifacts still exist, recreate if needed
    for (auto& key : m_registryKeys) {
        if (key.created && !RegistryKeyExists(key.rootKey, key.subKey)) {
            LogMessage(LogLevel::Warning, "Registry artifact was removed, recreating: " + key.subKey);
            CreateRegistryKey(key);
        }
    }
    
    UpdateMetrics();
    return true;
}

ArtifactType RegistryArtifactModule::GetType() const {
    return ArtifactType::Registry;
}

bool RegistryArtifactModule::IsActive() const {
    return m_isActive;
}

const ArtifactConfig& RegistryArtifactModule::GetConfig() const {
    return m_config;
}

bool RegistryArtifactModule::UpdateConfig(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    bool wasActive = m_isActive;
    
    if (wasActive) {
        Stop();
    }
    
    m_config = config;
    
    if (wasActive && config.enabled) {
        return Start();
    }
    
    return true;
}

StringVector RegistryArtifactModule::GetActiveArtifacts() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    StringVector artifacts;
    for (const auto& key : m_registryKeys) {
        if (key.created) {
            artifacts.push_back(key.subKey);
        }
    }
    
    return artifacts;
}

PerformanceMetrics RegistryArtifactModule::GetMetrics() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metrics;
}

bool RegistryArtifactModule::ValidateEnvironment() const {
    // Check if we have permission to access registry
    HKEY testKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE", 0, KEY_READ, &testKey);
    if (result == ERROR_SUCCESS) {
        RegCloseKey(testKey);
        return true;
    }
    
    return false;
}

std::string RegistryArtifactModule::GetModuleName() const {
    return "RegistryArtifactModule";
}

std::string RegistryArtifactModule::GetVersion() const {
    return "1.0.0";
}

void RegistryArtifactModule::LogMessage(LogLevel level, const std::string& message) const {
    if (m_logger) {
        m_logger->Log(level, message, GetModuleName());
    }
}

bool RegistryArtifactModule::ShouldContinueOperation() const {
    return m_isActive;
}

void RegistryArtifactModule::SetLogger(Logger* logger) {
    m_logger = logger;
}

bool RegistryArtifactModule::CreateArtifacts() {
    bool allSuccess = true;
    
    for (auto& key : m_registryKeys) {
        if (!CreateRegistryKey(key)) {
            allSuccess = false;
            LogMessage(LogLevel::Warning, "Failed to create registry key: " + key.subKey);
        }
    }
    
    return allSuccess;
}

bool RegistryArtifactModule::RemoveArtifacts() {
    bool allSuccess = true;
    
    for (const auto& key : m_registryKeys) {
        if (key.created && !RemoveRegistryKey(key)) {
            allSuccess = false;
            LogMessage(LogLevel::Warning, "Failed to remove registry key: " + key.subKey);
        }
    }
    
    m_registryKeys.clear();
    return allSuccess;
}

bool RegistryArtifactModule::CreateRegistryKey(RegistryKey& key) {
    HKEY hKey;
    DWORD disposition;
    
    LONG result = RegCreateKeyExA(
        key.rootKey,
        key.subKey.c_str(),
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        nullptr,
        &hKey,
        &disposition
    );
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    // Create all values
    for (const auto& [name, typeAndData] : key.values) {
        auto [data, size] = ConvertToRegistryData(typeAndData.first, typeAndData.second);
        
        RegSetValueExA(
            hKey,
            name.c_str(),
            0,
            typeAndData.first,
            data.data(),
            size
        );
    }
    
    RegCloseKey(hKey);
    key.created = true;
    m_operationCount++;
    
    return true;
}

bool RegistryArtifactModule::RemoveRegistryKey(const RegistryKey& key) {
    LONG result = RegDeleteTreeA(key.rootKey, key.subKey.c_str());
    return result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND;
}

void RegistryArtifactModule::SetupVMwareArtifacts() {
    // VMware Tools registry key
    RegistryKey vmwareTools(HKEY_LOCAL_MACHINE, Constants::Registry::VMWARE_TOOLS);
    vmwareTools.values["InstallPath"] = {REG_SZ, "C:\\Program Files\\VMware\\VMware Tools\\"};
    vmwareTools.values["Version"] = {REG_SZ, "11.3.0"};
    m_registryKeys.push_back(vmwareTools);
    
    // VMware service key
    RegistryKey vmwareService(HKEY_LOCAL_MACHINE, Constants::Registry::VMWARE_SERVICE);
    vmwareService.values["ImagePath"] = {REG_SZ, "C:\\Windows\\System32\\drivers\\vmhgfs.sys"};
    vmwareService.values["Start"] = {REG_DWORD, "2"};
    m_registryKeys.push_back(vmwareService);
}

void RegistryArtifactModule::SetupVirtualBoxArtifacts() {
    // VirtualBox Guest Additions registry key
    RegistryKey vboxAdditions(HKEY_LOCAL_MACHINE, Constants::Registry::VBOX_GUEST_ADDITIONS);
    vboxAdditions.values["InstallDir"] = {REG_SZ, "C:\\Program Files\\Oracle\\VirtualBox Guest Additions\\"};
    vboxAdditions.values["Version"] = {REG_SZ, "6.1.34"};
    m_registryKeys.push_back(vboxAdditions);
    
    // VirtualBox service key
    RegistryKey vboxService(HKEY_LOCAL_MACHINE, Constants::Registry::VBOX_SERVICE);
    vboxService.values["ImagePath"] = {REG_SZ, "C:\\Windows\\System32\\drivers\\VBoxMouse.sys"};
    vboxService.values["Start"] = {REG_DWORD, "2"};
    m_registryKeys.push_back(vboxService);
}

void RegistryArtifactModule::SetupSandboxieArtifacts() {
    // Sandboxie registry key
    RegistryKey sandboxie(HKEY_LOCAL_MACHINE, Constants::Registry::SANDBOXIE);
    sandboxie.values["InstallationPath"] = {REG_SZ, "C:\\Program Files\\Sandboxie\\"};
    sandboxie.values["Version"] = {REG_SZ, "5.55.0"};
    m_registryKeys.push_back(sandboxie);
}

void RegistryArtifactModule::SetupAnalysisToolArtifacts() {
    // Wireshark uninstall key
    RegistryKey wireshark(HKEY_LOCAL_MACHINE, Constants::Registry::WIRESHARK_UNINSTALL);
    wireshark.values["DisplayName"] = {REG_SZ, "Wireshark"};
    wireshark.values["InstallLocation"] = {REG_SZ, "C:\\Program Files\\Wireshark\\"};
    wireshark.values["DisplayVersion"] = {REG_SZ, "3.6.2"};
    m_registryKeys.push_back(wireshark);
    
    // Process Monitor uninstall key
    RegistryKey procmon(HKEY_LOCAL_MACHINE, Constants::Registry::PROCMON_UNINSTALL);
    procmon.values["DisplayName"] = {REG_SZ, "Process Monitor"};
    procmon.values["InstallLocation"] = {REG_SZ, "C:\\Tools\\"};
    m_registryKeys.push_back(procmon);
}

std::pair<std::vector<BYTE>, DWORD> RegistryArtifactModule::ConvertToRegistryData(
    DWORD type, const std::string& value) const {
    
    std::vector<BYTE> data;
    DWORD size = 0;
    
    switch (type) {
        case REG_SZ:
        case REG_EXPAND_SZ: {
            size = static_cast<DWORD>(value.size() + 1);
            data.resize(size);
            memcpy(data.data(), value.c_str(), size);
            break;
        }
        
        case REG_DWORD: {
            DWORD dwordValue = static_cast<DWORD>(std::stoul(value));
            size = sizeof(DWORD);
            data.resize(size);
            memcpy(data.data(), &dwordValue, size);
            break;
        }
        
        default:
            break;
    }
    
    return {data, size};
}

HKEY RegistryArtifactModule::ParseHKeyString(const std::string& hkeyString) const {
    if (hkeyString == "HKEY_LOCAL_MACHINE" || hkeyString == "HKLM") {
        return HKEY_LOCAL_MACHINE;
    }
    if (hkeyString == "HKEY_CURRENT_USER" || hkeyString == "HKCU") {
        return HKEY_CURRENT_USER;
    }
    if (hkeyString == "HKEY_CLASSES_ROOT" || hkeyString == "HKCR") {
        return HKEY_CLASSES_ROOT;
    }
    
    return nullptr;
}

bool RegistryArtifactModule::RegistryKeyExists(HKEY rootKey, const std::string& subKey) const {
    HKEY hKey;
    LONG result = RegOpenKeyExA(rootKey, subKey.c_str(), 0, KEY_READ, &hKey);
    
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    
    return false;
}

void RegistryArtifactModule::UpdateMetrics() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastMetricsUpdate);
    
    m_metrics.timestamp = now;
    m_metrics.cpuUsage = 0.01; // Minimal CPU usage for registry operations
    m_metrics.memoryUsageMB = 1; // Minimal memory footprint
    m_metrics.diskIOPS = m_operationCount / std::max(1, static_cast<int>(duration.count()));
    m_metrics.networkBPS = 0;
    
    m_lastMetricsUpdate = now;
    m_operationCount = 0;
}

} // namespace HoneyPottr
