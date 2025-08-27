#include <windows.h>
#include <iostream>
#include <string>
#include <memory>
#include <io.h>
#include <fcntl.h>
#include <shellapi.h>

#include "service/ServiceController.h"
#include "service/WindowsService.h"
#include "common/Constants.h"

using namespace HoneyPottr;

// Global service controller instance
std::unique_ptr<ServiceController> g_serviceController;

// Service control handler
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
    if (!g_serviceController) {
        return;
    }

    switch (dwCtrl) {
        case SERVICE_CONTROL_STOP:
            g_serviceController->Stop();
            break;
        case SERVICE_CONTROL_PAUSE:
            g_serviceController->Pause();
            break;
        case SERVICE_CONTROL_CONTINUE:
            g_serviceController->Resume();
            break;
        case SERVICE_CONTROL_INTERROGATE:
            // Return current status
            break;
        default:
            break;
    }
}

// Service main function
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    // Register service control handler
    std::wstring serviceNameW(Constants::SERVICE_NAME, Constants::SERVICE_NAME + strlen(Constants::SERVICE_NAME));
    SERVICE_STATUS_HANDLE statusHandle = RegisterServiceCtrlHandler(
        serviceNameW.c_str(), ServiceCtrlHandler);
    
    if (statusHandle == 0) {
        return;
    }

    // Initialize service controller
    g_serviceController = std::make_unique<ServiceController>();
    
    // Set service status to starting
    SERVICE_STATUS status = {};
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 3000;
    
    SetServiceStatus(statusHandle, &status);

    // Initialize the service
    std::string configPath;
    if (argc > 1) {
        // Convert wide string to narrow string
        std::wstring wConfigPath(argv[1]);
        configPath = std::string(wConfigPath.begin(), wConfigPath.end());
    }

    bool initSuccess = g_serviceController->Initialize(configPath);
    if (!initSuccess) {
        status.dwCurrentState = SERVICE_STOPPED;
        status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
        status.dwServiceSpecificExitCode = 1;
        SetServiceStatus(statusHandle, &status);
        return;
    }

    // Start the service
    bool startSuccess = g_serviceController->Start();
    if (!startSuccess) {
        status.dwCurrentState = SERVICE_STOPPED;
        status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
        status.dwServiceSpecificExitCode = 2;
        SetServiceStatus(statusHandle, &status);
        return;
    }

    // Set service status to running
    status.dwCurrentState = SERVICE_RUNNING;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    SetServiceStatus(statusHandle, &status);

    // Set up state change callback to update service status
    g_serviceController->SetStateCallback([statusHandle](ServiceState state) {
        SERVICE_STATUS status = {};
        status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
        status.dwWin32ExitCode = 0;
        status.dwServiceSpecificExitCode = 0;
        status.dwCheckPoint = 0;
        status.dwWaitHint = 0;

        switch (state) {
            case ServiceState::Running:
                status.dwCurrentState = SERVICE_RUNNING;
                break;
            case ServiceState::Stopped:
                status.dwCurrentState = SERVICE_STOPPED;
                break;
            case ServiceState::Paused:
                status.dwCurrentState = SERVICE_PAUSED;
                break;
            case ServiceState::Starting:
                status.dwCurrentState = SERVICE_START_PENDING;
                status.dwWaitHint = 3000;
                break;
            case ServiceState::Stopping:
                status.dwCurrentState = SERVICE_STOP_PENDING;
                status.dwWaitHint = 3000;
                break;
        }

        SetServiceStatus(statusHandle, &status);
    });

    // Wait for service to stop
    while (g_serviceController->IsRunning()) {
        Sleep(1000);
    }

    // Set service status to stopped
    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(statusHandle, &status);
}

// Console application entry point for testing
int RunConsoleMode(int argc, char* argv[]) {
    // Allocate a console for this GUI application
    AllocConsole();
    
    // Redirect stdout, stdin, stderr to console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    
    // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio(true);
    std::cout << "HoneyPottr - Console Mode\n";
    std::cout << "========================\n\n";

    // Create service controller
    auto serviceController = std::make_unique<ServiceController>();

    // Initialize
    std::string configPath;
    if (argc > 2) {
        configPath = argv[2];
    }

    std::cout << "Initializing service controller...\n";
    if (!serviceController->Initialize(configPath)) {
        std::cerr << "Failed to initialize service controller\n";
        return 1;
    }

    // Start
    std::cout << "Starting artifact generation...\n";
    if (!serviceController->Start()) {
        std::cerr << "Failed to start service\n";
        return 2;
    }

    std::cout << "HoneyPottr is running. Press Enter to stop...\n";
    std::cin.get();

    // Stop
    std::cout << "Stopping service...\n";
    serviceController->Stop();

    std::cout << "Service stopped.\n";
    
    // Clean up console
    FreeConsole();
    return 0;
}

// Install service
int InstallService() {
    SC_HANDLE scManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scManager) {
        std::cerr << "Failed to open Service Control Manager\n";
        std::cerr << "Make sure you are running as Administrator\n";
        return 1;
    }

    // Get current executable path
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);

    // Convert service names to wide strings
    std::wstring serviceNameW(Constants::SERVICE_NAME, Constants::SERVICE_NAME + strlen(Constants::SERVICE_NAME));
    std::wstring displayNameW(Constants::SERVICE_DISPLAY_NAME, Constants::SERVICE_DISPLAY_NAME + strlen(Constants::SERVICE_DISPLAY_NAME));

    SC_HANDLE service = CreateService(
        scManager,
        serviceNameW.c_str(),
        displayNameW.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        exePath,
        nullptr,
        nullptr,
        nullptr,
        L"LocalSystem",  // Run as LocalSystem for necessary privileges
        nullptr
    );

    if (!service) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_EXISTS) {
            std::cout << "Service already exists\n";
        } else {
            std::cerr << "Failed to create service. Error: " << error << "\n";
        }
        CloseServiceHandle(scManager);
        return 1;
    }

    // Set service description
    std::wstring descriptionW(Constants::SERVICE_DESCRIPTION, Constants::SERVICE_DESCRIPTION + strlen(Constants::SERVICE_DESCRIPTION));
    SERVICE_DESCRIPTION desc;
    desc.lpDescription = const_cast<wchar_t*>(descriptionW.c_str());
    ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &desc);

    // Configure service failure actions for automatic restart
    SC_ACTION failureActions[3];
    failureActions[0].Type = SC_ACTION_RESTART;
    failureActions[0].Delay = 5000;  // Restart after 5 seconds
    failureActions[1].Type = SC_ACTION_RESTART;
    failureActions[1].Delay = 10000; // Restart after 10 seconds
    failureActions[2].Type = SC_ACTION_RESTART;
    failureActions[2].Delay = 30000; // Restart after 30 seconds

    SERVICE_FAILURE_ACTIONS sfa;
    sfa.dwResetPeriod = 86400; // Reset failure count after 24 hours
    sfa.lpRebootMsg = nullptr;
    sfa.lpCommand = nullptr;
    sfa.cActions = 3;
    sfa.lpsaActions = failureActions;

    ChangeServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);

    // Set service to start after network is available
    SERVICE_DELAYED_AUTO_START_INFO delayedStart;
    delayedStart.fDelayedAutostart = TRUE;
    ChangeServiceConfig2(service, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &delayedStart);

    // Configure service to run even when no user is logged in
    SERVICE_SID_INFO sidInfo;
    sidInfo.dwServiceSidType = SERVICE_SID_TYPE_UNRESTRICTED;
    ChangeServiceConfig2(service, SERVICE_CONFIG_SERVICE_SID_INFO, &sidInfo);

    std::cout << "Service installed successfully with the following configuration:\n";
    std::cout << "- Automatic startup (delayed)\n";
    std::cout << "- Runs as LocalSystem\n";
    std::cout << "- Automatic restart on failure\n";
    std::cout << "- Runs even when no user is logged in\n";
    
    CloseServiceHandle(service);
    CloseServiceHandle(scManager);
    return 0;
}

// Uninstall service
int UninstallService() {
    SC_HANDLE scManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scManager) {
        std::cerr << "Failed to open Service Control Manager\n";
        return 1;
    }

    std::wstring serviceNameW(Constants::SERVICE_NAME, Constants::SERVICE_NAME + strlen(Constants::SERVICE_NAME));
    SC_HANDLE service = OpenService(scManager, serviceNameW.c_str(), DELETE);
    if (!service) {
        std::cerr << "Failed to open service\n";
        CloseServiceHandle(scManager);
        return 1;
    }

    if (!DeleteService(service)) {
        std::cerr << "Failed to delete service\n";
        CloseServiceHandle(service);
        CloseServiceHandle(scManager);
        return 1;
    }

    std::cout << "Service uninstalled successfully\n";
    
    CloseServiceHandle(service);
    CloseServiceHandle(scManager);
    return 0;
}

// Print usage information
void PrintUsage(const char* programName) {
    std::cout << "HoneyPottr - Honeypot Artifact Generator\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --console [config]    Run in console mode (for testing)\n";
    std::cout << "  --install             Install as Windows service\n";
    std::cout << "  --uninstall           Uninstall Windows service\n";
    std::cout << "  --help                Show this help message\n\n";
    std::cout << "When run without options, operates as a Windows service.\n";
}

// Parse command line for Windows application
int ParseCommandLine(int& argc, char**& argv) {
    // Get command line
    LPWSTR cmdLine = GetCommandLineW();
    
    // Parse command line to argc/argv
    LPWSTR* szArglist = CommandLineToArgvW(cmdLine, &argc);
    if (szArglist == nullptr) {
        return 0;
    }
    
    // Convert to char* array
    argv = new char*[argc];
    for (int i = 0; i < argc; i++) {
        int len = WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, nullptr, 0, nullptr, nullptr);
        argv[i] = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, argv[i], len, nullptr, nullptr);
    }
    
    LocalFree(szArglist);
    return argc;
}

// Main entry point for Windows application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    
    // Parse command line arguments
    int argc = 0;
    char** argv = nullptr;
    ParseCommandLine(argc, argv);
    
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "--console") {
            int result = RunConsoleMode(argc, argv);
            // Clean up argv
            for (int i = 0; i < argc; i++) {
                delete[] argv[i];
            }
            delete[] argv;
            return result;
        } else if (arg == "--install") {
            int result = InstallService();
            // Clean up argv
            for (int i = 0; i < argc; i++) {
                delete[] argv[i];
            }
            delete[] argv;
            return result;
        } else if (arg == "--uninstall") {
            int result = UninstallService();
            // Clean up argv
            for (int i = 0; i < argc; i++) {
                delete[] argv[i];
            }
            delete[] argv;
            return result;
        } else if (arg == "--help" || arg == "-h") {
            // For help, we need to show console
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            std::ios::sync_with_stdio(true);
            PrintUsage(argv[0]);
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            FreeConsole();
            // Clean up argv
            for (int i = 0; i < argc; i++) {
                delete[] argv[i];
            }
            delete[] argv;
            return 0;
        }
    }

    // Run as Windows service
    std::wstring serviceNameW(Constants::SERVICE_NAME, Constants::SERVICE_NAME + strlen(Constants::SERVICE_NAME));
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { const_cast<wchar_t*>(serviceNameW.c_str()), ServiceMain },
        { nullptr, nullptr }
    };

    if (!StartServiceCtrlDispatcher(serviceTable)) {
        DWORD error = GetLastError();
        if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            // Not running as service, show usage in console
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
            std::ios::sync_with_stdio(true);
            PrintUsage(argv[0]);
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            FreeConsole();
        } else {
            // Show error in message box since we can't use console
            std::string errorMsg = "Failed to start service control dispatcher. Error: " + std::to_string(error);
            MessageBoxA(nullptr, errorMsg.c_str(), "HoneyPottr Error", MB_ICONERROR);
        }
        
        // Clean up argv
        for (int i = 0; i < argc; i++) {
            delete[] argv[i];
        }
        delete[] argv;
        return error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ? 0 : 1;
    }

    // Clean up argv
    for (int i = 0; i < argc; i++) {
        delete[] argv[i];
    }
    delete[] argv;
    return 0;
}
