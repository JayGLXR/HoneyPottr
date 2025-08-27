#pragma once

#include <string>

namespace HoneyPottr {
namespace Constants {

// Service configuration
constexpr const char* SERVICE_NAME = "HoneyPottr";
constexpr const char* SERVICE_DISPLAY_NAME = "HoneyPottr Honeypot Artifact Generator";
constexpr const char* SERVICE_DESCRIPTION = "Generates honeypot artifacts to deceive malware into self-termination";

// Configuration files
constexpr const char* DEFAULT_CONFIG_FILE = "config/default_config.json";
constexpr const char* BASIC_PROFILE_FILE = "config/basic_profile.json";
constexpr const char* ADVANCED_PROFILE_FILE = "config/advanced_profile.json";

// Registry paths for common honeypot detection
namespace Registry {
    constexpr const char* VMWARE_TOOLS = "SOFTWARE\\VMware, Inc.\\VMware Tools";
    constexpr const char* VBOX_GUEST_ADDITIONS = "SOFTWARE\\Oracle\\VirtualBox Guest Additions";
    constexpr const char* SANDBOXIE = "SOFTWARE\\Sandboxie";
    constexpr const char* WIRESHARK_UNINSTALL = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Wireshark";
    constexpr const char* VBOX_SERVICE = "SYSTEM\\CurrentControlSet\\Services\\VBoxService";
    constexpr const char* VMWARE_SERVICE = "SYSTEM\\CurrentControlSet\\Services\\VMTools";
    constexpr const char* PROCMON_UNINSTALL = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Process Monitor";
}

// File paths for common honeypot detection
namespace FilePaths {
    constexpr const char* VMWARE_TOOLS_DIR = "C:\\Program Files\\VMware\\VMware Tools\\";
    constexpr const char* VBOX_ADDITIONS_DIR = "C:\\Program Files\\Oracle\\VirtualBox Guest Additions\\";
    constexpr const char* VBOX_MOUSE_DRIVER = "C:\\Windows\\System32\\drivers\\VBoxMouse.sys";
    constexpr const char* VMWARE_HGFS_DRIVER = "C:\\Windows\\System32\\drivers\\vmhgfs.sys";
    constexpr const char* WIRESHARK_DESKTOP = "C:\\Users\\Public\\Desktop\\Wireshark.lnk";
    constexpr const char* PROCMON_TOOLS = "C:\\Tools\\ProcessMonitor.exe";
    constexpr const char* SANDBOXIE_DIR = "C:\\Program Files\\Sandboxie\\";
}

// Process names for common honeypot detection
namespace ProcessNames {
    constexpr const char* VMTOOLS_SERVICE = "vmtoolsd.exe";
    constexpr const char* VBOX_SERVICE = "VBoxService.exe";
    constexpr const char* PROCESS_MONITOR = "procmon.exe";
    constexpr const char* WIRESHARK = "wireshark.exe";
    constexpr const char* SANDBOXIE = "sandboxie.exe";
    constexpr const char* VMWARE_TRAY = "vmware-tray.exe";
    constexpr const char* VBOX_TRAY = "VBoxTray.exe";
}

// Network artifacts
namespace Network {
    constexpr const char* VMWARE_MAC_PREFIX = "00:0C:29";
    constexpr const char* VBOX_MAC_PREFIX = "08:00:27";
    constexpr const char* VMWARE_ADAPTER_NAME = "VMware Virtual Ethernet Adapter";
    constexpr const char* VBOX_ADAPTER_NAME = "VirtualBox Host-Only Ethernet Adapter";
}

// Hardware artifacts
namespace Hardware {
    constexpr const char* VMWARE_BIOS_VENDOR = "VMware, Inc.";
    constexpr const char* VBOX_BIOS_VENDOR = "innotek GmbH";
    constexpr const char* VMWARE_SYSTEM_MANUFACTURER = "VMware, Inc.";
    constexpr const char* VBOX_SYSTEM_MANUFACTURER = "innotek GmbH";
    constexpr const char* VMWARE_SYSTEM_PRODUCT = "VMware Virtual Platform";
    constexpr const char* VBOX_SYSTEM_PRODUCT = "VirtualBox";
}

// Performance limits
namespace Performance {
    constexpr size_t MAX_MEMORY_USAGE_MB = 50;
    constexpr double MAX_CPU_USAGE_PERCENT = 1.0;
    constexpr size_t MAX_DISK_IO_OPERATIONS = 100;
    constexpr size_t MAX_NETWORK_BANDWIDTH_BPS = 1024;
}

// Timing constants
namespace Timing {
    constexpr int UPDATE_INTERVAL_SECONDS = 3600;
    constexpr int MONITORING_INTERVAL_SECONDS = 30;
    constexpr int ARTIFACT_REFRESH_INTERVAL_SECONDS = 7200;
    constexpr int CONFIG_CHECK_INTERVAL_SECONDS = 300;
}

// Logging constants
namespace Logging {
    constexpr const char* LOG_FILE_PATH = "logs/honeypottr.log";
    constexpr size_t MAX_LOG_FILE_SIZE_MB = 10;
    constexpr int MAX_LOG_FILES = 5;
    constexpr const char* EVENT_LOG_SOURCE = "HoneyPottr";
}

// Security constants
namespace Security {
    constexpr const char* ENCRYPTION_ALGORITHM = "AES-256-GCM";
    constexpr size_t ENCRYPTION_KEY_SIZE = 32;
    constexpr size_t ENCRYPTION_IV_SIZE = 16;
    constexpr const char* CERTIFICATE_STORE = "MY";
    constexpr const char* CERTIFICATE_SUBJECT = "CN=HoneyPottr";
}

} // namespace Constants
} // namespace HoneyPottr
