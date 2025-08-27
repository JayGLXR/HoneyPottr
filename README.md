# HoneyPottr 

**A lightweight honeypot artifact generator that deceives malware into self-termination**
![044E9D9A-3EE2-4782-8DA5-842329BAD797](https://github.com/user-attachments/assets/5e59efc9-081d-431a-91d7-f69d39d54b7c)


[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Windows](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Version: 1.0.0](https://img.shields.io/badge/Version-1.0.0-green.svg)](https://github.com/JayGLXR/HoneyPottr/releases)

## **What is HoneyPottr?**

HoneyPottr is a cybersecurity deception tool that creates honeypot artifacts on Windows systems to trick malware into believing it's running in a sandbox or analysis environment. When sophisticated malware performs environment checks to avoid detection, it encounters these artifacts and self-terminates, protecting your system from infection.

### **The Problem**
Modern malware often includes sandbox detection mechanisms to avoid analysis by security researchers. These "evasive" malware samples check for:
- Virtual machine indicators (VMware, VirtualBox)
- Analysis tools (Wireshark, Process Monitor)
- Honeypot environments
- Debugging tools

When detected, the malware simply exits without deploying its payload, making it harder to analyze but also harder to infect real systems.

### **The Solution**
HoneyPottr flips this defensive mechanism into a protective feature by making your real system *appear* to be a honeypot or analysis environment. The malware performs its checks, finds the fake artifacts, and deletes itself - protecting your system without any user intervention.

## **How It Works**

HoneyPottr operates as an invisible Windows service that continuously maintains realistic honeypot artifacts:

### **Registry Artifacts**
- VMware Tools installation entries
- VirtualBox Guest Additions registry keys  
- Sandboxie configuration entries
- Analysis tool installation records

### **File System Artifacts**
- Fake VMware and VirtualBox executables
- Dummy analysis tool binaries
- Simulated driver files
- Honeypot-specific directories

### **Process & Service Artifacts**
- Virtual machine service entries
- Simulated analysis tool processes
- Fake system services

### **Hardware Artifacts**
- Virtual machine BIOS signatures
- Fake hardware device entries
- VM-specific system information

## **Key Features**

- **🔍 Invisible Operation**: Runs completely in the background with no visible interface
- **⚡ Lightweight**: < 50MB RAM usage, < 1% CPU utilization
- **🔄 Automatic**: Set-and-forget operation with automatic startup
- **🛠️ Configurable**: Multiple artifact profiles (Basic, Advanced, Custom)
- **🏢 Enterprise Ready**: Professional installer with Terms of Service
- **🔒 Secure**: Runs with minimal privileges, digitally signed
- **📊 Monitoring**: Built-in performance monitoring and resource limits

## **Installation**

### **Quick Install (Recommended)**

1. **Download** the latest installer: [HoneyPottr-Installer-v1.0.0-FINAL.exe](https://github.com/JayGLXR/HoneyPottr/releases)
2. **Right-click** → "Run as administrator"
3. **Accept** the Terms of Service
4. **Complete** the installation wizard
5. **Done!** HoneyPottr starts automatically and runs invisibly

### **System Requirements**
- Windows 10/11 or Windows Server 2016+ (x64)
- Administrator privileges for installation
- 100MB available disk space
- 50MB available RAM

### **Verification**
Check if HoneyPottr is running:
```powershell
Get-Service -Name "HoneyPottr"
```

Verify artifacts are being created:
```cmd
reg query "HKLM\SOFTWARE\VMware, Inc.\VMware Tools"
```

## 🔧 **Configuration**

HoneyPottr includes three pre-configured profiles:

### **Basic Profile** (Default)
- Essential VM artifacts for common malware detection
- Minimal system impact
- Suitable for most users

### **Advanced Profile**
- Comprehensive artifact set for sophisticated malware
- Additional analysis tool indicators
- Enhanced deception capabilities

### **Custom Profile**
- User-defined artifact configurations
- Granular control over individual components
- Advanced users and researchers

Configuration files are located in: `C:\Program Files\HoneyPottr\config\`

## **Usage**

### **Normal Operation**
HoneyPottr operates completely automatically. Once installed:
- ✅ Starts with Windows
- ✅ Runs invisibly in background
- ✅ Maintains artifacts continuously
- ✅ Requires no user interaction

### **Management Commands**
```cmd
# Check service status
sc query HoneyPottr

# Manual service control
net start HoneyPottr
net stop HoneyPottr

# Test in console mode
HoneyPottr.exe --console

# Uninstall
HoneyPottr.exe --uninstall
```

## **Technical Details**

### **Architecture**
- **Language**: C++20 with native Windows APIs
- **Service Framework**: Windows Service Control Manager
- **Configuration**: JSON-based with hot-reload support
- **Logging**: File and Windows Event Log integration
- **Updates**: Secure configuration and artifact updates

### **Artifact Generation**
- **Modular Design**: Plugin-based artifact modules
- **Dynamic Updates**: Artifacts refresh automatically
- **Realistic Simulation**: Authentic-looking fake entries
- **Performance Optimized**: Minimal resource consumption

### **Security**
- **Minimal Privileges**: Runs with least required permissions
- **Code Signing**: Digitally signed executable (when distributed)
- **Audit Logging**: Comprehensive operation logging
- **Isolation**: Sandboxed execution where possible

## **Future Roadmap**

HoneyPottr is actively developed with exciting features planned:

### **Version 2.0 (Coming Soon)**
- **AI-Powered Artifacts**: Machine learning-based artifact generation
- **Network Deception**: Fake network services and honeypots
- **Dynamic Adaptation**: Real-time artifact adjustment based on threats

### **Cross-Platform Expansion**
- **Linux Version**: Native Linux honeypot artifact generator
- **Container Support**: Docker and Kubernetes deployment
- ☁**Cloud Integration**: AWS, Azure, GCP native implementations
- 🏗**Infrastructure as Code**: Terraform and Ansible modules


## **Contributing**

We welcome contributions from the cybersecurity community!

### **Development Setup**
```bash
# Clone the repository
git clone https://github.com/JayGLXR/HoneyPottr.git
cd HoneyPottr

# Build requirements
# - Visual Studio 2019+ with C++20 support
# - CMake 3.20+
# - Windows SDK

# Build
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

## 📜 **Legal & Compliance**

### **License**
HoneyPottr is released under the [MIT License](LICENSE) - free for personal and commercial use.

### **Responsible Use**
- **Legitimate Security**: Use for protecting your own systems
- **Research**: Cybersecurity research and education
- **Enterprise Defense**: Corporate security implementations
- **Unauthorized Systems**: Do not install on systems you don't own
- **Malicious Use**: Do not use for illegal activities

### **Disclaimer**
HoneyPottr creates simulated artifacts and does not introduce actual vulnerabilities. Users are responsible for compliance with applicable laws and regulations in their jurisdiction.

## **Support**

## 🏆 **Recognition**

HoneyPottr represents a novel approach to endpoint protection through deception technology. By turning malware's own evasion techniques against itself, it provides an innovative layer of defense that complements traditional security solutions.

---

To learn more about our cutting edge technology, visit IRISC2.com. 
