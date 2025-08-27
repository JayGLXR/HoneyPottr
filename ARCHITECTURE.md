# HoneyPottr Architecture Documentation

## Overview
HoneyPottr is a lightweight C++ Windows service designed to run continuously in the background, creating system artifacts that mimic honeypot environments. When malware performs sandbox detection checks, it will encounter these artifacts and self-terminate, believing it's running in a honeypot.

## Core Architecture

### 1. Main Service Controller
- **Purpose**: Central orchestrator managing all subsystems
- **Responsibilities**:
  - Service lifecycle management (start, stop, pause, resume)
  - Module initialization and coordination
  - Error handling and recovery
  - Configuration hot-reloading

### 2. Configuration Manager
- **Purpose**: Manages application configuration and artifact profiles
- **Features**:
  - JSON-based configuration files
  - Multiple artifact profiles (basic, advanced, custom)
  - Dynamic configuration updates without service restart
  - Encrypted sensitive configuration data

### 3. Artifact Engine
- **Purpose**: Core system responsible for generating and maintaining honeypot artifacts
- **Architecture**: Modular plugin-based system
- **Modules**:

#### Registry Artifact Module
Creates registry keys and values commonly checked by malware:
- Sandbox detection keys (VMware, VirtualBox, Sandboxie indicators)
- Analysis tool registry entries
- Fake security software entries
- Honeypot-specific registry artifacts

#### File System Artifact Module
Generates filesystem artifacts:
- Fake analysis tools and their configuration files
- Honeypot-specific directories and files
- Decoy documents with honeytokens
- Virtual machine guest additions files
- Sandbox-specific file structures

#### Process Artifact Module
Simulates processes and services:
- Fake analysis tool processes
- Honeypot monitoring services
- Virtual machine guest services
- Security research tool processes

#### Network Artifact Module
Creates network-based indicators:
- Fake network interfaces
- Honeypot-specific network configurations
- Simulated analysis tool network activity
- Fake VPN/proxy configurations

#### Hardware Artifact Module
Simulates hardware characteristics:
- Virtual machine hardware signatures
- Fake hardware device entries
- Sandbox-specific hardware configurations
- Analysis environment hardware profiles

### 4. Resource Monitor
- **Purpose**: Ensures minimal system impact
- **Features**:
  - Real-time CPU usage monitoring
  - Memory usage tracking and optimization
  - Disk I/O monitoring
  - Automatic resource throttling
  - Performance metrics collection

### 5. Logging System
- **Purpose**: Comprehensive logging and monitoring
- **Features**:
  - Structured logging with multiple levels
  - Performance metrics tracking
  - Security event logging
  - Configurable log rotation
  - Optional remote logging support

## Technical Specifications

### Development Stack
- **Language**: C++17/20
- **Frameworks**: 
  - Windows SDK for native API access
  - C++/WinRT for modern Windows API integration
  - JSON for Modern C++ for configuration parsing
- **Build System**: CMake with MSVC compiler
- **Service Framework**: Windows Service Control Manager (SCM)

### Performance Requirements
- **Memory Usage**: < 50MB resident memory
- **CPU Usage**: < 1% average CPU utilization
- **Disk I/O**: Minimal, primarily during initialization
- **Network Impact**: Negligible, only for configuration updates

### Security Considerations
- **Privilege Level**: Runs with minimal required privileges
- **Code Signing**: Digitally signed executable
- **Configuration Security**: Encrypted sensitive configuration data
- **Isolation**: Sandboxed execution where possible
- **Update Security**: Signed configuration and artifact updates

## Artifact Categories

### 1. Registry Artifacts
```
HKEY_LOCAL_MACHINE\SOFTWARE\VMware, Inc.\VMware Tools
HKEY_LOCAL_MACHINE\SOFTWARE\Oracle\VirtualBox Guest Additions
HKEY_LOCAL_MACHINE\SOFTWARE\Sandboxie
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{Wireshark}
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\VBoxService
```

### 2. File System Artifacts
```
C:\Program Files\VMware\VMware Tools\
C:\Program Files\Oracle\VirtualBox Guest Additions\
C:\Windows\System32\drivers\VBoxMouse.sys
C:\Windows\System32\drivers\vmhgfs.sys
C:\Users\Public\Desktop\Wireshark.lnk
C:\Tools\ProcessMonitor.exe
```

### 3. Process Artifacts
```
vmtoolsd.exe (VMware Tools Service)
VBoxService.exe (VirtualBox Service)
procmon.exe (Process Monitor)
wireshark.exe (Network Analyzer)
sandboxie.exe (Sandboxie)
```

### 4. Network Artifacts
```
Virtual network adapters with honeypot-specific MAC addresses
Fake network interfaces (VMware, VirtualBox)
Simulated analysis tool network connections
Honeypot-specific network configurations
```

### 5. Hardware Artifacts
```
Virtual machine hardware signatures
Fake hardware device entries in Device Manager
Sandbox-specific hardware configurations
Analysis environment hardware profiles
```

## Deployment Architecture

### Installation
- MSI installer package
- Automatic service registration
- Configuration file deployment
- Initial artifact generation

### Service Management
- Windows Service Control Manager integration
- Automatic startup configuration
- Service recovery options
- Remote management capabilities

### Updates
- Automatic configuration updates
- Artifact profile updates
- Secure update mechanism
- Rollback capabilities

## Configuration System

### Configuration File Structure
```json
{
  "service": {
    "updateInterval": 3600,
    "logLevel": "INFO",
    "resourceLimits": {
      "maxMemoryMB": 50,
      "maxCpuPercent": 1
    }
  },
  "artifacts": {
    "profiles": ["basic", "advanced"],
    "modules": {
      "registry": { "enabled": true, "intensity": "medium" },
      "filesystem": { "enabled": true, "intensity": "high" },
      "processes": { "enabled": true, "intensity": "low" },
      "network": { "enabled": false },
      "hardware": { "enabled": true, "intensity": "medium" }
    }
  }
}
```

### Artifact Profiles
- **Basic**: Minimal artifacts for basic malware detection
- **Advanced**: Comprehensive artifacts for sophisticated malware
- **Custom**: User-defined artifact sets
- **Research**: Artifacts for specific research scenarios

## Monitoring and Maintenance

### Health Monitoring
- Service health checks
- Artifact integrity verification
- Resource usage monitoring
- Performance metrics collection

### Maintenance Tasks
- Artifact refresh and rotation
- Configuration updates
- Log rotation and cleanup
- Performance optimization

### Alerting
- Service failure notifications
- Resource usage alerts
- Security event notifications
- Update failure alerts

## Security Model

### Threat Model
- **Insider Threats**: Minimal privilege execution
- **External Attacks**: Signed code and secure updates
- **Malware Analysis**: Artifact authenticity and believability
- **System Compromise**: Isolation and containment

### Security Controls
- Code signing and verification
- Encrypted configuration storage
- Secure communication channels
- Audit logging and monitoring
- Regular security updates

## Future Enhancements

### Planned Features
- Machine learning-based artifact adaptation
- Cloud-based threat intelligence integration
- Advanced behavioral simulation
- Multi-platform support (Linux, macOS)
- Integration with SIEM systems

### Research Areas
- Dynamic artifact generation based on current threat landscape
- Behavioral analysis of malware detection techniques
- Advanced evasion technique simulation
- Collaborative threat intelligence sharing

## Conclusion

HoneyPottr provides a robust, lightweight solution for creating convincing honeypot artifacts that deceive malware into self-termination. The modular architecture ensures maintainability and extensibility while the focus on performance guarantees minimal system impact during continuous operation.
