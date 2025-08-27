#include "config/ConfigurationManager.h"
#include <iostream>

namespace HoneyPottr {

class ConfigurationManager {
public:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;
    
    bool LoadConfiguration(const std::string& configPath) {
        std::cout << "ConfigurationManager: Loading config from " << configPath << std::endl;
        return true;
    }
    
    bool SaveConfiguration(const std::string& configPath) {
        std::cout << "ConfigurationManager: Saving config to " << configPath << std::endl;
        return true;
    }
};

} // namespace HoneyPottr
