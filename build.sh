#!/bin/bash
# Cross-compilation build script for HoneyPottr on macOS/Linux

set -e

echo "================================================"
echo "HoneyPottr Windows Cross-Compilation Build"
echo "================================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo -e "${RED}Error: Docker is not installed${NC}"
    echo "Please install Docker Desktop from https://www.docker.com/products/docker-desktop"
    exit 1
fi

# Check if Docker is running
if ! docker info &> /dev/null; then
    echo -e "${RED}Error: Docker is not running${NC}"
    echo "Please start Docker Desktop and try again"
    exit 1
fi

echo -e "${YELLOW}Note: Cross-compilation with MinGW has limitations${NC}"
echo "This will attempt to build but may fail due to missing Windows SDK headers."
echo "For full compilation, use a Windows machine with Visual Studio."
echo ""
echo "Press Enter to continue or Ctrl+C to cancel..."
read

# Clean previous builds
echo -e "${GREEN}Cleaning previous builds...${NC}"
rm -rf build/
mkdir -p build

# Build using Docker
echo -e "${GREEN}Building Docker image...${NC}"
docker build -t honeypottr-windows-build:latest .

# Extract build artifacts
echo -e "${GREEN}Extracting build artifacts...${NC}"
docker create --name honeypottr-temp honeypottr-windows-build:latest
docker cp honeypottr-temp:/output/. ./build/ || echo "No artifacts to copy"
docker rm honeypottr-temp

# Check results
echo ""
echo "================================================"
echo -e "${GREEN}Build process completed${NC}"
echo "================================================"
echo ""

if [ -d "build/bin" ]; then
    echo "Build artifacts found in build/bin:"
    ls -lh build/bin/ || true
else
    echo -e "${YELLOW}Warning: No executable artifacts found${NC}"
    echo ""
    echo "This is expected due to Windows SDK limitations in cross-compilation."
    echo ""
    echo "To complete the build, you have three options:"
    echo "1. Use a Windows machine with Visual Studio"
    echo "2. Use GitHub Actions (see .github/workflows/)"
    echo "3. Use a cloud Windows VM (Azure, AWS, etc.)"
fi

echo ""
echo "Build log available in build/ directory"

