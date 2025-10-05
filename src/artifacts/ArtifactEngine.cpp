#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include "artifacts/ArtifactEngine.h"
#include "config/ConfigurationManager.h"
#include "logging/Logger.h"
#include "artifacts/modules/RegistryArtifactModule.h"
#include "artifacts/modules/FileSystemArtifactModule.h"
#include "artifacts/modules/ProcessArtifactModule.h"
#include "artifacts/modules/NetworkArtifactModule.h"
#include "artifacts/modules/HardwareArtifactModule.h"

namespace HoneyPottr {

ArtifactEngine::ArtifactEngine()
    : m_configManager(nullptr)
    , m_logger(nullptr)
    , m_isRunning(false)
    , m_shouldStop(false) {
}

ArtifactEngine::~ArtifactEngine() {
    Stop();
    CleanupModules();
}

bool ArtifactEngine::Initialize(ConfigurationManager* configManager, Logger* logger) {
    m_configManager = configManager;
    m_logger = logger;

    if (m_logger) {
        m_logger->Info("Initializing Artifact Engine", "ArtifactEngine");
    }

    // Register all modules
    RegisterModules();

    // Initialize modules based on configuration
    if (!InitializeModules()) {
        if (m_logger) {
            m_logger->Error("Failed to initialize artifact modules", "ArtifactEngine");
        }
        return false;
    }

    if (m_logger) {
        m_logger->Info("Artifact Engine initialized successfully", "ArtifactEngine");
    }

    return true;
}

bool ArtifactEngine::Start() {
    if (m_isRunning) {
        return true;
    }

    if (m_logger) {
        m_logger->Info("Starting Artifact Engine", "ArtifactEngine");
    }

    // Start all enabled modules
    std::lock_guard<std::mutex> lock(m_modulesMutex);
    
    for (auto& [type, module] : m_modules) {
        if (module && module->GetConfig().enabled) {
            if (!module->Start()) {
                if (m_logger) {
                    m_logger->Warning("Failed to start module: " + module->GetModuleName(), 
                                    "ArtifactEngine");
                }
            } else {
                if (m_logger) {
                    m_logger->Info("Started module: " + module->GetModuleName(), 
                                 "ArtifactEngine");
                }
            }
        }
    }

    m_isRunning = true;
    m_shouldStop = false;

    // Start refresh thread
    m_refreshThread = std::thread(&ArtifactEngine::RefreshLoop, this);

    if (m_logger) {
        m_logger->Info("Artifact Engine started successfully", "ArtifactEngine");
    }

    return true;
}

void ArtifactEngine::Stop() {
    if (!m_isRunning) {
        return;
    }

    if (m_logger) {
        m_logger->Info("Stopping Artifact Engine", "ArtifactEngine");
    }

    m_shouldStop = true;

    if (m_refreshThread.joinable()) {
        m_refreshThread.join();
    }

    // Stop all modules
    std::lock_guard<std::mutex> lock(m_modulesMutex);
    
    for (auto& [type, module] : m_modules) {
        if (module && module->IsActive()) {
            module->Stop();
            if (m_logger) {
                m_logger->Info("Stopped module: " + module->GetModuleName(), 
                             "ArtifactEngine");
            }
        }
    }

    m_isRunning = false;

    if (m_logger) {
        m_logger->Info("Artifact Engine stopped", "ArtifactEngine");
    }
}

bool ArtifactEngine::Refresh() {
    if (!m_isRunning) {
        return false;
    }

    if (m_logger) {
        m_logger->Debug("Refreshing artifacts", "ArtifactEngine");
    }

    std::lock_guard<std::mutex> lock(m_modulesMutex);
    (void)lock; // Suppress unused variable warning
    
    bool allSuccess = true;
    for (auto& [type, module] : m_modules) {
        if (module && module->IsActive()) {
            if (!module->Refresh()) {
                allSuccess = false;
                if (m_logger) {
                    m_logger->Warning("Failed to refresh module: " + module->GetModuleName(), 
                                    "ArtifactEngine");
                }
            }
        }
    }

    return allSuccess;
}

bool ArtifactEngine::IsRunning() const {
    return m_isRunning;
}

std::vector<ArtifactType> ArtifactEngine::GetActiveModules() const {
    std::lock_guard<std::mutex> lock(m_modulesMutex);
    (void)lock; // Suppress unused variable warning
    
    std::vector<ArtifactType> activeTypes;
    for (const auto& [type, module] : m_modules) {
        if (module && module->IsActive()) {
            activeTypes.push_back(type);
        }
    }
    
    return activeTypes;
}

IArtifactModule* ArtifactEngine::GetModule(ArtifactType type) const {
    std::lock_guard<std::mutex> lock(m_modulesMutex);
    (void)lock; // Suppress unused variable warning
    
    auto it = m_modules.find(type);
    if (it != m_modules.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

PerformanceMetrics ArtifactEngine::GetMetrics() const {
    PerformanceMetrics combined = {};
    combined.timestamp = std::chrono::steady_clock::now();

    std::lock_guard<std::mutex> lock(m_modulesMutex);
    (void)lock; // Suppress unused variable warning
    
    for (const auto& [type, module] : m_modules) {
        if (module && module->IsActive()) {
            auto metrics = module->GetMetrics();
            combined.cpuUsage += metrics.cpuUsage;
            combined.memoryUsageMB += metrics.memoryUsageMB;
            combined.diskIOPS += metrics.diskIOPS;
            combined.networkBPS += metrics.networkBPS;
        }
    }

    return combined;
}

void ArtifactEngine::RegisterModules() {
    // Create instances of all artifact modules
    m_modules[ArtifactType::Registry] = std::make_unique<RegistryArtifactModule>();
    m_modules[ArtifactType::FileSystem] = std::make_unique<FileSystemArtifactModule>();
    m_modules[ArtifactType::Process] = std::make_unique<ProcessArtifactModule>();
    m_modules[ArtifactType::Network] = std::make_unique<NetworkArtifactModule>();
    m_modules[ArtifactType::Hardware] = std::make_unique<HardwareArtifactModule>();
}

bool ArtifactEngine::InitializeModules() {
    if (!m_configManager) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_modulesMutex);
    (void)lock; // Suppress unused variable warning
    
    for (auto& [type, module] : m_modules) {
        if (module) {
            // Set logger first so module can log during initialization
            module->SetLogger(m_logger);
            
            ArtifactConfig config = m_configManager->GetArtifactConfig(type);
            
            if (!module->Initialize(config)) {
                if (m_logger) {
                    m_logger->Warning("Failed to initialize module: " + module->GetModuleName(), 
                                    "ArtifactEngine");
                }
                // Continue with other modules
            } else {
                if (m_logger && config.enabled) {
                    m_logger->Info("Initialized module: " + module->GetModuleName(), 
                                 "ArtifactEngine");
                }
            }
        }
    }

    return true;
}

void ArtifactEngine::CleanupModules() {
    std::lock_guard<std::mutex> lock(m_modulesMutex);
    (void)lock; // Suppress unused variable warning
    m_modules.clear();
}

void ArtifactEngine::RefreshLoop() {
    while (!m_shouldStop) {
        // Sleep for 1 hour by default (configurable per module)
        for (int i = 0; i < 3600 && !m_shouldStop; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (!m_shouldStop) {
            Refresh();
        }
    }
}

} // namespace HoneyPottr
