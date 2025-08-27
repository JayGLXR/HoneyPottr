#include "monitoring/ResourceMonitor.h"
#include <iostream>

namespace HoneyPottr {
// Stub implementation for ResourceMonitor
class ResourceMonitor {
public:
    ResourceMonitor() = default;
    ~ResourceMonitor() = default;
    
    bool Initialize() {
        std::cout << "ResourceMonitor: Initialize stub" << std::endl;
        return true;
    }
    
    PerformanceMetrics GetMetrics() const {
        PerformanceMetrics metrics = {};
        return metrics;
    }
};
}
