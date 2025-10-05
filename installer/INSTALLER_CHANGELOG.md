# HoneyPottr Installer Changelog

## Version 1.0.0 (2025-10-05) - Production Release

### New Features
- ✅ Complete WiX MSI installer with advanced features
- ✅ Complete NSIS EXE installer with modern UI
- ✅ Automatic Windows service installation
- ✅ Service failure recovery configuration (3 restart attempts)
- ✅ Event Log source registration
- ✅ Multi-profile installation (basic, advanced)
- ✅ Start Menu shortcuts (Console, Config, Logs, Service, Docs)
- ✅ Desktop shortcuts (optional)
- ✅ Unattended/silent installation support
- ✅ Group Policy deployment support (MSI)
- ✅ Configuration file preservation during upgrade
- ✅ Log file preservation options
- ✅ Honeypot artifact cleanup during uninstall
- ✅ 64-bit only installation (security)
- ✅ Windows 10+ requirement enforcement

### Components Installed
1. **HoneyPottr.exe** (Release build, ~2-5 MB)
2. **Configuration Files**:
   - default_config.json
   - basic_profile.json
   - advanced_profile.json
3. **Documentation** (7 files):
   - README.md
   - INSTALLATION.md
   - LICENSE
   - TERMS_OF_SERVICE.txt
   - PRODUCTION_SUMMARY.md
   - BUILD_INSTRUCTIONS.md
   - FINAL_STATUS.md
4. **Logs Directory** (empty, with proper permissions)
5. **Start Menu Shortcuts** (6 items)
6. **Desktop Shortcuts** (2 items, optional)

### Service Configuration
- **Name**: HoneyPottr
- **Display Name**: HoneyPottr Honeypot Artifact Generator
- **Description**: Production-grade honeypot service
- **Account**: LocalSystem
- **Startup Type**: Automatic (Delayed)
- **Recovery**: Restart on failure (5s, 10s, 30s)
- **Dependencies**: RPC, WMI, TCP/IP
- **SID Type**: Unrestricted

### Registry Entries
- `HKLM\SOFTWARE\HoneyPottr` - Application settings
  - InstallPath
  - Version
  - ConfigPath
  - LogPath
- `HKLM\SYSTEM\CurrentControlSet\Services\HoneyPottr` - Service config
- `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\HoneyPottr` - Uninstall info
- `HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\HoneyPottr` - Event Log

### Permissions Set
- **Logs Directory**: SYSTEM (Full), Administrators (Full), LocalService (RW)
- **Config Directory**: Protected, not overwritten on upgrade
- **Install Directory**: Standard Program Files permissions
- **Service Registry**: SYSTEM and Administrators only

### System Checks
- ✅ Administrator privileges required
- ✅ Windows 10 or later (64-bit) required
- ✅ 64-bit architecture required
- ✅ Existing installation detection
- ✅ Service conflict detection

### Uninstall Features
- ✅ Service stop and removal
- ✅ File cleanup with preservation options
- ✅ Config preservation prompt
- ✅ Log preservation prompt
- ✅ Honeypot artifact cleanup prompt (detailed)
- ✅ Registry cleanup
- ✅ Shortcut removal
- ✅ Complete uninstallation

### Silent Installation Support
**MSI**:
```batch
msiexec /i HoneyPottr.msi /quiet /norestart /log install.log
```

**NSIS**:
```batch
HoneyPottr-Setup-v1.0.0-x64.exe /S
```

### Deployment Methods
1. **Interactive**: Standard UI-based installation
2. **Silent**: Command-line automated installation
3. **Group Policy**: MSI deployment via AD
4. **SCCM**: Enterprise software deployment
5. **PowerShell**: Remote deployment scripts

---

## Technical Details

### MSI Properties
- Platform: x64
- Installer Version: 500
- Install Scope: perMachine
- Privileges: elevated
- Compression: Cabinet (embedded)

### NSIS Properties
- Platform: x64
- Unicode: Yes
- Compression: LZMA (solid)
- Execution Level: admin
- UI: Modern UI 2

### File Sizes
- MSI Installer: ~5-8 MB
- NSIS Installer: ~3-6 MB
- Installed Size: ~10 MB

---

## Upgrade Path

### From v1.0.0 to Future Versions
- Configuration files preserved
- Logs preserved
- Service settings migrated
- Artifacts maintained (unless reconfigured)
- Automatic version detection
- Rollback support (MSI only)

---

## Known Limitations

1. **Windows Only**: No Linux/macOS support (by design)
2. **64-bit Only**: No 32-bit support (security requirement)
3. **Admin Required**: Cannot install without elevation
4. **Service Account**: Must run as LocalSystem for full API access
5. **Windows 10+**: Earlier versions not supported

---

## Future Enhancements

Planned for future versions:
- [ ] Code signing integration
- [ ] Update checker in installer
- [ ] Configuration wizard
- [ ] Advanced service configuration UI
- [ ] Remote deployment PowerShell module
- [ ] Docker Windows container support
- [ ] Chocolatey package
- [ ] WinGet package manifest

---

*Installer Version: 1.0.0*  
*Last Updated: October 5, 2025*  
*Maintainer: HoneyPottr Project*

