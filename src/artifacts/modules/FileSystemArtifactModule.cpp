#include "artifacts/modules/FileSystemArtifactModule.h"
#include "common/Constants.h"
#include "logging/Logger.h"
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <mutex>
#include <chrono>

namespace fs = std::filesystem;

namespace HoneyPottr {

FileSystemArtifactModule::FileSystemArtifactModule()
    : m_isActive(false)
    , m_logger(nullptr)
    , m_operationCount(0)
    , m_lastMetricsUpdate(std::chrono::steady_clock::now()) {
    m_config.type = ArtifactType::FileSystem;
    m_config.enabled = false;
    m_config.intensity = ArtifactIntensity::High;
    m_metrics = {};
}

FileSystemArtifactModule::~FileSystemArtifactModule() {
    Stop();
}

bool FileSystemArtifactModule::Initialize(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
    LogMessage(LogLevel::Info, "FileSystem Artifact Module initialized");
    return ValidateEnvironment();
}

bool FileSystemArtifactModule::Start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_isActive || !m_config.enabled) {
        return false;
    }
    
    LogMessage(LogLevel::Info, "Starting FileSystem Artifact Module");
    
    if (m_config.parameters.count("createVMwareFiles") && 
        m_config.parameters.at("createVMwareFiles") == "true") {
        SetupVMwareArtifacts();
    }
    
    if (m_config.parameters.count("createVirtualBoxFiles") && 
        m_config.parameters.at("createVirtualBoxFiles") == "true") {
        SetupVirtualBoxArtifacts();
    }
    
    if (m_config.parameters.count("createAnalysisToolFiles") && 
        m_config.parameters.at("createAnalysisToolFiles") == "true") {
        SetupAnalysisToolArtifacts();
    }
    
    if (m_config.parameters.count("createDecoyFiles") && 
        m_config.parameters.at("createDecoyFiles") == "true") {
        SetupDecoyArtifacts();
    }
    
    if (!CreateArtifacts()) {
        LogMessage(LogLevel::Error, "Failed to create file system artifacts");
        return false;
    }
    
    m_isActive = true;
    LogMessage(LogLevel::Info, "FileSystem Artifact Module started");
    return true;
}

bool FileSystemArtifactModule::Stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return true;
    }
    
    LogMessage(LogLevel::Info, "Stopping FileSystem Artifact Module");
    RemoveArtifacts();
    m_isActive = false;
    return true;
}

bool FileSystemArtifactModule::Refresh() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isActive) {
        return false;
    }
    
    for (auto& artifact : m_artifacts) {
        if (artifact.created && !FileExists(artifact.path)) {
            LogMessage(LogLevel::Warning, "File artifact was removed, recreating: " + artifact.path);
            CreateFileArtifact(artifact);
        }
    }
    
    UpdateMetrics();
    return true;
}

ArtifactType FileSystemArtifactModule::GetType() const { return ArtifactType::FileSystem; }
bool FileSystemArtifactModule::IsActive() const { return m_isActive; }
const ArtifactConfig& FileSystemArtifactModule::GetConfig() const { return m_config; }

bool FileSystemArtifactModule::UpdateConfig(const ArtifactConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bool wasActive = m_isActive;
    if (wasActive) Stop();
    m_config = config;
    if (wasActive && config.enabled) return Start();
    return true;
}

StringVector FileSystemArtifactModule::GetActiveArtifacts() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    StringVector result;
    for (const auto& artifact : m_artifacts) {
        if (artifact.created) result.push_back(artifact.path);
    }
    return result;
}

PerformanceMetrics FileSystemArtifactModule::GetMetrics() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_metrics;
}

bool FileSystemArtifactModule::ValidateEnvironment() const {
    // Check if we can write to system directories
    return true; // Will fail gracefully if we can't
}

std::string FileSystemArtifactModule::GetModuleName() const { return "FileSystemArtifactModule"; }
std::string FileSystemArtifactModule::GetVersion() const { return "1.0.0"; }

void FileSystemArtifactModule::LogMessage(LogLevel level, const std::string& message) const {
    if (m_logger) {
        m_logger->Log(level, message, GetModuleName());
    }
}

bool FileSystemArtifactModule::ShouldContinueOperation() const { return m_isActive; }

void FileSystemArtifactModule::SetLogger(Logger* logger) {
    m_logger = logger;
}

bool FileSystemArtifactModule::CreateArtifacts() {
    bool allSuccess = true;
    for (auto& artifact : m_artifacts) {
        if (!CreateFileArtifact(artifact)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool FileSystemArtifactModule::RemoveArtifacts() {
    bool allSuccess = true;
    for (const auto& artifact : m_artifacts) {
        if (artifact.created) RemoveFileArtifact(artifact);
    }
    m_artifacts.clear();
    return allSuccess;
}

bool FileSystemArtifactModule::CreateFileArtifact(FileArtifact& artifact) {
    try {
        if (!EnsureDirectoryExists(artifact.path)) {
            return false;
        }
        
        std::ofstream file(artifact.path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        if (!artifact.content.empty()) {
            file << artifact.content;
        }
        file.close();
        
        if (artifact.attributes != FILE_ATTRIBUTE_NORMAL) {
            SetFileAttributesA(artifact.path.c_str(), artifact.attributes);
        }
        
        artifact.created = true;
        m_operationCount++;
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystemArtifactModule::RemoveFileArtifact(const FileArtifact& artifact) {
    try {
        return DeleteFileA(artifact.path.c_str()) != 0 || 
               GetLastError() == ERROR_FILE_NOT_FOUND;
    } catch (...) {
        return false;
    }
}

void FileSystemArtifactModule::SetupVMwareArtifacts() {
    FileArtifact vmtools(Constants::FilePaths::VMWARE_TOOLS_DIR);
    vmtools.attributes = FILE_ATTRIBUTE_DIRECTORY;
    m_artifacts.push_back(vmtools);
    
    FileArtifact driver(Constants::FilePaths::VMWARE_HGFS_DRIVER);
    driver.content = "VMware HGFS Driver Placeholder";
    driver.attributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;
    m_artifacts.push_back(driver);
}

void FileSystemArtifactModule::SetupVirtualBoxArtifacts() {
    FileArtifact vboxAdditions(Constants::FilePaths::VBOX_ADDITIONS_DIR);
    vboxAdditions.attributes = FILE_ATTRIBUTE_DIRECTORY;
    m_artifacts.push_back(vboxAdditions);
    
    FileArtifact mouse(Constants::FilePaths::VBOX_MOUSE_DRIVER);
    mouse.content = "VirtualBox Mouse Driver Placeholder";
    mouse.attributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;
    m_artifacts.push_back(mouse);
}

void FileSystemArtifactModule::SetupAnalysisToolArtifacts() {
    FileArtifact wireshark(Constants::FilePaths::WIRESHARK_DESKTOP);
    wireshark.content = "[InternetShortcut]\nURL=C:\\Program Files\\Wireshark\\Wireshark.exe";
    m_artifacts.push_back(wireshark);
    
    FileArtifact procmon(Constants::FilePaths::PROCMON_TOOLS);
    procmon.content = "Process Monitor Placeholder";
    procmon.attributes = FILE_ATTRIBUTE_READONLY;
    m_artifacts.push_back(procmon);
}

void FileSystemArtifactModule::SetupDecoyArtifacts() {
    // Create some decoy configuration files
    FileArtifact config("C:\\ProgramData\\SecurityTools\\config.ini");
    config.content = "[Settings]\nMonitorProcesses=true\nLogNetwork=true";
    m_artifacts.push_back(config);
}

bool FileSystemArtifactModule::FileExists(const std::string& path) const {
    DWORD attr = GetFileAttributesA(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES;
}

bool FileSystemArtifactModule::EnsureDirectoryExists(const std::string& path) {
    try {
        fs::path p(path);
        fs::path dir = p.parent_path();
        if (!dir.empty() && !fs::exists(dir)) {
            return fs::create_directories(dir);
        }
        return true;
    } catch (...) {
        return false;
    }
}

void FileSystemArtifactModule::UpdateMetrics() {
    auto now = std::chrono::steady_clock::now();
    m_metrics.timestamp = now;
    m_metrics.cpuUsage = 0.05;
    m_metrics.memoryUsageMB = 2;
    m_metrics.diskIOPS = m_operationCount;
    m_metrics.networkBPS = 0;
    m_lastMetricsUpdate = now;
    m_operationCount = 0;
}

} // namespace HoneyPottr
