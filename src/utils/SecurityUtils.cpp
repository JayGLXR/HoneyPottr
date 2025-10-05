#include "utils/SecurityUtils.h"
#include <wincrypt.h>
#include <sddl.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <wintrust.h>
#include <softpub.h>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "advapi32.lib")

namespace HoneyPottr {
namespace SecurityUtils {

bool GenerateRandomBytes(unsigned char* buffer, size_t size) {
    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, nullptr, nullptr, 
                            PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return false;
    }
    
    bool result = CryptGenRandom(hCryptProv, static_cast<DWORD>(size), buffer) != 0;
    CryptReleaseContext(hCryptProv, 0);
    
    return result;
}

std::string GenerateRandomString(size_t length, bool alphanumeric) {
    const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const char allchars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-_=+";
    
    const char* charset = alphanumeric ? alphanum : allchars;
    size_t charsetLen = alphanumeric ? sizeof(alphanum) - 1 : sizeof(allchars) - 1;
    
    std::vector<unsigned char> randomBytes(length);
    if (!GenerateRandomBytes(randomBytes.data(), length)) {
        return std::string();
    }
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += charset[randomBytes[i] % charsetLen];
    }
    
    return result;
}

std::string CalculateSHA256(const unsigned char* data, size_t size) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    
    if (!CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return std::string();
    }
    
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return std::string();
    }
    
    if (!CryptHashData(hHash, data, static_cast<DWORD>(size), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return std::string();
    }
    
    DWORD hashLen = 0;
    DWORD hashLenSize = sizeof(DWORD);
    CryptGetHashParam(hHash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashLen), &hashLenSize, 0);
    
    std::vector<BYTE> hashBytes(hashLen);
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hashBytes.data(), &hashLen, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return std::string();
    }
    
    std::ostringstream oss;
    for (DWORD i = 0; i < hashLen; ++i) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(hashBytes[i]);
    }
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    
    return oss.str();
}

std::string CalculateSHA256(const std::string& str) {
    return CalculateSHA256(reinterpret_cast<const unsigned char*>(str.data()), str.size());
}

bool VerifyFileSignature(const std::string& filePath) {
    std::wstring wFilePath(filePath.begin(), filePath.end());
    
    WINTRUST_FILE_INFO fileData = {};
    fileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    fileData.pcwszFilePath = wFilePath.c_str();
    
    GUID policyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    
    WINTRUST_DATA trustData = {};
    trustData.cbStruct = sizeof(trustData);
    trustData.dwUIChoice = WTD_UI_NONE;
    trustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    trustData.dwUnionChoice = WTD_CHOICE_FILE;
    trustData.pFile = &fileData;
    trustData.dwStateAction = WTD_STATEACTION_VERIFY;
    
    LONG status = WinVerifyTrust(nullptr, &policyGUID, &trustData);
    
    // Close the trust data
    trustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &policyGUID, &trustData);
    
    return status == ERROR_SUCCESS;
}

bool IsElevatedIntegrity() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }
    
    TOKEN_ELEVATION elevation;
    DWORD size;
    if (!GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
        CloseHandle(token);
        return false;
    }
    
    CloseHandle(token);
    return elevation.TokenIsElevated != 0;
}

void SecureZeroMemory(void* buffer, size_t size) {
    volatile unsigned char* p = static_cast<volatile unsigned char*>(buffer);
    while (size--) {
        *p++ = 0;
    }
}

std::vector<unsigned char> EncryptDPAPI(const std::vector<unsigned char>& plaintext,
                                        const std::string& description) {
    DATA_BLOB dataIn;
    dataIn.pbData = const_cast<BYTE*>(plaintext.data());
    dataIn.cbData = static_cast<DWORD>(plaintext.size());
    
    DATA_BLOB dataOut = {};
    
    std::wstring wDescription(description.begin(), description.end());
    LPWSTR pDescription = description.empty() ? nullptr : const_cast<LPWSTR>(wDescription.c_str());
    
    if (!CryptProtectData(&dataIn, pDescription, nullptr, nullptr, nullptr, 0, &dataOut)) {
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> result(dataOut.pbData, dataOut.pbData + dataOut.cbData);
    LocalFree(dataOut.pbData);
    
    return result;
}

std::vector<unsigned char> DecryptDPAPI(const std::vector<unsigned char>& ciphertext,
                                        std::string* description) {
    DATA_BLOB dataIn;
    dataIn.pbData = const_cast<BYTE*>(ciphertext.data());
    dataIn.cbData = static_cast<DWORD>(ciphertext.size());
    
    DATA_BLOB dataOut = {};
    LPWSTR pDescription = nullptr;
    
    if (!CryptUnprotectData(&dataIn, &pDescription, nullptr, nullptr, nullptr, 0, &dataOut)) {
        return std::vector<unsigned char>();
    }
    
    std::vector<unsigned char> result(dataOut.pbData, dataOut.pbData + dataOut.cbData);
    
    if (description && pDescription) {
        std::wstring wDesc(pDescription);
        *description = std::string(wDesc.begin(), wDesc.end());
    }
    
    if (pDescription) {
        LocalFree(pDescription);
    }
    LocalFree(dataOut.pbData);
    
    return result;
}

bool IsRunningInVM() {
    // Check for common VM artifacts
    bool vmDetected = false;
    
    // Check for VMware
    __try {
        unsigned int magic = 0;
        unsigned int port = 0x5658; // VMware I/O port
        
        __asm {
            push edx
            push ecx
            push ebx
            
            mov eax, 'VMXh'
            mov ebx, 0
            mov ecx, 10
            mov edx, port
            in eax, dx
            
            mov magic, ebx
            
            pop ebx
            pop ecx
            pop edx
        }
        
        if (magic == 0x564D5868) { // 'VMXh'
            vmDetected = true;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        // Exception means not in VM
    }
    
    // Check for VirtualBox via registry (simpler check)
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                     "SOFTWARE\\Oracle\\VirtualBox Guest Additions", 
                     0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        vmDetected = true;
    }
    
    return vmDetected;
}

bool IsDebuggerPresent() {
    return ::IsDebuggerPresent() != 0;
}

std::string GetCurrentUserSID() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return std::string();
    }
    
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    
    std::vector<BYTE> buffer(size);
    if (!GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
        CloseHandle(token);
        return std::string();
    }
    
    TOKEN_USER* tokenUser = reinterpret_cast<TOKEN_USER*>(buffer.data());
    LPSTR sidString = nullptr;
    
    if (!ConvertSidToStringSidA(tokenUser->User.Sid, &sidString)) {
        CloseHandle(token);
        return std::string();
    }
    
    std::string result(sidString);
    LocalFree(sidString);
    CloseHandle(token);
    
    return result;
}

bool HasPrivilege(const std::string& privilegeName) {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }
    
    LUID luid;
    if (!LookupPrivilegeValueA(nullptr, privilegeName.c_str(), &luid)) {
        CloseHandle(token);
        return false;
    }
    
    PRIVILEGE_SET privs;
    privs.PrivilegeCount = 1;
    privs.Control = PRIVILEGE_SET_ALL_NECESSARY;
    privs.Privilege[0].Luid = luid;
    privs.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    BOOL result = FALSE;
    PrivilegeCheck(token, &privs, &result);
    
    CloseHandle(token);
    return result != FALSE;
}

} // namespace SecurityUtils
} // namespace HoneyPottr
