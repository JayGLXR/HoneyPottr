#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include "common/Types.h"

namespace HoneyPottr {

/**
 * @brief Configuration manager for loading and managing application settings
 * 
 * This class handles loading, validation, and access to configuration settings
 * from JSON files. It supports hot-reloading and configuration profiles.
 */
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();

    // Non-copyable, non-movable
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) = delete;
    ConfigurationManager& operator=(ConfigurationManager&&) = delete;

    /**
     * @brief Load configuration from a JSON file
     * @param configPath Path to the configuration file
     * @return true if configuration was loaded successfully
     */
    bool LoadConfiguration(const std::string& configPath);

    /**
     * @brief Reload configuration from the previously loaded file
     * @return true if configuration was reloaded successfully
     */
    bool ReloadConfiguration();

    /**
     * @brief Save current configuration to a file
     * @param configPath Path to save the configuration
     * @return true if configuration was saved successfully
     */
    bool SaveConfiguration(const std::string& configPath);

    /**
     * @brief Validate the current configuration
     * @return true if configuration is valid
     */
    bool ValidateConfiguration() const;

    /**
     * @brief Get the entire configuration as JSON
     * @return JSON object containing all configuration
     */
    nlohmann::json GetConfiguration() const;

    /**
     * @brief Get artifact configuration for a specific module
     * @param type Artifact type
     * @return Artifact configuration structure
     */
    ArtifactConfig GetArtifactConfig(ArtifactType type) const;

    /**
     * @brief Get all enabled artifact configurations
     * @return Vector of artifact configurations
     */
    std::vector<ArtifactConfig> GetEnabledArtifacts() const;

    /**
     * @brief Get resource limits configuration
     * @return Resource limits structure
     */
    ResourceLimits GetResourceLimits() const;

    /**
     * @brief Get service update interval in seconds
     * @return Update interval
     */
    int GetUpdateInterval() const;

    /**
     * @brief Get monitoring interval in seconds
     * @return Monitoring interval
     */
    int GetMonitoringInterval() const;

    /**
     * @brief Get log level from configuration
     * @return Log level enumeration
     */
    LogLevel GetLogLevel() const;

    /**
     * @brief Get log file path
     * @return Path to log file
     */
    std::string GetLogFilePath() const;

    /**
     * @brief Check if event logging is enabled
     * @return true if event logging is enabled
     */
    bool IsEventLogEnabled() const;

    /**
     * @brief Check if console logging is enabled
     * @return true if console logging is enabled
     */
    bool IsConsoleLogEnabled() const;

    /**
     * @brief Get a configuration value by path
     * @param path JSON path (e.g., "service.name")
     * @param defaultValue Default value if path doesn't exist
     * @return Configuration value or default
     */
    template<typename T>
    T GetValue(const std::string& path, const T& defaultValue) const;

    /**
     * @brief Set a configuration value by path
     * @param path JSON path
     * @param value Value to set
     * @return true if value was set successfully
     */
    template<typename T>
    bool SetValue(const std::string& path, const T& value);

    /**
     * @brief Get the path of the currently loaded configuration file
     * @return Configuration file path
     */
    std::string GetConfigFilePath() const;

    /**
     * @brief Check if configuration has been loaded
     * @return true if configuration is loaded
     */
    bool IsLoaded() const;

private:
    /**
     * @brief Parse artifact type from string
     * @param typeStr Type string (e.g., "registry")
     * @return Artifact type enum
     */
    ArtifactType ParseArtifactType(const std::string& typeStr) const;

    /**
     * @brief Parse artifact intensity from string
     * @param intensityStr Intensity string (e.g., "medium")
     * @return Artifact intensity enum
     */
    ArtifactIntensity ParseIntensity(const std::string& intensityStr) const;

    /**
     * @brief Parse log level from string
     * @param levelStr Log level string (e.g., "INFO")
     * @return Log level enum
     */
    LogLevel ParseLogLevel(const std::string& levelStr) const;

    /**
     * @brief Get a nested JSON value by path
     * @param path Dot-separated path
     * @return JSON value or null if not found
     */
    nlohmann::json GetValueByPath(const std::string& path) const;

    /**
     * @brief Set a nested JSON value by path
     * @param path Dot-separated path
     * @param value Value to set
     * @return true if value was set
     */
    bool SetValueByPath(const std::string& path, const nlohmann::json& value);

    /**
     * @brief Apply default values for missing configuration keys
     */
    void ApplyDefaults();

    /**
     * @brief Merge configuration with profile files
     * @param profileNames List of profile names to merge
     * @return true if merge succeeded
     */
    bool MergeProfiles(const std::vector<std::string>& profileNames);

    // Configuration data
    nlohmann::json m_config;
    std::string m_configPath;
    bool m_isLoaded;

    // Synchronization
    mutable std::mutex m_mutex;
};

} // namespace HoneyPottr
