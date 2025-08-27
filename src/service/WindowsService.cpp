#include "service/WindowsService.h"
#include <iostream>

namespace HoneyPottr {

// Stub implementation for Windows service wrapper
class WindowsService {
public:
    WindowsService() = default;
    ~WindowsService() = default;
    
    bool Install() {
        std::cout << "WindowsService: Install stub" << std::endl;
        return true;
    }
    
    bool Uninstall() {
        std::cout << "WindowsService: Uninstall stub" << std::endl;
        return true;
    }
    
    bool Start() {
        std::cout << "WindowsService: Start stub" << std::endl;
        return true;
    }
    
    bool Stop() {
        std::cout << "WindowsService: Stop stub" << std::endl;
        return true;
    }
};

} // namespace HoneyPottr
