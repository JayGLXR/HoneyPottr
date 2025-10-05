#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include "service/ServiceController.h"
#include "config/ConfigurationManager.h"
#include "logging/Logger.h"
#include "monitoring/ResourceMonitor.h"
#include "artifacts/ArtifactEngine.h"
#include "common/Constants.h"

namespace HoneyPottr {

ServiceController::ServiceController() 
    : m_configManager(nullptr)
    , m_artifactEngine(nullptr)
    , m_resourceMonitor(nullptr)
    , m_logger(nullptr)
    , m_state(ServiceState::Stopped)
    , m_shouldStop(false)
    , m_lastMaintenanceTime(std::chrono::steady_clock::now())
    , m_lastMetricsTime(std::chrono::steady_clock::now()) {
}

ServiceController::~ServiceController() {
    Stop();
    ShutdownSubsystems();
}

bool ServiceController::Initialize(const std::string& configPath) {
    m_configPath = configPath.empty() ? Constants::DEFAULT_CONFIG_FILE : configPath;
    
    std::cout << "ServiceController: Initializing with config: " << m_configPath << std::endl;
    
    // Initialize all subsystems
    if (!InitializeSubsystems()) {
        std::cerr << "ServiceController: Failed to initialize subsystems" << std::endl;
        return false;
    }
    
    UpdateState(ServiceState::Stopped);
    return true;
}

bool ServiceController::Start() {
    if (m_state == ServiceState::Running) {
        return true;
    }
    
    UpdateState(ServiceState::Starting);
    
    if (m_logger) {
        m_logger->Info("Starting HoneyPottr service", "ServiceController");
    }
    
    // Start resource monitoring
    if (m_resourceMonitor) {
        m_resourceMonitor->Start();
    }
    
    // Start artifact engine
    if (m_artifactEngine) {
        if (!m_artifactEngine->Start()) {
            if (m_logger) {
                m_logger->Error("Failed to start artifact engine", "ServiceController");
            }
            return false;
        }
    }
    
    UpdateState(ServiceState::Running);
    
    // Start service loop in separate thread
    m_shouldStop = false;
    m_serviceThread = std::thread(&ServiceController::ServiceLoop, this);
    
    if (m_logger) {
        m_logger->Info("HoneyPottr service started successfully", "ServiceController");
    }
    
    return true;
}

bool ServiceController::Stop() {
    if (m_state == ServiceState::Stopped) {
        return true;
    }
    
    UpdateState(ServiceState::Stopping);
    
    if (m_logger) {
        m_logger->Info("Stopping HoneyPottr service", "ServiceController");
    }
    
    m_shouldStop = true;
    
    if (m_serviceThread.joinable()) {
        m_serviceThread.join();
    }
    
    // Stop all subsystems
    if (m_artifactEngine) {
        m_artifactEngine->Stop();
    }
    
    if (m_resourceMonitor) {
        m_resourceMonitor->Stop();
    }
    
    UpdateState(ServiceState::Stopped);
    
    if (m_logger) {
        m_logger->Info("HoneyPottr service stopped", "ServiceController");
    }
    
    return true;
}

bool ServiceController::Pause() {
    if (m_state != ServiceState::Running) {
        return false;
    }
    
    UpdateState(ServiceState::Paused);
    std::cout << "ServiceController: Service paused" << std::endl;
    return true;
}

bool ServiceController::Resume() {
    if (m_state != ServiceState::Paused) {
        return false;
    }
    
    UpdateState(ServiceState::Running);
    std::cout << "ServiceController: Service resumed" << std::endl;
    return true;
}

ServiceState ServiceController::GetState() const {
    return m_state.load();
}

bool ServiceController::IsRunning() const {
    return m_state.load() == ServiceState::Running;
}

bool ServiceController::ReloadConfiguration() {
    if (m_logger) {
        m_logger->Info("Reloading configuration", "ServiceController");
    }
    
    if (m_configManager) {
        if (!m_configManager->ReloadConfiguration()) {
            if (m_logger) {
                m_logger->Error("Failed to reload configuration", "ServiceController");
            }
            return false;
        }
    }
    
    // Restart artifact engine with new configuration
    if (m_state == ServiceState::Running && m_artifactEngine) {
        m_artifactEngine->Stop();
        if (!m_artifactEngine->Initialize(m_configManager, m_logger)) {
            if (m_logger) {
                m_logger->Error("Failed to reinitialize artifact engine", "ServiceController");
            }
            return false;
        }
        m_artifactEngine->Start();
    }
    
    if (m_logger) {
        m_logger->Info("Configuration reloaded successfully", "ServiceController");
    }
    
    return true;
}

PerformanceMetrics ServiceController::GetMetrics() const {
    PerformanceMetrics metrics = {};
    metrics.timestamp = std::chrono::steady_clock::now();
    
    if (m_resourceMonitor) {
        metrics = m_resourceMonitor->GetMetrics();
    }
    
    return metrics;
}

void ServiceController::SetStateCallback(ServiceStateCallback callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    (void)lock; // Suppress unused variable warning
    m_stateCallback = callback;
}

void ServiceController::SetErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    (void)lock; // Suppress unused variable warning
    m_errorCallback = callback;
}

void ServiceController::SetMetricsCallback(MetricsCallback callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    (void)lock; // Suppress unused variable warning
    m_metricsCallback = callback;
}

ConfigurationManager* ServiceController::GetConfigurationManager() const {
    return m_configManager;
}

ArtifactEngine* ServiceController::GetArtifactEngine() const {
    return m_artifactEngine;
}

ResourceMonitor* ServiceController::GetResourceMonitor() const {
    return m_resourceMonitor;
}

Logger* ServiceController::GetLogger() const {
    return m_logger;
}

void ServiceController::ServiceLoop() {
    std::cout << "ServiceController: Service loop started" << std::endl;
    
    while (!m_shouldStop) {
        if (m_state == ServiceState::Running) {
            PerformMaintenance();
            CheckResourceUsage();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "ServiceController: Service loop ended" << std::endl;
}

bool ServiceController::InitializeSubsystems() {
    try {
        // Initialize Configuration Manager
        m_configManager = new ConfigurationManager();
        if (!m_configManager->LoadConfiguration(m_configPath)) {
            std::cerr << "Failed to load configuration" << std::endl;
            return false;
        }
        
        // Initialize Logger
        m_logger = new Logger();
        if (!m_logger->Initialize(
            m_configManager->GetLogFilePath(),
            m_configManager->GetLogLevel(),
            10, // max file size MB
            5,  // max log files
            m_configManager->IsEventLogEnabled(),
            m_configManager->IsConsoleLogEnabled()
        )) {
            std::cerr << "Failed to initialize logger" << std::endl;
            return false;
        }
        
        m_logger->Info("HoneyPottr service initializing", "ServiceController");
        
        // Initialize Resource Monitor
        m_resourceMonitor = new ResourceMonitor();
        if (!m_resourceMonitor->Initialize(m_configManager->GetResourceLimits(), m_logger)) {
            m_logger->Error("Failed to initialize resource monitor", "ServiceController");
            return false;
        }
        
        // Initialize Artifact Engine
        m_artifactEngine = new ArtifactEngine();
        if (!m_artifactEngine->Initialize(m_configManager, m_logger)) {
            m_logger->Error("Failed to initialize artifact engine", "ServiceController");
            return false;
        }
        
        m_logger->Info("All subsystems initialized successfully", "ServiceController");
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

void ServiceController::ShutdownSubsystems() {
    if (m_artifactEngine) {
        delete m_artifactEngine;
        m_artifactEngine = nullptr;
    }
    
    if (m_resourceMonitor) {
        delete m_resourceMonitor;
        m_resourceMonitor = nullptr;
    }
    
    if (m_logger) {
        m_logger->Info("Shutting down HoneyPottr service", "ServiceController");
        m_logger->Shutdown();
        delete m_logger;
        m_logger = nullptr;
    }
    
    if (m_configManager) {
        delete m_configManager;
        m_configManager = nullptr;
    }
}

void ServiceController::UpdateState(ServiceState newState) {
    ServiceState oldState = m_state.exchange(newState);
    
    if (oldState != newState) {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        (void)lock; // Suppress unused variable warning
        if (m_stateCallback) {
            m_stateCallback(newState);
        }
    }
}

void ServiceController::HandleError(const std::string& error) {
    std::cerr << "ServiceController Error: " << error << std::endl;
    
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    (void)lock; // Suppress unused variable warning
    if (m_errorCallback) {
        m_errorCallback(error);
    }
}

void ServiceController::PerformMaintenance() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastMaintenance = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastMaintenanceTime);
    
    if (timeSinceLastMaintenance.count() >= 300) { // 5 minutes
        if (m_logger) {
            m_logger->Debug("Performing maintenance tasks", "ServiceController");
        }
        
        // Refresh artifacts
        if (m_artifactEngine && m_artifactEngine->IsRunning()) {
            m_artifactEngine->Refresh();
        }
        
        // Check for configuration changes
        // In a production system, you might watch for file changes
        
        m_lastMaintenanceTime = now;
    }
}

void ServiceController::CheckResourceUsage() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastMetrics = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastMetricsTime);
    
    if (timeSinceLastMetrics.count() >= 30) { // 30 seconds
        PerformanceMetrics metrics = GetMetrics();
        
        // Check if resource limits are exceeded
        if (m_resourceMonitor && m_resourceMonitor->IsResourceLimitExceeded()) {
            if (m_logger) {
                m_logger->Warning("Resource limits exceeded", "ServiceController");
            }
            
            // Could implement throttling here
        }
        
        {
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            (void)lock; // Suppress unused variable warning
            if (m_metricsCallback) {
                m_metricsCallback(metrics);
            }
        }
        
        m_lastMetricsTime = now;
    }
}

} // namespace HoneyPottr
