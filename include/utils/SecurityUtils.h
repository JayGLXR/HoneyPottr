#pragma once

#include <string>
#include <vector>
#include <windows.h>

namespace HoneyPottr {
namespace SecurityUtils {

/**
 * @brief Generate cryptographically secure random bytes
 * @param buffer Buffer to fill with random data
 * @param size Size of buffer in bytes
 * @return true if successful
 */
bool GenerateRandomBytes(unsigned char* buffer, size_t size);

/**
 * @brief Generate a random string
 * @param length Length of string to generate
 * @param alphanumeric If true, only use alphanumeric characters
 * @return Random string
 */
std::string GenerateRandomString(size_t length, bool alphanumeric = true);

/**
 * @brief Calculate SHA-256 hash of data
 * @param data Data to hash
 * @param size Size of data
 * @return Hex-encoded hash string
 */
std::string CalculateSHA256(const unsigned char* data, size_t size);

/**
 * @brief Calculate SHA-256 hash of string
 * @param str String to hash
 * @return Hex-encoded hash string
 */
std::string CalculateSHA256(const std::string& str);

/**
 * @brief Verify file signature
 * @param filePath Path to file
 * @return true if file has valid signature
 */
bool VerifyFileSignature(const std::string& filePath);

/**
 * @brief Check if current process integrity level is elevated
 * @return true if running with high integrity
 */
bool IsElevatedIntegrity();

/**
 * @brief Securely wipe memory
 * @param buffer Buffer to wipe
 * @param size Size of buffer
 */
void SecureZeroMemory(void* buffer, size_t size);

/**
 * @brief Encrypt data using Windows Data Protection API (DPAPI)
 * @param plaintext Data to encrypt
 * @param description Optional description
 * @return Encrypted data
 */
std::vector<unsigned char> EncryptDPAPI(const std::vector<unsigned char>& plaintext,
                                        const std::string& description = "");

/**
 * @brief Decrypt data using Windows Data Protection API (DPAPI)
 * @param ciphertext Encrypted data
 * @param description Output parameter for description
 * @return Decrypted data
 */
std::vector<unsigned char> DecryptDPAPI(const std::vector<unsigned char>& ciphertext,
                                        std::string* description = nullptr);

/**
 * @brief Check if the system is in a virtual machine
 * @return true if VM detected
 */
bool IsRunningInVM();

/**
 * @brief Check if debugger is present
 * @return true if debugger detected
 */
bool IsDebuggerPresent();

/**
 * @brief Get the SID of the current user
 * @return SID string
 */
std::string GetCurrentUserSID();

/**
 * @brief Check if the current process has a specific privilege
 * @param privilegeName Privilege name (e.g., SE_DEBUG_NAME)
 * @return true if privilege is enabled
 */
bool HasPrivilege(const std::string& privilegeName);

} // namespace SecurityUtils
} // namespace HoneyPottr
