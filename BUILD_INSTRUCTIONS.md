# HoneyPottr Build Instructions

## Table of Contents
- [Building on Windows (Native)](#building-on-windows-native)
- [Cross-Compilation from macOS/Linux](#cross-compilation-from-macoslinux)
- [Using GitHub Actions](#using-github-actions)
- [Docker Build](#docker-build)
- [Troubleshooting](#troubleshooting)

---

## Building on Windows (Native)

### Prerequisites
- **Windows 10/11** (64-bit)
- **Visual Studio 2019 or 2022** with "Desktop development with C++" workload
- **CMake 3.20 or later**
- **Git**

### Build Steps

#### Option 1: Using Visual Studio

1. **Clone the repository:**
   ```cmd
   git clone https://github.com/yourusername/HoneyPottr.git
   cd HoneyPottr
   ```

2. **Generate Visual Studio solution:**
   ```cmd
   mkdir build
   cd build
   cmake -G "Visual Studio 17 2022" -A x64 ..
   ```

3. **Build:**
   ```cmd
   cmake --build . --config Release
   ```
   Or open `HoneyPottr.sln` in Visual Studio and build from IDE.

4. **Find the executable:**
   ```cmd
   build\bin\Release\HoneyPottr.exe
   ```

#### Option 2: Using MinGW on Windows

1. **Install MinGW-w64:**
   - Download from: https://sourceforge.net/projects/mingw-w64/
   - Or use MSYS2: https://www.msys2.org/

2. **Build:**
   ```cmd
   mkdir build
   cd build
   cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
   cmake --build .
   ```

### Testing the Build

```cmd
cd build\bin\Release

:: Test console mode
HoneyPottr.exe --console

:: Install as service (requires administrator)
HoneyPottr.exe --install

:: Check service status
sc query HoneyPottr
```

---

## Cross-Compilation from macOS/Linux

**⚠️ Important:** Cross-compilation has significant limitations due to missing Windows SDK headers. This approach is provided for development/testing but **full compilation requires a Windows environment**.

### Prerequisites
- Docker Desktop
- 8GB+ RAM available for Docker
- 10GB+ disk space

### Using the Build Script (Recommended)

```bash
# Make the script executable
chmod +x build.sh

# Run the build
./build.sh
```

The script will:
1. Check Docker installation
2. Build a Docker image with MinGW-w64
3. Attempt cross-compilation
4. Extract any artifacts to `build/` directory

### Manual Docker Build

```bash
# Build the Docker image
docker build -t honeypottr-windows-build:latest .

# Create a container
docker create --name honeypottr-temp honeypottr-windows-build:latest

# Copy build artifacts
docker cp honeypottr-temp:/output/. ./build/

# Clean up
docker rm honeypottr-temp
```

### Using Docker Compose

```bash
# Build using MinGW cross-compilation
docker-compose up windows-build

# Or build artifacts will be in ./build/
```

### Limitations of Cross-Compilation

- **Missing Windows SDK headers**: PDH, WMI, and other advanced Windows APIs
- **No Windows Event Log support**: Event log APIs unavailable
- **No resource compilation**: .rc files cannot be processed
- **Limited testing**: Cannot run or test the executable on macOS/Linux

**Recommended:** Use this only for syntax checking and basic builds. For production builds, use Windows or GitHub Actions.

---

## Using GitHub Actions

The repository includes GitHub Actions workflow for automated Windows builds.

### Setup

1. **Push to GitHub:**
   ```bash
   git add .
   git commit -m "Initial commit"
   git push origin main
   ```

2. **Automatic builds:**
   - Builds trigger on push to `main` or `develop`
   - Also on pull requests
   - Manual trigger via Actions tab

3. **Download artifacts:**
   - Go to Actions tab in GitHub
   - Select the latest workflow run
   - Download artifacts (executable + installer)

### Manual Trigger

1. Go to **Actions** tab in your GitHub repository
2. Select **Windows Build** workflow
3. Click **Run workflow**
4. Choose branch and run

### Artifacts

The workflow produces:
- `HoneyPottr-Release-x64`: Production executable
- `HoneyPottr-Debug-x64`: Debug build with symbols
- `HoneyPottr-Installer-x64`: MSI installer package

---

## Docker Build

### Full Windows Container Build

Requires Docker Desktop with Windows containers enabled (Windows host only):

```bash
docker-compose up windows-native
```

This uses `mcr.microsoft.com/windows/servercore` base image and builds natively with Visual Studio tools.

### Dockerfile Stages

The multi-stage Dockerfile includes:

1. **Builder stage**: Sets up MinGW cross-compilation environment
2. **Artifacts stage**: Extracts build outputs
3. **Final stage**: Provides ready-to-copy artifacts

---

## Troubleshooting

### CMake Cannot Find Compiler

**Windows:**
```cmd
:: Ensure Visual Studio is in PATH
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Then run CMake
cmake -B build ..
```

**Cross-compilation:**
```bash
# Verify MinGW installation
docker run --rm honeypottr-windows-build:latest x86_64-w64-mingw32-gcc --version
```

### Missing Dependencies

```cmd
:: Install vcpkg for dependency management
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install

:: Install nlohmann-json
vcpkg install nlohmann-json:x64-windows
```

### Linker Errors

Ensure all required Windows libraries are linked (see `CMakeLists.txt`):
- advapi32, kernel32, user32, shell32
- ole32, oleaut32, wbemuuid
- ws2_32, iphlpapi, psapi
- version, wintrust, crypt32, pdh

### Build Errors in Docker

If cross-compilation fails with header errors:

```bash
# View detailed logs
docker build --no-cache --progress=plain -t honeypottr-windows-build:latest .

# Or use Windows VM/GitHub Actions instead
```

### Performance Issues

For faster builds:

**Windows:**
```cmd
:: Use Ninja generator
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

**Docker:**
```bash
# Use build cache
docker build --cache-from honeypottr-windows-build:latest -t honeypottr-windows-build:latest .
```

---

## Build Configuration Options

### CMake Options

```cmake
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release with debug info
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..

# Custom install prefix
cmake -DCMAKE_INSTALL_PREFIX="C:\Program Files\HoneyPottr" ..

# Static linking (if available)
cmake -DBUILD_SHARED_LIBS=OFF ..
```

### Compiler Flags

Edit `CMakeLists.txt` to modify compiler flags:

```cmake
# MSVC
add_compile_options(/W4 /WX /O2)

# MinGW
add_compile_options(-Wall -Wextra -O3)
```

---

## Recommended Build Method by Platform

| Platform | Method | Completeness | Difficulty |
|----------|--------|--------------|------------|
| Windows | Visual Studio | 100% | Easy |
| Windows | MinGW | 100% | Medium |
| macOS | GitHub Actions | 100% | Easy |
| macOS | Docker Cross-Compile | ~60% | Hard |
| Linux | GitHub Actions | 100% | Easy |
| Linux | Docker Cross-Compile | ~60% | Hard |

---

## Next Steps After Building

1. **Test in console mode:** `HoneyPottr.exe --console`
2. **Review logs:** Check `logs/honeypottr.log`
3. **Configure:** Edit `config/default_config.json`
4. **Install service:** `HoneyPottr.exe --install` (as Administrator)
5. **Start service:** `net start HoneyPottr`

---

## Support

For build issues:
1. Check this document
2. Review CMake output carefully
3. Ensure all prerequisites are installed
4. Try GitHub Actions for guaranteed working builds

For Windows-specific issues, native compilation is always recommended over cross-compilation.

