cmake_minimum_required(VERSION 3.20)
project(HoneyPottr VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
    add_compile_definitions(_WIN32_WINNT=0x0A00) # Windows 10
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
else()
    add_compile_definitions(NDEBUG)
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Find required packages
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # Download nlohmann/json as a header-only library
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(HONEYPOTTR_SOURCES
    src/main.cpp
    src/service/ServiceController.cpp
    src/service/WindowsService.cpp
    src/config/ConfigurationManager.cpp
    src/artifacts/ArtifactEngine.cpp
    src/artifacts/modules/RegistryArtifactModule.cpp
    src/artifacts/modules/FileSystemArtifactModule.cpp
    src/artifacts/modules/ProcessArtifactModule.cpp
    src/artifacts/modules/NetworkArtifactModule.cpp
    src/artifacts/modules/HardwareArtifactModule.cpp
    src/monitoring/ResourceMonitor.cpp
    src/logging/Logger.cpp
    src/utils/WindowsUtils.cpp
    src/utils/SecurityUtils.cpp
)

# Header files
set(HONEYPOTTR_HEADERS
    include/service/ServiceController.h
    include/service/WindowsService.h
    include/config/ConfigurationManager.h
    include/artifacts/ArtifactEngine.h
    include/artifacts/IArtifactModule.h
    include/artifacts/modules/RegistryArtifactModule.h
    include/artifacts/modules/FileSystemArtifactModule.h
    include/artifacts/modules/ProcessArtifactModule.h
    include/artifacts/modules/NetworkArtifactModule.h
    include/artifacts/modules/HardwareArtifactModule.h
    include/monitoring/ResourceMonitor.h
    include/logging/Logger.h
    include/utils/WindowsUtils.h
    include/utils/SecurityUtils.h
    include/common/Types.h
    include/common/Constants.h
)

# Create executable (Windows application, not console)
add_executable(HoneyPottr WIN32 ${HONEYPOTTR_SOURCES} ${HONEYPOTTR_HEADERS})

# Link libraries
target_link_libraries(HoneyPottr
    PRIVATE
    nlohmann_json::nlohmann_json
    advapi32    # Registry and service APIs
    kernel32    # Core Windows APIs
    user32      # User interface APIs
    shell32     # Shell APIs
    ole32       # COM APIs
    oleaut32    # Automation APIs
    wbemuuid    # WMI APIs
    ws2_32      # Winsock APIs
    iphlpapi    # IP Helper APIs
    psapi       # Process APIs
    version     # Version APIs
    wintrust    # Trust verification APIs
    crypt32     # Cryptography APIs
)

# Set target properties
set_target_properties(HoneyPottr PROPERTIES
    OUTPUT_NAME "HoneyPottr"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Resource file for version info and manifest
if(WIN32)
    target_sources(HoneyPottr PRIVATE resources/HoneyPottr.rc)
endif()

# Installation
install(TARGETS HoneyPottr
    RUNTIME DESTINATION bin
)

install(FILES
    config/default_config.json
    config/basic_profile.json
    config/advanced_profile.json
    DESTINATION config
)

# CPack configuration for installer
set(CPACK_PACKAGE_NAME "HoneyPottr")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lightweight honeypot artifact generator")
set(CPACK_PACKAGE_VENDOR "HoneyPottr Project")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "HoneyPottr")
set(CPACK_GENERATOR "WIX")
set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012")

# WiX-specific settings for proper permissions
set(CPACK_WIX_PRODUCT_GUID "87654321-4321-4321-4321-210987654321")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/honeypottr.ico")
set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/resources/banner.bmp")
set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/resources/dialog.bmp")
set(CPACK_WIX_PROGRAM_MENU_FOLDER "HoneyPottr")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/honeypottr/honeypottr")

# Require administrator privileges for installation
set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/installer/admin_privileges.xml")

# Custom WiX template for service installation
set(CPACK_WIX_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/installer/HoneyPottr.wxs")

include(CPack)
