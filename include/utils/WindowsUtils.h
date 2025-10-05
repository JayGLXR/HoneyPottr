#pragma once

#include <string>
#include <vector>
#include <windows.h>

namespace HoneyPottr {
namespace WindowsUtils {

/**
 * @brief Convert wide string to UTF-8
 * @param wstr Wide string
 * @return UTF-8 encoded string
 */
std::string WideToUtf8(const std::wstring& wstr);

/**
 * @brief Convert UTF-8 string to wide string
 * @param str UTF-8 string
 * @return Wide string
 */
std::wstring Utf8ToWide(const std::string& str);

/**
 * @brief Check if the current process is running with administrator privileges
 * @return true if running as administrator
 */
bool IsRunningAsAdmin();

/**
 * @brief Get the path to the current executable
 * @return Executable path
 */
std::string GetExecutablePath();

/**
 * @brief Get the directory containing the current executable
 * @return Directory path
 */
std::string GetExecutableDirectory();

/**
 * @brief Create a directory and all parent directories
 * @param path Directory path
 * @return true if directory exists or was created
 */
bool CreateDirectoryRecursive(const std::string& path);

/**
 * @brief Check if a file exists
 * @param path File path
 * @return true if file exists
 */
bool FileExists(const std::string& path);

/**
 * @brief Check if a directory exists
 * @param path Directory path
 * @return true if directory exists
 */
bool DirectoryExists(const std::string& path);

/**
 * @brief Get Windows version information
 * @return Version string (e.g., "Windows 10")
 */
std::string GetWindowsVersion();

/**
 * @brief Get computer name
 * @return Computer name
 */
std::string GetComputerName();

/**
 * @brief Get current user name
 * @return User name
 */
std::string GetUserName();

/**
 * @brief Generate a GUID as string
 * @return GUID string
 */
std::string GenerateGUID();

/**
 * @brief Get last Windows error as string
 * @param errorCode Error code (or 0 to use GetLastError())
 * @return Error description
 */
std::string GetLastErrorString(DWORD errorCode = 0);

/**
 * @brief Set file attributes
 * @param path File path
 * @param attributes Attributes to set
 * @return true if successful
 */
bool SetFileAttributesEx(const std::string& path, DWORD attributes);

/**
 * @brief Check if a process is running
 * @param processName Process name (e.g., "notepad.exe")
 * @return true if process is running
 */
bool IsProcessRunning(const std::string& processName);

/**
 * @brief Get list of running processes
 * @return Vector of process names
 */
std::vector<std::string> GetRunningProcesses();

/**
 * @brief Enable or disable a Windows privilege for current process
 * @param privilegeName Privilege name (e.g., SE_DEBUG_NAME)
 * @param enable true to enable, false to disable
 * @return true if successful
 */
bool SetPrivilege(const std::string& privilegeName, bool enable);

} // namespace WindowsUtils
} // namespace HoneyPottr
