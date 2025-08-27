#include "artifacts/modules/RegistryArtifactModule.h"
#include <iostream>

namespace HoneyPottr {

RegistryArtifactModule::RegistryArtifactModule() : m_isActive(false), m_operationCount(0) {}
RegistryArtifactModule::~RegistryArtifactModule() = default;

bool RegistryArtifactModule::Initialize(const ArtifactConfig& config) {
    m_config = config;
    std::cout << "RegistryArtifactModule: Initialize stub" << std::endl;
    return true;
}

bool RegistryArtifactModule::Start() {
    m_isActive = true;
    std::cout << "RegistryArtifactModule: Start stub" << std::endl;
    return true;
}

bool RegistryArtifactModule::Stop() {
    m_isActive = false;
    std::cout << "RegistryArtifactModule: Stop stub" << std::endl;
    return true;
}

bool RegistryArtifactModule::Refresh() { return true; }
ArtifactType RegistryArtifactModule::GetType() const { return ArtifactType::Registry; }
bool RegistryArtifactModule::IsActive() const { return m_isActive; }
const ArtifactConfig& RegistryArtifactModule::GetConfig() const { return m_config; }
bool RegistryArtifactModule::UpdateConfig(const ArtifactConfig& config) { m_config = config; return true; }
StringVector RegistryArtifactModule::GetActiveArtifacts() const { return {}; }
PerformanceMetrics RegistryArtifactModule::GetMetrics() const { return {}; }
bool RegistryArtifactModule::ValidateEnvironment() const { return true; }
std::string RegistryArtifactModule::GetModuleName() const { return "RegistryArtifactModule"; }
std::string RegistryArtifactModule::GetVersion() const { return "1.0.0"; }
void RegistryArtifactModule::LogMessage(LogLevel /*level*/, const std::string& /*message*/) const {}
bool RegistryArtifactModule::ShouldContinueOperation() const { return true; }

} // namespace HoneyPottr
