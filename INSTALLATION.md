# HoneyPottr Installation Guide

## Prerequisites

### System Requirements
- **Operating System**: Windows 10/11 or Windows Server 2016+
- **Architecture**: x64 (64-bit)
- **RAM**: Minimum 100MB available (HoneyPottr uses < 50MB)
- **Disk Space**: 50MB for installation
- **Privileges**: Administrator privileges required for installation

### Development Requirements (for building from source)
- Visual Studio 2019+ with C++17/20 support
- CMake 3.20+
- WiX Toolset 3.11+ (for MSI installer creation)
- vcpkg or Conan (for dependency management)

## Installation Methods

### Method 1: MSI Installer (Recommended)

1. **Download the MSI installer** from the releases page
2. **Right-click** the MSI file and select **"Run as administrator"**
3. Follow the installation wizard
4. The service will be **automatically installed and started**

**What the MSI installer does:**
- ✅ Installs HoneyPottr to `C:\Program Files\HoneyPottr\`
- ✅ Creates Windows service with proper permissions
- ✅ Configures automatic startup (delayed)
- ✅ Sets up service recovery (automatic restart on failure)
- ✅ Creates configuration files with default settings
- ✅ Configures proper file and registry permissions
- ✅ Adds Start Menu shortcuts

### Method 2: Manual Installation

If you prefer manual installation or are building from source:

#### Step 1: Build the Application
```cmd
# Clone repository
git clone <repository-url>
cd HoneyPottr

# Create build directory
mkdir build
cd build

# Configure with CMake (Release build)
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Create installer (optional)
cpack
```

#### Step 2: Install as Service
```cmd
# Open Command Prompt as Administrator
# Navigate to the build output directory
cd build\bin\Release

# Install the service
HoneyPottr.exe --install

# Start the service
net start HoneyPottr
```

## Service Configuration

### Service Properties
- **Service Name**: `HoneyPottr`
- **Display Name**: `HoneyPottr Honeypot Artifact Generator`
- **Account**: `LocalSystem` (required for registry/file operations)
- **Startup Type**: `Automatic (Delayed Start)`
- **Recovery**: Automatic restart on failure

### Required Permissions

The service requires the following permissions to function properly:

#### Registry Permissions
- **HKEY_LOCAL_MACHINE\SOFTWARE** - Create/modify keys for VM artifacts
- **HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services** - Create service entries

#### File System Permissions
- **C:\Program Files\** - Create VM tool directories and files
- **C:\Windows\System32\drivers\** - Create dummy driver files
- **Installation directory** - Read configuration, write logs

#### System Permissions
- **Service Control Manager** - Register and manage simulated services
- **WMI (Windows Management Instrumentation)** - Create hardware artifacts
- **Process enumeration** - Simulate processes in task lists

### Why LocalSystem Account?

HoneyPottr runs as `LocalSystem` because it needs to:
1. **Create registry keys** in HKEY_LOCAL_MACHINE
2. **Create files** in system directories (Program Files, System32)
3. **Simulate system services** and processes
4. **Access WMI** for hardware artifact generation
5. **Run continuously** without user login

This is similar to how legitimate system services (Windows Update, Windows Defender, etc.) operate.

## Verification

### Check Service Status
```cmd
# Check if service is running
sc query HoneyPottr

# View service configuration
sc qc HoneyPottr

# Check service startup type
sc qc HoneyPottr | findstr START_TYPE
```

### Check Generated Artifacts
```cmd
# Check for VMware registry artifacts
reg query "HKLM\SOFTWARE\VMware, Inc.\VMware Tools" 2>nul && echo "VMware artifacts found" || echo "VMware artifacts not found"

# Check for VirtualBox registry artifacts  
reg query "HKLM\SOFTWARE\Oracle\VirtualBox Guest Additions" 2>nul && echo "VirtualBox artifacts found" || echo "VirtualBox artifacts not found"

# Check for file artifacts
dir "C:\Program Files\VMware\VMware Tools\" 2>nul && echo "VMware files found" || echo "VMware files not found"
```

### View Logs
```cmd
# View service logs
type "C:\Program Files\HoneyPottr\logs\honeypottr.log"

# View Windows Event Log
eventvwr.msc
# Navigate to: Windows Logs > Application > Filter by Source: "HoneyPottr"
```

## Startup Behavior

### Automatic Startup
- ✅ **Starts automatically** when Windows boots
- ✅ **Delayed start** - waits for system to fully initialize
- ✅ **Runs without user login** - operates as system service
- ✅ **Survives user logoff/logon** - continues running
- ✅ **Automatic restart** on failure or crash

### Service Dependencies
The service waits for these Windows services to start first:
- **RPC Service (RpcSs)** - Required for registry operations
- **Windows Management Instrumentation (Winmgmt)** - Required for WMI artifacts

### Boot Process Integration
1. **Windows boots** and loads core services
2. **Network becomes available** (delayed start trigger)
3. **HoneyPottr service starts** automatically
4. **Artifacts are generated** within 30 seconds
5. **Service runs continuously** until shutdown

## Troubleshooting

### Installation Issues

**Error: "Failed to open Service Control Manager"**
- **Cause**: Not running as Administrator
- **Solution**: Right-click Command Prompt → "Run as administrator"

**Error: "Access denied" during installation**
- **Cause**: Insufficient privileges or antivirus interference
- **Solution**: Temporarily disable antivirus, run as Administrator

**Error: "Service already exists"**
- **Cause**: Previous installation not fully removed
- **Solution**: Run `HoneyPottr.exe --uninstall` first

### Runtime Issues

**Service fails to start**
- Check Windows Event Log for error details
- Verify configuration file syntax: `config\default_config.json`
- Ensure no port conflicts (if network module enabled)

**Artifacts not being created**
- Check service is running: `sc query HoneyPottr`
- Review logs: `C:\Program Files\HoneyPottr\logs\honeypottr.log`
- Verify module configuration in config file

**High resource usage**
- Check configuration limits in `default_config.json`
- Reduce artifact intensity levels
- Disable unused modules

### Service Management

```cmd
# Stop service
net stop HoneyPottr

# Start service  
net start HoneyPottr

# Restart service
net stop HoneyPottr && net start HoneyPottr

# Check service status
sc query HoneyPottr

# View service configuration
sc qc HoneyPottr
```

## Uninstallation

### Method 1: MSI Uninstaller
1. Open **Programs and Features** (appwiz.cpl)
2. Find **"HoneyPottr"** in the list
3. Click **"Uninstall"**
4. Follow the uninstall wizard

### Method 2: Manual Uninstallation
```cmd
# Stop and remove service (as Administrator)
net stop HoneyPottr
HoneyPottr.exe --uninstall

# Remove installation directory
rmdir /s "C:\Program Files\HoneyPottr"

# Clean up artifacts (optional)
reg delete "HKLM\SOFTWARE\VMware, Inc.\VMware Tools" /f 2>nul
reg delete "HKLM\SOFTWARE\Oracle\VirtualBox Guest Additions" /f 2>nul
```

## Security Considerations

### Legitimate Use
- HoneyPottr is designed for **legitimate security research** and **protection**
- Ensure compliance with **local laws and regulations**
- Use only on **systems you own or have permission** to modify

### Antivirus Detection
- Some antivirus software may flag HoneyPottr as suspicious
- This is expected behavior due to its honeypot simulation capabilities
- Add installation directory to antivirus exclusions if needed
- The application is **digitally signed** for authenticity verification

### Network Security
- HoneyPottr operates **locally only** by default
- No network communication unless explicitly configured
- All artifacts are **simulated** - no actual vulnerabilities created

## Support

For installation issues:
1. Check this installation guide
2. Review the troubleshooting section
3. Check Windows Event Log for error details
4. Create an issue on GitHub with error details and system information
