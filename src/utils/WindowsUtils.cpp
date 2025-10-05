#include "utils/WindowsUtils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

namespace HoneyPottr {
namespace WindowsUtils {

std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), 
                                         static_cast<int>(wstr.size()), 
                                         nullptr, 0, nullptr, nullptr);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), 
                       static_cast<int>(wstr.size()), 
                       &result[0], size_needed, nullptr, nullptr);
    return result;
}

std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), 
                                         static_cast<int>(str.size()), 
                                         nullptr, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), 
                       static_cast<int>(str.size()), 
                       &result[0], size_needed);
    return result;
}

bool IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&ntAuthority, 2, 
                                 SECURITY_BUILTIN_DOMAIN_RID, 
                                 DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    return isAdmin != FALSE;
}

std::string GetExecutablePath() {
    char path[MAX_PATH];
    if (GetModuleFileNameA(nullptr, path, MAX_PATH) > 0) {
        return std::string(path);
    }
    return std::string();
}

std::string GetExecutableDirectory() {
    std::string exePath = GetExecutablePath();
    if (exePath.empty()) {
        return std::string();
    }
    
    size_t lastSlash = exePath.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        return exePath.substr(0, lastSlash);
    }
    
    return exePath;
}

bool CreateDirectoryRecursive(const std::string& path) {
    if (DirectoryExists(path)) {
        return true;
    }
    
    // Try to create parent directory first
    size_t lastSlash = path.find_last_of("\\/");
    if (lastSlash != std::string::npos && lastSlash > 0) {
        std::string parent = path.substr(0, lastSlash);
        if (!CreateDirectoryRecursive(parent)) {
            return false;
        }
    }
    
    return CreateDirectoryA(path.c_str(), nullptr) != 0 || 
           GetLastError() == ERROR_ALREADY_EXISTS;
}

bool FileExists(const std::string& path) {
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && 
           !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool DirectoryExists(const std::string& path) {
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && 
           (attr & FILE_ATTRIBUTE_DIRECTORY);
}

std::string GetWindowsVersion() {
    std::string version = "Windows";
    
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    
    // Use RtlGetVersion for accurate version info
    typedef NTSTATUS (WINAPI *RtlGetVersionFunc)(PRTL_OSVERSIONINFOW);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll) {
        auto RtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(
            GetProcAddress(ntdll, "RtlGetVersion"));
        if (RtlGetVersion) {
            RtlGetVersion(reinterpret_cast<PRTL_OSVERSIONINFOW>(&osvi));
            
            if (osvi.dwMajorVersion == 10) {
                version += " 10/11";
            } else if (osvi.dwMajorVersion == 6) {
                if (osvi.dwMinorVersion == 3) version += " 8.1";
                else if (osvi.dwMinorVersion == 2) version += " 8";
                else if (osvi.dwMinorVersion == 1) version += " 7";
                else if (osvi.dwMinorVersion == 0) version += " Vista";
            }
        }
    }
    
    return version;
}

std::string GetComputerName() {
    char name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(name);
    if (::GetComputerNameA(name, &size)) {
        return std::string(name);
    }
    return std::string();
}

std::string GetUserName() {
    char name[UNLEN + 1];
    DWORD size = sizeof(name);
    if (::GetUserNameA(name, &size)) {
        return std::string(name);
    }
    return std::string();
}

std::string GenerateGUID() {
    GUID guid;
    if (CoCreateGuid(&guid) != S_OK) {
        return std::string();
    }
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8) << guid.Data1 << "-"
        << std::setw(4) << guid.Data2 << "-"
        << std::setw(4) << guid.Data3 << "-"
        << std::setw(2) << static_cast<int>(guid.Data4[0])
        << std::setw(2) << static_cast<int>(guid.Data4[1]) << "-"
        << std::setw(2) << static_cast<int>(guid.Data4[2])
        << std::setw(2) << static_cast<int>(guid.Data4[3])
        << std::setw(2) << static_cast<int>(guid.Data4[4])
        << std::setw(2) << static_cast<int>(guid.Data4[5])
        << std::setw(2) << static_cast<int>(guid.Data4[6])
        << std::setw(2) << static_cast<int>(guid.Data4[7]);
    
    return oss.str();
}

std::string GetLastErrorString(DWORD errorCode) {
    if (errorCode == 0) {
        errorCode = ::GetLastError();
    }
    
    if (errorCode == 0) {
        return std::string();
    }
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    
    // Remove trailing newlines
    while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
        message.pop_back();
    }
    
    return message;
}

bool SetFileAttributesEx(const std::string& path, DWORD attributes) {
    return ::SetFileAttributesA(path.c_str(), attributes) != 0;
}

bool IsProcessRunning(const std::string& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    
    bool found = false;
    if (Process32First(snapshot, &entry)) {
        do {
            std::string exeFile(entry.szExeFile);
            std::transform(exeFile.begin(), exeFile.end(), exeFile.begin(), ::tolower);
            std::string searchName = processName;
            std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
            
            if (exeFile == searchName) {
                found = true;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    
    CloseHandle(snapshot);
    return found;
}

std::vector<std::string> GetRunningProcesses() {
    std::vector<std::string> processes;
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }
    
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &entry)) {
        do {
            processes.push_back(std::string(entry.szExeFile));
        } while (Process32Next(snapshot, &entry));
    }
    
    CloseHandle(snapshot);
    return processes;
}

bool SetPrivilege(const std::string& privilegeName, bool enable) {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return false;
    }
    
    TOKEN_PRIVILEGES tp;
    LUID luid;
    
    if (!LookupPrivilegeValueA(nullptr, privilegeName.c_str(), &luid)) {
        CloseHandle(token);
        return false;
    }
    
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
    
    bool result = AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), 
                                       nullptr, nullptr) != 0;
    
    CloseHandle(token);
    return result;
}

} // namespace WindowsUtils
} // namespace HoneyPottr
