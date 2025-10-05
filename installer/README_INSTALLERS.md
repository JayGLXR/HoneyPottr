# HoneyPottr Installer Documentation

## Available Installers

HoneyPottr provides three installer options for Windows deployment:

### 1. WiX Toolset Installer (MSI) - **RECOMMENDED**

**File**: `HoneyPottr.wxs`  
**Output**: `HoneyPottr-v1.0.0-x64.msi`  
**Type**: Windows Installer Package (MSI)

**Features**:
- ✅ Professional MSI installer
- ✅ Integrated with Windows Installer
- ✅ Group Policy deployment support
- ✅ Unattended installation support
- ✅ Component-based installation
- ✅ Automatic rollback on failure
- ✅ Add/Remove Programs integration

**Building**:
```batch
cd installer
candle HoneyPottr.wxs -ext WixUtilExtension
light HoneyPottr.wixobj -ext WixUtilExtension -out HoneyPottr.msi
```

**Installation**:
```batch
:: Interactive
msiexec /i HoneyPottr.msi

:: Silent/Unattended
msiexec /i HoneyPottr.msi /quiet /norestart /log install.log

:: With parameters
msiexec /i HoneyPottr.msi INSTALLFOLDER="C:\HoneyPottr" /quiet
```

**Uninstallation**:
```batch
:: Interactive
msiexec /x HoneyPottr.msi

:: Silent
msiexec /x HoneyPottr.msi /quiet /norestart
```

---

### 2. NSIS Installer (EXE)

**File**: `HoneyPottr-Installer.nsi`  
**Output**: `HoneyPottr-Setup-v1.0.0-x64.exe`  
**Type**: Self-contained executable installer

**Features**:
- ✅ Single executable installer
- ✅ Modern UI with wizard
- ✅ Customizable installation
- ✅ Component selection
- ✅ Silent installation support
- ✅ Smaller download size

**Building**:
```batch
cd installer
makensis HoneyPottr-Installer.nsi
```

**Installation**:
```batch
:: Interactive
HoneyPottr-Setup-v1.0.0-x64.exe

:: Silent
HoneyPottr-Setup-v1.0.0-x64.exe /S

:: Silent with log
HoneyPottr-Setup-v1.0.0-x64.exe /S /D=C:\HoneyPottr
```

**Uninstallation**:
```batch
:: From installation directory
C:\Program Files\HoneyPottr\uninst.exe

:: Silent
C:\Program Files\HoneyPottr\uninst.exe /S
```

---

### 3. Simplified NSIS Installer

**File**: `HoneyPottr-Simple.nsi`  
**Output**: `HoneyPottr-Simple-Setup.exe`  
**Type**: Basic executable installer

**Features**:
- ✅ Minimal installer
- ✅ Quick installation
- ✅ No component selection
- ✅ Fastest installation

---

## Installation Components

All installers include:

### Core Components (Required)
- `HoneyPottr.exe` - Main service executable
- `config/default_config.json` - Default configuration
- `config/basic_profile.json` - Basic profile
- `config/advanced_profile.json` - Advanced profile
- Documentation files (README, LICENSE, etc.)

### Windows Service (Required)
- Service registration with LocalSystem account
- Automatic startup (delayed)
- Failure recovery configuration (3 restart attempts)
- Dependencies: RPC, WMI, TCP/IP

### Shortcuts (Optional)
- Start Menu folder with:
  - Console mode launcher
  - Configuration editor
  - Log viewer
  - Service manager
  - Documentation
  - Uninstaller
- Desktop shortcuts (optional)

### Event Log Registration (Optional)
- Windows Event Log source registration
- Enables logging to Application log
- Event types: Information, Warning, Error

---

## Installer Features

### System Requirements Check
- ✅ Windows 10 or later (64-bit)
- ✅ Administrator privileges
- ✅ 64-bit operating system
- ✅ Sufficient disk space (50 MB)

### Service Configuration
- **Account**: LocalSystem
- **Startup**: Automatic (Delayed)
- **Recovery**: Restart on failure (5s, 10s, 30s delays)
- **Dependencies**: RPC, WMI, TCP/IP
- **SID Type**: Unrestricted

### File Permissions
- **Logs Directory**: Full access for SYSTEM and Administrators
- **Config Directory**: Protected, not overwritten on upgrade
- **Install Directory**: Standard Program Files permissions

### Registry Entries
Created in:
- `HKLM\SOFTWARE\HoneyPottr` - Application settings
- `HKLM\SYSTEM\CurrentControlSet\Services\HoneyPottr` - Service configuration
- `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\HoneyPottr` - Uninstall info

---

## Silent Installation

### MSI Silent Install
```batch
msiexec /i HoneyPottr.msi /quiet /norestart INSTALLFOLDER="C:\HoneyPottr" /log "%TEMP%\honeypottr-install.log"
```

### NSIS Silent Install
```batch
HoneyPottr-Setup-v1.0.0-x64.exe /S /D=C:\HoneyPottr
```

### Parameters
- `/quiet` or `/S` - Silent mode (no UI)
- `/norestart` - Don't restart computer
- `/log <file>` - Log installation to file
- `INSTALLFOLDER=<path>` or `/D=<path>` - Custom install location

---

## Unattended Deployment

### Group Policy Deployment (MSI only)
1. Copy `HoneyPottr.msi` to network share
2. Open Group Policy Management
3. Computer Configuration → Software Settings → Software Installation
4. Right-click → New → Package
5. Select `HoneyPottr.msi`
6. Choose "Assigned" deployment

### SCCM Deployment
1. Import `HoneyPottr.msi` as application
2. Create deployment package
3. Deploy to target collection
4. Monitor deployment status

### PowerShell Deployment
```powershell
# Install on remote computers
$computers = Get-Content "computers.txt"
foreach ($computer in $computers) {
    Copy-Item "HoneyPottr.msi" "\\$computer\C$\Temp\"
    Invoke-Command -ComputerName $computer -ScriptBlock {
        msiexec /i "C:\Temp\HoneyPottr.msi" /quiet /norestart
    }
}
```

---

## Upgrade Process

### From Previous Version
1. **Automatic**: New MSI detects old version and upgrades
2. **Manual**: Uninstall old, install new (preserves config)

### Configuration Preservation
- ✅ Configuration files NOT overwritten during upgrade
- ✅ Logs preserved
- ✅ Service settings migrated

---

## Uninstallation Options

### Standard Uninstall
- From Add/Remove Programs (Settings → Apps)
- Or from Start Menu → HoneyPottr → Uninstall
- Or run `uninst.exe` from install directory

### Complete Cleanup
During uninstallation, you'll be prompted to:
1. **Keep or remove configuration files** (custom settings)
2. **Keep or remove log files** (forensic analysis)
3. **Remove honeypot artifacts** (registry keys, files created)

### Artifact Cleanup
If you choose to remove artifacts, the uninstaller will delete:

**Registry Keys:**
- `HKLM\SOFTWARE\VMware, Inc.\VMware Tools`
- `HKLM\SOFTWARE\Oracle\VirtualBox Guest Additions`
- `HKLM\SOFTWARE\Sandboxie`
- `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Wireshark`
- `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Process Monitor`
- `HKLM\SYSTEM\CurrentControlSet\Services\VBoxService`
- `HKLM\SYSTEM\CurrentControlSet\Services\VMTools`

**Files & Directories:**
- `C:\Program Files\VMware\VMware Tools\`
- `C:\Program Files\Oracle\VirtualBox Guest Additions\`
- `C:\Program Files\Sandboxie\`
- `C:\Tools\`
- `C:\ProgramData\SecurityTools\`
- `C:\Windows\System32\drivers\VBoxMouse.sys`
- `C:\Windows\System32\drivers\vmhgfs.sys`
- Desktop shortcuts for Wireshark

---

## Building Installers

### Prerequisites
- **For WiX**: WiX Toolset 3.11+ or 4.0+
- **For NSIS**: NSIS 3.08+
- **For Both**: HoneyPottr.exe built in `build\bin\Release\`

### WiX Build Process
```batch
cd installer

:: Compile
candle HoneyPottr.wxs -ext WixUtilExtension -ext WixUIExtension

:: Link
light HoneyPottr.wixobj -ext WixUtilExtension -ext WixUIExtension ^
  -out HoneyPottr-v1.0.0-x64.msi

:: Or use CPack (from CMake)
cd ..\build
cpack -G WIX -C Release
```

### NSIS Build Process
```batch
cd installer

:: Compile installer
makensis HoneyPottr-Installer.nsi

:: Or use /V4 for verbose output
makensis /V4 HoneyPottr-Installer.nsi
```

### Automated Build (CMake/CPack)
```batch
cd build
cmake --build . --config Release
cpack -G "WIX;NSIS" -C Release
```

This creates both MSI and EXE installers automatically.

---

## Installer Customization

### Modify Installation Path
Edit the installer scripts:

**WiX**:
```xml
<Directory Id="ProgramFilesFolder">
  <Directory Id="INSTALLFOLDER" Name="HoneyPottr">
```

**NSIS**:
```nsis
InstallDir "$PROGRAMFILES64\HoneyPottr"
```

### Change Service Account
**WiX**:
```xml
<ServiceInstall ... Account="NetworkService" Password="[PASSWORD]" />
```

**NSIS**:
```nsis
nsExec::ExecToLog 'sc config HoneyPottr obj= "NT AUTHORITY\NetworkService"'
```

### Add Custom Files
**WiX**:
```xml
<Component Id="CustomFiles" Guid="*">
  <File Id="CustomFile" Source="path\to\file.txt" />
</Component>
```

**NSIS**:
```nsis
File "path\to\file.txt"
```

---

## Troubleshooting

### "Administrator privileges required"
**Solution**: Right-click installer → Run as Administrator

### "Service installation failed"
**Solutions**:
1. Check if service already exists: `sc query HoneyPottr`
2. Remove manually: `sc delete HoneyPottr`
3. Reinstall

### "Windows version not supported"
**Solution**: Requires Windows 10+ 64-bit. Upgrade your OS.

### "File in use" errors
**Solutions**:
1. Stop HoneyPottr service: `sc stop HoneyPottr`
2. Close any running instances
3. Restart and try again

### Installer logs
- **WiX/MSI**: `%TEMP%\MSI*.log`
- **NSIS**: Installation directory

---

## Advanced Options

### MSI Command-Line Options
```batch
:: Repair installation
msiexec /f HoneyPottr.msi

:: Administrative installation (extract files)
msiexec /a HoneyPottr.msi TARGETDIR=C:\ExtractedMSI

:: Uninstall by product code
msiexec /x {PRODUCT-CODE-GUID}

:: Enable logging
msiexec /i HoneyPottr.msi /l*v install.log
```

### NSIS Command-Line Options
```batch
:: Silent with custom directory
HoneyPottr-Setup.exe /S /D=C:\Custom\Path

:: Silent without desktop shortcuts
HoneyPottr-Setup.exe /S /NODESKTOP

:: Silent without starting service
HoneyPottr-Setup.exe /S /NOSERVICE
```

---

## Production Deployment Checklist

Before deploying to production:

- [ ] Build HoneyPottr.exe in Release configuration
- [ ] Test executable in console mode
- [ ] Review and customize configuration files
- [ ] Build installer (WiX or NSIS)
- [ ] Test installer on clean Windows VM
- [ ] Verify service starts automatically
- [ ] Check log file creation
- [ ] Verify artifacts are created
- [ ] Test uninstallation
- [ ] Document any customizations
- [ ] Sign installer (optional, recommended for enterprise)

---

## Code Signing (Recommended)

For enterprise deployment, sign the installer:

### Sign Executable
```batch
signtool sign /f certificate.pfx /p password /t http://timestamp.digicert.com HoneyPottr.exe
```

### Sign MSI
```batch
signtool sign /f certificate.pfx /p password /d "HoneyPottr" /t http://timestamp.digicert.com HoneyPottr.msi
```

### Sign NSIS EXE
```batch
signtool sign /f certificate.pfx /p password HoneyPottr-Setup.exe
```

---

## Support

For installer issues:
1. Check this documentation
2. Review installer logs
3. Test on clean Windows 10/11 VM
4. Verify all prerequisites installed
5. Check GitHub Issues

---

## File Manifest

Files included in all installers:

```
HoneyPottr/
├── HoneyPottr.exe                # Main service executable
├── config/
│   ├── default_config.json       # Default configuration  
│   ├── basic_profile.json        # Basic profile
│   └── advanced_profile.json     # Advanced profile
├── logs/                         # Log directory (created empty)
├── README.md                     # Project documentation
├── INSTALLATION.md               # Installation guide
├── PRODUCTION_SUMMARY.md         # Production deployment guide
├── BUILD_INSTRUCTIONS.md         # Build instructions
├── FINAL_STATUS.md               # Project status
├── LICENSE                       # MIT License
├── TERMS_OF_SERVICE.txt          # Terms of service
└── uninst.exe                    # Uninstaller (NSIS only)
```

Start Menu shortcuts:
- HoneyPottr Console Mode
- Configuration Editor
- View Logs
- Manage Service
- Documentation
- Uninstall

---

## Version History

### v1.0.0 (2025-10-05) - Production Release
- Complete implementation of all modules
- Production-grade logging and monitoring
- Comprehensive artifact generation
- Resource limit enforcement
- Multi-profile support
- Full service integration
- Event Log support
- Automatic failure recovery

---

*Last Updated: October 5, 2025*  
*Installer Version: 1.0.0*  
*Target Platform: Windows 10/11 x64*

