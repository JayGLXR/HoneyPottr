#pragma once

#include "common/Types.h"
#include <string>
#include <vector>
#include <memory>

namespace HoneyPottr {

/**
 * @brief Interface for artifact generation modules
 * 
 * This interface defines the contract for all artifact generation modules.
 * Each module is responsible for creating and maintaining specific types
 * of honeypot artifacts (registry, files, processes, etc.).
 */
class IArtifactModule {
public:
    virtual ~IArtifactModule() = default;

    /**
     * @brief Initialize the artifact module
     * @param config Configuration parameters for this module
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool Initialize(const ArtifactConfig& config) = 0;

    /**
     * @brief Start generating artifacts
     * @return true if artifacts were successfully created, false otherwise
     */
    virtual bool Start() = 0;

    /**
     * @brief Stop generating artifacts and clean up
     * @return true if cleanup succeeded, false otherwise
     */
    virtual bool Stop() = 0;

    /**
     * @brief Refresh/update existing artifacts
     * @return true if refresh succeeded, false otherwise
     */
    virtual bool Refresh() = 0;

    /**
     * @brief Get the type of artifacts this module generates
     * @return The artifact type
     */
    virtual ArtifactType GetType() const = 0;

    /**
     * @brief Get the current status of the module
     * @return true if the module is active and generating artifacts
     */
    virtual bool IsActive() const = 0;

    /**
     * @brief Get the current configuration
     * @return The current artifact configuration
     */
    virtual const ArtifactConfig& GetConfig() const = 0;

    /**
     * @brief Update the module configuration
     * @param config New configuration parameters
     * @return true if configuration was successfully updated
     */
    virtual bool UpdateConfig(const ArtifactConfig& config) = 0;

    /**
     * @brief Get a list of artifacts currently being maintained
     * @return Vector of artifact descriptions
     */
    virtual StringVector GetActiveArtifacts() const = 0;

    /**
     * @brief Get performance metrics for this module
     * @return Performance metrics structure
     */
    virtual PerformanceMetrics GetMetrics() const = 0;

    /**
     * @brief Validate that the module can operate in the current environment
     * @return true if the module can operate, false otherwise
     */
    virtual bool ValidateEnvironment() const = 0;

    /**
     * @brief Get the module name for logging and identification
     * @return Module name string
     */
    virtual std::string GetModuleName() const = 0;

    /**
     * @brief Get the module version
     * @return Version string
     */
    virtual std::string GetVersion() const = 0;

protected:
    /**
     * @brief Log a message from this module
     * @param level Log level
     * @param message Message to log
     */
    virtual void LogMessage(LogLevel level, const std::string& message) const = 0;

    /**
     * @brief Check if the module should continue operating based on resource constraints
     * @return true if the module should continue, false if it should throttle
     */
    virtual bool ShouldContinueOperation() const = 0;
};

/**
 * @brief Factory function type for creating artifact modules
 */
using ArtifactModuleFactory = std::function<std::unique_ptr<IArtifactModule>()>;

/**
 * @brief Registry for artifact module factories
 */
class ArtifactModuleRegistry {
public:
    /**
     * @brief Register a factory for an artifact module type
     * @param type The artifact type this factory creates
     * @param factory Factory function
     */
    static void RegisterFactory(ArtifactType type, ArtifactModuleFactory factory);

    /**
     * @brief Create an artifact module of the specified type
     * @param type The artifact type to create
     * @return Unique pointer to the created module, or nullptr if type not registered
     */
    static std::unique_ptr<IArtifactModule> CreateModule(ArtifactType type);

    /**
     * @brief Get all registered artifact types
     * @return Vector of registered artifact types
     */
    static std::vector<ArtifactType> GetRegisteredTypes();

private:
    static std::map<ArtifactType, ArtifactModuleFactory> s_factories;
};

} // namespace HoneyPottr
