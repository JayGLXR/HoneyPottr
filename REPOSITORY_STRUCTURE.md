# HoneyPottr Repository Structure

This document describes the structure of the HoneyPottr repository and what each directory/file contains.

## 📁 Root Directory

```
HoneyPottr/
├── CMakeLists.txt              # Main CMake build configuration
├── LICENSE                     # MIT License
├── README.md                   # Project overview and quick start
├── INSTALLATION.md             # Installation instructions
├── TERMS_OF_SERVICE.txt        # Terms of service
├── .gitignore                  # Git ignore rules
├── Dockerfile                  # Docker cross-compilation environment
├── docker-compose.yml          # Docker Compose configuration
├── build.sh                    # Automated build script (macOS/Linux)
└── .github/                    # GitHub Actions CI/CD
    └── workflows/
        └── windows-build.yml   # Automatic Windows build workflow
```

## 📖 Documentation Files

```
├── BUILD_INSTRUCTIONS.md       # How to build on different platforms
├── BUILD_SUMMARY.md            # Build method comparisons
├── COMPLETE_STATUS.md          # Project completion status
├── CROSS_COMPILE_STATUS.md     # Cross-compilation analysis
├── FINAL_STATUS.md             # Final production readiness report
├── PRODUCTION_SUMMARY.md       # Production deployment guide
└── REPOSITORY_STRUCTURE.md     # This file
```

**For Users**: Start with `README.md`, then `INSTALLATION.md`, then `PRODUCTION_SUMMARY.md`

## 💻 Source Code

```
src/
├── main.cpp                    # Application entry point
├── artifacts/
│   ├── ArtifactEngine.cpp      # Main artifact orchestration engine
│   └── modules/                # Individual artifact generators
│       ├── RegistryArtifactModule.cpp
│       ├── FileSystemArtifactModule.cpp
│       ├── ProcessArtifactModule.cpp
│       ├── NetworkArtifactModule.cpp
│       └── HardwareArtifactModule.cpp
├── config/
│   └── ConfigurationManager.cpp    # JSON configuration management
├── logging/
│   └── Logger.cpp                  # Async logging system
├── monitoring/
│   └── ResourceMonitor.cpp         # Resource usage monitoring
├── service/
│   ├── ServiceController.cpp       # Main service controller
│   └── WindowsService.cpp          # Windows service wrapper
└── utils/
    ├── WindowsUtils.cpp            # Windows-specific utilities
    └── SecurityUtils.cpp           # Security and crypto utilities
```

## 📋 Header Files

```
include/
├── artifacts/
│   ├── ArtifactEngine.h
│   ├── IArtifactModule.h       # Artifact module interface
│   └── modules/
│       ├── RegistryArtifactModule.h
│       ├── FileSystemArtifactModule.h
│       ├── ProcessArtifactModule.h
│       ├── NetworkArtifactModule.h
│       └── HardwareArtifactModule.h
├── common/
│   ├── Constants.h             # Application constants
│   └── Types.h                 # Common type definitions
├── config/
│   └── ConfigurationManager.h
├── logging/
│   └── Logger.h
├── monitoring/
│   └── ResourceMonitor.h
├── service/
│   ├── ServiceController.h
│   └── WindowsService.h
└── utils/
    ├── WindowsUtils.h
    └── SecurityUtils.h
```

## ⚙️ Configuration Files

```
config/
├── default_config.json         # Default application configuration
├── basic_profile.json          # Minimal footprint profile
└── advanced_profile.json       # Maximum coverage profile
```

**Note**: These are templates. Actual configuration should be edited after installation.

## 📦 Installer Files

```
installer/
├── HoneyPottr.wxs              # WiX Toolset installer definition (MSI)
├── HoneyPottr-Installer.nsi    # NSIS installer script (EXE)
├── HoneyPottr-Simple.nsi       # Simplified NSIS installer
├── admin_privileges.xml        # Elevation manifest for MSI
├── README_INSTALLERS.md        # Installer documentation
└── INSTALLER_CHANGELOG.md      # Installer version history
```

## 🎨 Resources

```
resources/
├── honeypottr.ico              # Application icon
├── honeypottr-16.png           # Icon 16x16
├── honeypottr-32.png           # Icon 32x32
├── honeypottr-48.png           # Icon 48x48
├── honeypottr-64.png           # Icon 64x64
├── honeypottr-128.png          # Icon 128x128
├── honeypottr-256.png          # Icon 256x256
├── HoneyPottr.rc               # Resource script
├── resource.h                  # Resource header
└── HoneyPottr.manifest         # Application manifest
```

## 🚫 Excluded from Repository (.gitignore)

The following are **NOT** included in the repository:

- `build/` - Build output directory
- `*.exe`, `*.dll`, `*.obj` - Compiled binaries
- `*.msi` - Installer packages (built on demand)
- `logs/` - Runtime log files
- `.vs/`, `.vscode/` - IDE settings
- `CMakeCache.txt`, `CMakeFiles/` - CMake artifacts
- `.DS_Store` - macOS system files
- `docker-build.log` - Build logs

**Why**: These are generated during build and should not be version controlled.

## 📊 File Counts by Type

```
Source Files (.cpp):        16 files, ~4,200 lines
Header Files (.h):          15 files, ~2,000 lines
Configuration (.json):       3 files, ~400 lines
Installers (.wxs, .nsi):     3 files, ~900 lines
Documentation (.md):        10 files, ~2,000 lines
Build Scripts:               3 files, ~300 lines
Resources:                  10 files
─────────────────────────────────────────────────
Total:                      60 files, ~9,800 lines
```

## 🎯 User-Relevant Files

### For End Users (Installing HoneyPottr)
- `README.md` - Start here
- `INSTALLATION.md` - How to install
- `config/*.json` - Configuration templates
- Installer files (built from GitHub Actions)

### For Developers (Building/Modifying)
- All source files (`src/`, `include/`)
- `CMakeLists.txt` - Build configuration
- `BUILD_INSTRUCTIONS.md` - How to build
- Build scripts (`Dockerfile`, `build.sh`, etc.)

### For DevOps (Deploying)
- `PRODUCTION_SUMMARY.md` - Deployment guide
- `installer/` - Installer definitions
- `.github/workflows/` - CI/CD pipelines

## 🔄 Build Outputs (Not in Repo)

These are generated during builds:

```
build/                          # Created by CMake
├── bin/
│   └── Release/
│       └── HoneyPottr.exe      # Compiled executable
├── CMakeFiles/                 # CMake cache
└── *.msi, *.exe                # Installer packages

logs/                           # Created at runtime
└── honeypottr.log              # Application logs
```

## 📝 Important Notes

### What to Commit
✅ Source code (`.cpp`, `.h`)
✅ Configuration templates (`.json`)
✅ Build scripts (`CMakeLists.txt`, `.sh`)
✅ Documentation (`.md`)
✅ Installer definitions (`.wxs`, `.nsi`)
✅ Resources (`.ico`, `.png`, `.rc`)

### What NOT to Commit
❌ Build artifacts (`build/`, `*.exe`, `*.dll`)
❌ Log files (`*.log`, `logs/`)
❌ IDE settings (`.vs/`, `.vscode/`)
❌ OS files (`.DS_Store`, `Thumbs.db`)
❌ Installer packages (`*.msi`)
❌ Temporary files (`*.tmp`, `*.bak`)

### Special Cases
⚠️ `HoneyPottr Installer.zip` - Removed (old artifact)
⚠️ `docker-build.log` - Removed (build artifact)
⚠️ Random images - Removed (not needed)

## 🚀 Clean Repository

After cleanup, the repository contains only:
- Source code
- Build configuration
- Documentation
- Resources
- Installer definitions

All build artifacts and temporary files are excluded via `.gitignore`.

## 🔍 Verification

To verify repository cleanliness:

```bash
# Check what will be committed
git status

# Check what's being ignored
git status --ignored

# See all tracked files
git ls-files
```

## 📦 Release Artifacts

Release artifacts (installers, executables) are published via:
- **GitHub Releases**: Tagged versions with downloadable installers
- **GitHub Actions Artifacts**: Automatic builds from CI/CD
- **Not in repository**: Built on-demand from source

## 🎓 Best Practices Applied

1. ✅ **Separate source from build** - Build directory ignored
2. ✅ **Version control source only** - No binaries in repo
3. ✅ **Document everything** - Comprehensive README files
4. ✅ **Clean structure** - Organized by component type
5. ✅ **Professional .gitignore** - Excludes common artifacts
6. ✅ **CI/CD ready** - GitHub Actions configured
7. ✅ **User-focused** - Clear separation of user vs developer files

---

*Last Updated: October 5, 2025*  
*Repository Version: 1.0.0*  
*Purpose: Production-grade honeypot artifact generator*
