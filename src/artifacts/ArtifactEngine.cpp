#include "artifacts/ArtifactEngine.h"
#include <iostream>

namespace HoneyPottr {

class ArtifactEngine {
public:
    ArtifactEngine() = default;
    ~ArtifactEngine() = default;
    
    bool Initialize() {
        std::cout << "ArtifactEngine: Initialize stub" << std::endl;
        return true;
    }
    
    bool Start() {
        std::cout << "ArtifactEngine: Start stub" << std::endl;
        return true;
    }
    
    bool Stop() {
        std::cout << "ArtifactEngine: Stop stub" << std::endl;
        return true;
    }
};

} // namespace HoneyPottr
