# HoneyPottr GUI Installer

## Overview

The `HoneyPottr-Installer-v1.0.0.exe` is a professional Windows installer created with NSIS (Nullsoft Scriptable Install System) that provides a complete GUI installation experience for HoneyPottr.

## Features

### 🎯 **Professional GUI Interface**
- Modern Windows installer interface
- Welcome screen with product information
- **Terms of Service agreement** with checkbox confirmation
- Component selection (Core, Service, Desktop Shortcut)
- Installation directory selection
- Progress indication during installation
- Completion screen with option to test the application

### 📋 **Terms of Service Integration**
- **Comprehensive EULA** covering legitimate use, prohibited activities, and legal disclaimers
- **MIT License** attribution to **Calvexa Group, LLC**
- **Checkbox confirmation** required before proceeding
- Clear warnings about system modifications
- Compliance and liability sections

### 🔧 **Installation Components**

**Core Application (Required):**
- HoneyPottr.exe executable
- Configuration files (default, basic, advanced profiles)
- Documentation (README, Installation Guide, Architecture)
- License and Terms of Service files
- Start Menu shortcuts
- Registry entries for proper Windows integration

**Windows Service (Optional, Recommended):**
- Automatic service installation with proper privileges
- Service configured for automatic startup
- Immediate service start after installation
- Error handling with user feedback

**Desktop Shortcut (Optional):**
- Desktop shortcut for console mode testing

### 🛡️ **Security & Compliance**

**Administrator Requirements:**
- Installer requires administrator privileges
- Automatic UAC elevation prompt
- Clear error messages if not running as admin

**System Compatibility:**
- Windows 7 or later required
- 64-bit Windows support
- Automatic architecture detection

**Upgrade Handling:**
- Detects existing installations
- Offers to uninstall previous versions
- Clean upgrade process

### 🗑️ **Professional Uninstaller**

**Complete Removal:**
- Stops and removes Windows service
- Removes all installed files and directories
- Cleans up Start Menu shortcuts and desktop shortcuts
- Removes registry entries
- **Optional artifact cleanup** with user confirmation

**Artifact Cleanup Option:**
- User can choose to remove honeypot artifacts
- Cleans up registry entries created by HoneyPottr
- Removes simulated VM files and directories
- Removes fake analysis tool entries

## Installation Process

### 1. **Welcome Screen**
- Product introduction
- Company attribution (Calvexa Group, LLC)
- MIT License notice

### 2. **Terms of Service**
- Comprehensive EULA display
- Checkbox confirmation required
- Cannot proceed without agreement

### 3. **Component Selection**
- Core Application (required)
- Windows Service (recommended)
- Desktop Shortcut (optional)

### 4. **Installation Directory**
- Default: `C:\Program Files\HoneyPottr`
- User can customize location

### 5. **Installation Progress**
- Real-time progress indication
- Detailed status messages
- Service installation feedback

### 6. **Completion**
- Success confirmation
- Option to test in console mode
- Links to documentation

## Technical Specifications

**Installer Details:**
- **File Size**: 2.6MB (compressed)
- **Format**: PE32 executable (GUI) for Windows
- **Compression**: LZMA compression (21.0% of original size)
- **Architecture**: Intel 80386 compatible (runs on x86 and x64)

**Included Files:**
- HoneyPottr.exe (12.8MB, statically linked)
- Configuration files (JSON format)
- Complete documentation set
- License and legal files

**Registry Integration:**
- Proper Windows application registration
- Uninstaller registry entries
- Add/Remove Programs integration
- File association entries

## Usage Instructions

### For End Users:
1. **Download** `HoneyPottr-Installer-v1.0.0.exe`
2. **Right-click** → "Run as administrator"
3. **Follow** the installation wizard
4. **Read and accept** the Terms of Service
5. **Select components** (recommend keeping defaults)
6. **Complete** installation

### For IT Administrators:
```cmd
# Silent installation (all components)
HoneyPottr-Installer-v1.0.0.exe /S

# Silent installation with specific components
HoneyPottr-Installer-v1.0.0.exe /S /D=C:\CustomPath\HoneyPottr
```

## Legal Information

**Copyright**: © 2024 Calvexa Group, LLC  
**License**: MIT License  
**Distribution**: Free and open source  

**Terms of Service**: Comprehensive EULA included covering:
- Permitted and prohibited uses
- System modification warnings
- Liability limitations
- Compliance requirements
- Indemnification clauses

## Support

The installer includes comprehensive documentation:
- `README.md` - General information and usage
- `INSTALLATION.md` - Detailed installation guide
- `ARCHITECTURE.md` - Technical architecture details
- `TERMS_OF_SERVICE.txt` - Complete legal terms

For technical support or questions about the installer, refer to the included documentation or contact Calvexa Group, LLC.

---

**This installer provides a professional, legally compliant, and user-friendly way to deploy HoneyPottr on Windows systems while ensuring users understand the terms of use and system modifications involved.**
