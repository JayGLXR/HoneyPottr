#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <mutex>
#include "config/ConfigurationManager.h"
#include "common/Constants.h"
#include <windows.h>

using json = nlohmann::json;

namespace HoneyPottr {

ConfigurationManager::ConfigurationManager() 
    : m_isLoaded(false) {
}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::LoadConfiguration(const std::string& configPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        // Determine the actual config file path
        m_configPath = configPath.empty() ? Constants::DEFAULT_CONFIG_FILE : configPath;
        
        // Try to resolve relative paths
        std::string resolvedPath = m_configPath;
        if (m_configPath.find(':') == std::string::npos) {
            // Relative path - try to resolve relative to executable
            char exePath[MAX_PATH];
            if (GetModuleFileNameA(nullptr, exePath, MAX_PATH) > 0) {
                std::string exeDir(exePath);
                size_t lastSlash = exeDir.find_last_of("\\/");
                if (lastSlash != std::string::npos) {
                    resolvedPath = exeDir.substr(0, lastSlash + 1) + m_configPath;
                }
            }
        }
        
        std::ifstream configFile(resolvedPath);
        if (!configFile.is_open()) {
            // Try the original path
            configFile.open(m_configPath);
            if (!configFile.is_open()) {
                std::cerr << "Failed to open configuration file: " << m_configPath << std::endl;
                return false;
            }
        } else {
            m_configPath = resolvedPath;
        }
        
        // Parse JSON
        m_config = json::parse(configFile, nullptr, false);
        if (m_config.is_discarded()) {
            std::cerr << "Failed to parse configuration JSON" << std::endl;
            return false;
        }
        
        configFile.close();
        
        // Apply defaults for missing values
        ApplyDefaults();
        
        // Merge with profile files if specified
        if (m_config.contains("artifacts") && m_config["artifacts"].contains("profiles")) {
            auto profiles = m_config["artifacts"]["profiles"];
            if (profiles.is_array()) {
                std::vector<std::string> profileNames;
                for (const auto& profile : profiles) {
                    if (profile.is_string()) {
                        profileNames.push_back(profile.get<std::string>());
                    }
                }
                MergeProfiles(profileNames);
            }
        }
        
        // Validate configuration
        if (!ValidateConfiguration()) {
            std::cerr << "Configuration validation failed" << std::endl;
            return false;
        }
        
        m_isLoaded = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception loading configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigurationManager::ReloadConfiguration() {
    if (m_configPath.empty()) {
        return false;
    }
    return LoadConfiguration(m_configPath);
}

bool ConfigurationManager::SaveConfiguration(const std::string& configPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        std::ofstream configFile(configPath);
        if (!configFile.is_open()) {
            return false;
        }
        
        configFile << m_config.dump(2);
        configFile.close();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception saving configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigurationManager::ValidateConfiguration() const {
    // Check required sections
    if (!m_config.contains("service") || !m_config.contains("artifacts")) {
        return false;
    }
    
    // Validate resource limits
    if (m_config.contains("service") && m_config["service"].contains("resourceLimits")) {
        auto limits = m_config["service"]["resourceLimits"];
        if (limits.contains("maxMemoryMB")) {
            int maxMem = limits["maxMemoryMB"].get<int>();
            if (maxMem <= 0 || maxMem > 1024) {
                return false;
            }
        }
    }
    
    return true;
}

nlohmann::json ConfigurationManager::GetConfiguration() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_config;
}

ArtifactConfig ConfigurationManager::GetArtifactConfig(ArtifactType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    ArtifactConfig config;
    config.type = type;
    config.enabled = false;
    config.intensity = ArtifactIntensity::Low;
    config.refreshInterval = Duration(3600000); // 1 hour default
    
    if (!m_config.contains("artifacts") || !m_config["artifacts"].contains("modules")) {
        return config;
    }
    
    // Map artifact type to module name
    std::string moduleName;
    switch (type) {
        case ArtifactType::Registry: moduleName = "registry"; break;
        case ArtifactType::FileSystem: moduleName = "filesystem"; break;
        case ArtifactType::Process: moduleName = "processes"; break;
        case ArtifactType::Network: moduleName = "network"; break;
        case ArtifactType::Hardware: moduleName = "hardware"; break;
        default: return config;
    }
    
    auto modules = m_config["artifacts"]["modules"];
    if (!modules.contains(moduleName)) {
        return config;
    }
    
    auto module = modules[moduleName];
    
    // Parse module configuration
    if (module.contains("enabled")) {
        config.enabled = module["enabled"].get<bool>();
    }
    
    if (module.contains("intensity")) {
        config.intensity = ParseIntensity(module["intensity"].get<std::string>());
    }
    
    if (module.contains("refreshInterval")) {
        config.refreshInterval = Duration(module["refreshInterval"].get<int>() * 1000);
    }
    
    if (module.contains("parameters") && module["parameters"].is_object()) {
        for (auto& [key, value] : module["parameters"].items()) {
            if (value.is_string()) {
                config.parameters[key] = value.get<std::string>();
            } else if (value.is_boolean()) {
                config.parameters[key] = value.get<bool>() ? "true" : "false";
            } else if (value.is_number()) {
                config.parameters[key] = std::to_string(value.get<double>());
            }
        }
    }
    
    return config;
}

std::vector<ArtifactConfig> ConfigurationManager::GetEnabledArtifacts() const {
    std::vector<ArtifactConfig> artifacts;
    
    for (auto type : {ArtifactType::Registry, ArtifactType::FileSystem, 
                      ArtifactType::Process, ArtifactType::Network, 
                      ArtifactType::Hardware}) {
        auto config = GetArtifactConfig(type);
        if (config.enabled) {
            artifacts.push_back(config);
        }
    }
    
    return artifacts;
}

ResourceLimits ConfigurationManager::GetResourceLimits() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    ResourceLimits limits;
    limits.maxMemoryMB = Constants::Performance::MAX_MEMORY_USAGE_MB;
    limits.maxCpuPercent = Constants::Performance::MAX_CPU_USAGE_PERCENT;
    limits.maxDiskIOPS = Constants::Performance::MAX_DISK_IO_OPERATIONS;
    limits.maxNetworkBPS = Constants::Performance::MAX_NETWORK_BANDWIDTH_BPS;
    
    if (m_config.contains("service") && m_config["service"].contains("resourceLimits")) {
        auto limitsJson = m_config["service"]["resourceLimits"];
        
        if (limitsJson.contains("maxMemoryMB")) {
            limits.maxMemoryMB = limitsJson["maxMemoryMB"].get<int>();
        }
        if (limitsJson.contains("maxCpuPercent")) {
            limits.maxCpuPercent = limitsJson["maxCpuPercent"].get<double>();
        }
        if (limitsJson.contains("maxDiskIOPS")) {
            limits.maxDiskIOPS = limitsJson["maxDiskIOPS"].get<int>();
        }
        if (limitsJson.contains("maxNetworkBPS")) {
            limits.maxNetworkBPS = limitsJson["maxNetworkBPS"].get<int>();
        }
    }
    
    return limits;
}

int ConfigurationManager::GetUpdateInterval() const {
    return GetValue<int>("service.updateInterval", Constants::Timing::UPDATE_INTERVAL_SECONDS);
}

int ConfigurationManager::GetMonitoringInterval() const {
    return GetValue<int>("service.monitoringInterval", Constants::Timing::MONITORING_INTERVAL_SECONDS);
}

LogLevel ConfigurationManager::GetLogLevel() const {
    std::string level = GetValue<std::string>("service.logLevel", "INFO");
    return ParseLogLevel(level);
}

std::string ConfigurationManager::GetLogFilePath() const {
    return GetValue<std::string>("logging.logFile", Constants::Logging::LOG_FILE_PATH);
}

bool ConfigurationManager::IsEventLogEnabled() const {
    return GetValue<bool>("logging.logToEventLog", true);
}

bool ConfigurationManager::IsConsoleLogEnabled() const {
    return GetValue<bool>("logging.logToConsole", false);
}

template<typename T>
T ConfigurationManager::GetValue(const std::string& path, const T& defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    (void)lock; // Suppress unused variable warning
    
    try {
        auto value = GetValueByPath(path);
        if (value.is_null()) {
            return defaultValue;
        }
        return value.get<T>();
    } catch (...) {
        return defaultValue;
    }
}

// Explicit template instantiations
template int ConfigurationManager::GetValue<int>(const std::string&, const int&) const;
template bool ConfigurationManager::GetValue<bool>(const std::string&, const bool&) const;
template double ConfigurationManager::GetValue<double>(const std::string&, const double&) const;
template std::string ConfigurationManager::GetValue<std::string>(const std::string&, const std::string&) const;

template<typename T>
bool ConfigurationManager::SetValue(const std::string& path, const T& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    (void)lock; // Suppress unused variable warning
    return SetValueByPath(path, json(value));
}

// Explicit template instantiations
template bool ConfigurationManager::SetValue<int>(const std::string&, const int&);
template bool ConfigurationManager::SetValue<bool>(const std::string&, const bool&);
template bool ConfigurationManager::SetValue<double>(const std::string&, const double&);
template bool ConfigurationManager::SetValue<std::string>(const std::string&, const std::string&);

std::string ConfigurationManager::GetConfigFilePath() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_configPath;
}

bool ConfigurationManager::IsLoaded() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_isLoaded;
}

ArtifactType ConfigurationManager::ParseArtifactType(const std::string& typeStr) const {
    if (typeStr == "registry") return ArtifactType::Registry;
    if (typeStr == "filesystem") return ArtifactType::FileSystem;
    if (typeStr == "processes") return ArtifactType::Process;
    if (typeStr == "network") return ArtifactType::Network;
    if (typeStr == "hardware") return ArtifactType::Hardware;
    return ArtifactType::Registry;
}

ArtifactIntensity ConfigurationManager::ParseIntensity(const std::string& intensityStr) const {
    std::string lower = intensityStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "low") return ArtifactIntensity::Low;
    if (lower == "medium") return ArtifactIntensity::Medium;
    if (lower == "high") return ArtifactIntensity::High;
    if (lower == "maximum") return ArtifactIntensity::Maximum;
    
    return ArtifactIntensity::Medium;
}

LogLevel ConfigurationManager::ParseLogLevel(const std::string& levelStr) const {
    std::string upper = levelStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "DEBUG") return LogLevel::Debug;
    if (upper == "INFO") return LogLevel::Info;
    if (upper == "WARNING") return LogLevel::Warning;
    if (upper == "ERROR") return LogLevel::Error;
    if (upper == "CRITICAL") return LogLevel::Critical;
    
    return LogLevel::Info;
}

nlohmann::json ConfigurationManager::GetValueByPath(const std::string& path) const {
    json current = m_config;
    std::istringstream ss(path);
    std::string token;
    
    while (std::getline(ss, token, '.')) {
        if (!current.contains(token)) {
            return json();
        }
        current = current[token];
    }
    
    return current;
}

bool ConfigurationManager::SetValueByPath(const std::string& path, const nlohmann::json& value) {
    try {
        std::istringstream ss(path);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, '.')) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) {
            return false;
        }
        
        json* current = &m_config;
        for (size_t i = 0; i < tokens.size() - 1; ++i) {
            if (!current->contains(tokens[i])) {
                (*current)[tokens[i]] = json::object();
            }
            current = &(*current)[tokens[i]];
        }
        
        (*current)[tokens.back()] = value;
        return true;
        
    } catch (...) {
        return false;
    }
}

void ConfigurationManager::ApplyDefaults() {
    // Apply service defaults
    if (!m_config.contains("service")) {
        m_config["service"] = json::object();
    }
    
    auto& service = m_config["service"];
    if (!service.contains("updateInterval")) {
        service["updateInterval"] = Constants::Timing::UPDATE_INTERVAL_SECONDS;
    }
    if (!service.contains("monitoringInterval")) {
        service["monitoringInterval"] = Constants::Timing::MONITORING_INTERVAL_SECONDS;
    }
    if (!service.contains("logLevel")) {
        service["logLevel"] = "INFO";
    }
    
    // Apply logging defaults
    if (!m_config.contains("logging")) {
        m_config["logging"] = json::object();
    }
    
    auto& logging = m_config["logging"];
    if (!logging.contains("logFile")) {
        logging["logFile"] = Constants::Logging::LOG_FILE_PATH;
    }
    if (!logging.contains("logToEventLog")) {
        logging["logToEventLog"] = true;
    }
    if (!logging.contains("logToConsole")) {
        logging["logToConsole"] = false;
    }
}

bool ConfigurationManager::MergeProfiles(const std::vector<std::string>& profileNames) {
    for (const auto& profileName : profileNames) {
        std::string profilePath;
        
        if (profileName == "basic") {
            profilePath = Constants::BASIC_PROFILE_FILE;
        } else if (profileName == "advanced") {
            profilePath = Constants::ADVANCED_PROFILE_FILE;
        } else {
            profilePath = "config/" + profileName + "_profile.json";
        }
        
        // Resolve relative path
        if (profilePath.find(':') == std::string::npos) {
            char exePath[MAX_PATH];
            if (GetModuleFileNameA(nullptr, exePath, MAX_PATH) > 0) {
                std::string exeDir(exePath);
                size_t lastSlash = exeDir.find_last_of("\\/");
                if (lastSlash != std::string::npos) {
                    profilePath = exeDir.substr(0, lastSlash + 1) + profilePath;
                }
            }
        }
        
        try {
            std::ifstream profileFile(profilePath);
            if (profileFile.is_open()) {
                json profile = json::parse(profileFile);
                m_config.merge_patch(profile);
                profileFile.close();
            }
        } catch (...) {
            // Profile merge failure is non-fatal
        }
    }
    
    return true;
}

} // namespace HoneyPottr
