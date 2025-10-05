; HoneyPottr NSIS Installer Script - Production Version 1.0.0
; Copyright (c) 2024-2025 HoneyPottr Project
; Licensed under MIT License

!define PRODUCT_NAME "HoneyPottr"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "HoneyPottr Project"
!define PRODUCT_WEB_SITE "https://github.com/honeypottr/honeypottr"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\HoneyPottr.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define SERVICE_NAME "HoneyPottr"
!define MIN_WIN_VER "10.0"

; Modern UI
!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
; ServiceLib not needed - using direct service commands

; Installer settings
Name "${PRODUCT_NAME} ${PRODUCT_VERSION} - Production Release"
OutFile "HoneyPottr-Setup-v${PRODUCT_VERSION}-x64.exe"
InstallDir "$PROGRAMFILES64\HoneyPottr"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin
SetCompressor /SOLID lzma
SetCompressorDictSize 32
SetDatablockOptimize on
Unicode True

; Modern UI Configuration
!define MUI_ABORTWARNING
!define MUI_ICON "..\resources\honeypottr.ico"
!define MUI_UNICON "..\resources\honeypottr.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis3-metro.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\nsis3-metro.bmp"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "Welcome to ${PRODUCT_NAME} ${PRODUCT_VERSION} Setup"
!define MUI_WELCOMEPAGE_TEXT "${PRODUCT_NAME} is a production-grade Windows service that generates honeypot artifacts to deceive malware into self-termination.$\r$\n$\r$\nFeatures:$\r$\n  • Registry, filesystem, process, network, and hardware artifacts$\r$\n  • Automatic service with failure recovery$\r$\n  • Resource monitoring and limits$\r$\n  • Comprehensive logging$\r$\n  • Multiple deployment profiles$\r$\n$\r$\nClick Next to continue."
!insertmacro MUI_PAGE_WELCOME

; License page
!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "..\TERMS_OF_SERVICE.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Installation page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_TITLE "${PRODUCT_NAME} Installation Complete!"
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME} v${PRODUCT_VERSION} has been successfully installed.$\r$\n$\r$\n✓ Service installed and configured for automatic startup$\r$\n✓ Configuration files in: $INSTDIR\config$\r$\n✓ Logs will be written to: $INSTDIR\logs$\r$\n$\r$\nThe HoneyPottr service will start automatically on next boot.$\r$\nYou can start it now using: Services.msc$\r$\n$\r$\nClick Finish to complete setup."
!define MUI_FINISHPAGE_RUN "$INSTDIR\HoneyPottr.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--console"
!define MUI_FINISHPAGE_RUN_TEXT "Test in console mode (opens terminal)"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.md"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "View documentation"
!define MUI_FINISHPAGE_LINK "Visit project website"
!define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Version Information
VIProductVersion "${PRODUCT_VERSION}.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "Comments" "Lightweight honeypot artifact generator"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalTrademarks" ""
VIAddVersionKey "LegalCopyright" "© 2024 ${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileDescription" "${PRODUCT_NAME} Installer"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}.0"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}.0"

; Installer sections
Section "Core Application" SecCore
  SectionIn RO  ; Read-only section (always installed)
  
  DetailPrint "Installing HoneyPottr v${PRODUCT_VERSION}..."
  
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Copy main executable
  File "..\build\bin\Release\HoneyPottr.exe"
  
  ; Copy documentation files
  File "..\README.md"
  File "..\INSTALLATION.md"
  File "..\LICENSE"
  File "..\TERMS_OF_SERVICE.txt"
  File "..\PRODUCTION_SUMMARY.md"
  File "..\BUILD_INSTRUCTIONS.md"
  File "..\FINAL_STATUS.md"
  
  ; Create config directory and copy configuration files
  CreateDirectory "$INSTDIR\config"
  SetOutPath "$INSTDIR\config"
  SetOverwrite off  ; Don't overwrite existing config
  File "..\config\default_config.json"
  File "..\config\basic_profile.json"
  File "..\config\advanced_profile.json"
  
  ; Create logs directory with proper permissions
  CreateDirectory "$INSTDIR\logs"
  AccessControl::GrantOnFile "$INSTDIR\logs" "(S-1-5-18)" "FullAccess"  ; SYSTEM
  AccessControl::GrantOnFile "$INSTDIR\logs" "(S-1-5-32-544)" "FullAccess"  ; Administrators
  
  ; Create Start Menu folder and shortcuts
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME} Console.lnk" \
    "$INSTDIR\HoneyPottr.exe" "--console" "$INSTDIR\HoneyPottr.exe" 0 SW_SHOWNORMAL \
    "" "Run HoneyPottr in console mode for testing"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Configuration.lnk" \
    "notepad.exe" "$INSTDIR\config\default_config.json" "" 0 SW_SHOWNORMAL \
    "" "Edit HoneyPottr configuration"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\View Logs.lnk" \
    "explorer.exe" "$INSTDIR\logs" "" 0 SW_SHOWNORMAL \
    "" "Open log directory"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Manage Service.lnk" \
    "mmc.exe" "services.msc /s" "" 0 SW_SHOWNORMAL \
    "" "Open Windows Services to manage HoneyPottr"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Documentation.lnk" \
    "$INSTDIR\README.md" "" "" 0 SW_SHOWNORMAL \
    "" "View HoneyPottr documentation"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" \
    "$INSTDIR\uninst.exe" "" "" 0 SW_SHOWNORMAL \
    "" "Uninstall HoneyPottr"
  
  ; Application registry entries
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\HoneyPottr.exe"
  WriteRegStr HKLM "SOFTWARE\HoneyPottr" "InstallPath" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\HoneyPottr" "Version" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "SOFTWARE\HoneyPottr" "ConfigPath" "$INSTDIR\config\default_config.json"
  WriteRegStr HKLM "SOFTWARE\HoneyPottr" "LogPath" "$INSTDIR\logs"
  
  ; Uninstaller registry entries
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\HoneyPottr.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "${PRODUCT_WEB_SITE}/wiki"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLUpdateInfo" "${PRODUCT_WEB_SITE}/releases"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Comments" "Production honeypot artifact generator"
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" 1
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair" 1
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" 10240
  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninst.exe"
  
  DetailPrint "Core application installed successfully"
SectionEnd

Section "Windows Service (Recommended)" SecService
  SectionIn RO  ; Make service installation mandatory
  
  DetailPrint "Installing HoneyPottr Windows Service..."
  
  ; Stop service if it exists (ignore errors)
  nsExec::ExecToLog 'sc stop "${SERVICE_NAME}"'
  Sleep 2000
  
  ; Uninstall old service if exists (ignore errors)
  nsExec::ExecToLog 'sc delete "${SERVICE_NAME}"'
  Sleep 1000
  
  ; Install service using the exe
  DetailPrint "Creating service..."
  nsExec::ExecToLog '"$INSTDIR\HoneyPottr.exe" --install'
  Pop $0
  ${If} $0 != 0
    DetailPrint "Service creation returned code: $0 (may already exist)"
  ${EndIf}
  
  ; Configure service for automatic restart on failure
  DetailPrint "Configuring service recovery..."
  nsExec::ExecToLog 'sc failure "${SERVICE_NAME}" reset= 86400 actions= restart/5000/restart/10000/restart/30000'
  nsExec::ExecToLog 'sc config "${SERVICE_NAME}" start= delayed-auto'
  nsExec::ExecToLog 'sc description "${SERVICE_NAME}" "Production-grade honeypot service generating VM and analysis tool artifacts to deceive malware"'
  
  ; Set service SID type
  nsExec::ExecToLog 'sc sidtype "${SERVICE_NAME}" unrestricted'
  
  ; Start service
  DetailPrint "Starting HoneyPottr service..."
  nsExec::ExecToLog 'sc start "${SERVICE_NAME}"'
  Pop $0
  ${If} $0 == 0
    DetailPrint "✓ Service started successfully!"
    MessageBox MB_ICONINFORMATION "HoneyPottr service has been installed and started successfully.$\r$\n$\r$\nThe service will run automatically on system startup and generate honeypot artifacts to deceive malware."
  ${Else}
    DetailPrint "Service installed but not started (code: $0)"
    MessageBox MB_ICONINFORMATION "HoneyPottr service has been installed.$\r$\n$\r$\nIt will start automatically on next system boot.$\r$\nTo start now, use: Services.msc"
  ${EndIf}
SectionEnd

Section "Desktop Shortcuts" SecDesktop
  CreateShortCut "$DESKTOP\${PRODUCT_NAME} Console.lnk" "$INSTDIR\HoneyPottr.exe" "--console" \
    "$INSTDIR\HoneyPottr.exe" 0 SW_SHOWNORMAL "" "Test HoneyPottr in console mode"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME} Config.lnk" "notepad.exe" "$INSTDIR\config\default_config.json" \
    "" 0 SW_SHOWNORMAL "" "Edit HoneyPottr configuration"
SectionEnd

Section "Event Log Registration" SecEventLog
  DetailPrint "Registering Windows Event Log source..."
  nsExec::ExecToLog 'reg add "HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\HoneyPottr" /v EventMessageFile /t REG_SZ /d "$INSTDIR\HoneyPottr.exe" /f'
  nsExec::ExecToLog 'reg add "HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\HoneyPottr" /v TypesSupported /t REG_DWORD /d 7 /f'
  DetailPrint "Event Log source registered"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} "Core application files, configuration, and documentation (required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecService} "Windows service installation with automatic startup and failure recovery (required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Desktop shortcuts for quick access to console mode and configuration"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecEventLog} "Register Event Log source for logging to Windows Event Viewer"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Installer functions
Function .onInit
  ; Check if running as administrator
  UserInfo::GetAccountType
  pop $0
  ${If} $0 != "admin"
    MessageBox MB_ICONSTOP "Administrator privileges required. Please run as administrator."
    SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
  
  ; Check Windows version (Windows 10 or later)
  ${If} ${AtMostWin8.1}
    MessageBox MB_ICONSTOP "This application requires Windows 10 or later (64-bit).$\r$\n$\r$\nYour system is not supported."
    Quit
  ${EndIf}
  
  ; Check for 64-bit system
  ${IfNot} ${RunningX64}
    MessageBox MB_ICONSTOP "This application requires a 64-bit operating system.$\r$\n$\r$\nYour 32-bit system is not supported."
    Quit
  ${EndIf}
  
  ; Check if already installed
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" done
  
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "${PRODUCT_NAME} is already installed. Do you want to uninstall the previous version?" \
    IDYES uninst
  Abort
  
  uninst:
    ClearErrors
    ExecWait '$R0 /S _?=$INSTDIR'
    
    IfErrors no_remove_uninstaller done
    IfFileExists "$INSTDIR\uninst.exe" 0 no_remove_uninstaller
    Delete "$R0"
    RMDir "$INSTDIR"
    
  no_remove_uninstaller:
  done:
FunctionEnd

; Uninstaller section
Section Uninstall
  ; Stop and remove service
  DetailPrint "Stopping HoneyPottr service..."
  nsExec::ExecToLog 'sc stop "${SERVICE_NAME}"'
  Sleep 3000
  
  DetailPrint "Removing service..."
  nsExec::ExecToLog 'sc delete "${SERVICE_NAME}"'
  Sleep 1000
  
  ; Remove files and directories
  Delete "$INSTDIR\HoneyPottr.exe"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\INSTALLATION.md"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\TERMS_OF_SERVICE.txt"
  Delete "$INSTDIR\PRODUCTION_SUMMARY.md"
  Delete "$INSTDIR\BUILD_INSTRUCTIONS.md"
  Delete "$INSTDIR\FINAL_STATUS.md"
  Delete "$INSTDIR\uninst.exe"
  
  ; Ask before removing config (may contain customizations)
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "Do you want to keep your configuration files?$\r$\n$\r$\nSelect 'No' to remove all configuration." \
    IDYES keep_config
  RMDir /r "$INSTDIR\config"
  Goto config_done
  keep_config:
    DetailPrint "Configuration files preserved"
  config_done:
  
  ; Ask before removing logs
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "Do you want to keep log files for analysis?$\r$\n$\r$\nSelect 'No' to remove all logs." \
    IDYES keep_logs
  RMDir /r "$INSTDIR\logs"
  Goto logs_done
  keep_logs:
    DetailPrint "Log files preserved"
  logs_done:
  
  RMDir "$INSTDIR"
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME} Console.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Configuration.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\View Logs.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Manage Service.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Documentation.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  RMDir "$SMPROGRAMS\${PRODUCT_NAME}"
  Delete "$DESKTOP\${PRODUCT_NAME} Console.lnk"
  Delete "$DESKTOP\${PRODUCT_NAME} Config.lnk"
  
  ; Remove Event Log registration
  DeleteRegKey HKLM "SYSTEM\CurrentControlSet\Services\EventLog\Application\HoneyPottr"
  
  ; Remove registry entries
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey HKLM "SOFTWARE\HoneyPottr"
  
  ; Optional: Clean up honeypot artifacts
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "═══════════════════════════════════════════════$\r$\n\
     HONEYPOT ARTIFACT CLEANUP$\r$\n\
     ═══════════════════════════════════════════════$\r$\n$\r$\n\
     Do you want to remove the honeypot artifacts?$\r$\n$\r$\n\
     This will clean up:$\r$\n\
     • VMware registry keys and files$\r$\n\
     • VirtualBox registry keys and files$\r$\n\
     • Sandboxie registry keys$\r$\n\
     • Analysis tool signatures$\r$\n\
     • Fake drivers and shortcuts$\r$\n$\r$\n\
     Select YES to remove artifacts (recommended)$\r$\n\
     Select NO to leave artifacts in place" \
    IDNO skip_cleanup
  
  DetailPrint "Cleaning up honeypot artifacts..."
  
  ; Remove registry artifacts (all created by HoneyPottr)
  DetailPrint "Removing registry artifacts..."
  nsExec::ExecToLog 'reg delete "HKLM\SOFTWARE\VMware, Inc.\VMware Tools" /f'
  nsExec::ExecToLog 'reg delete "HKLM\SOFTWARE\Oracle\VirtualBox Guest Additions" /f'
  nsExec::ExecToLog 'reg delete "HKLM\SOFTWARE\Sandboxie" /f'
  nsExec::ExecToLog 'reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Wireshark" /f'
  nsExec::ExecToLog 'reg delete "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Process Monitor" /f'
  nsExec::ExecToLog 'reg delete "HKLM\SYSTEM\CurrentControlSet\Services\VBoxService" /f'
  nsExec::ExecToLog 'reg delete "HKLM\SYSTEM\CurrentControlSet\Services\VMTools" /f'
  
  ; Remove file artifacts
  DetailPrint "Removing file artifacts..."
  RMDir /r "$PROGRAMFILES64\VMware\VMware Tools"
  RMDir /r "$PROGRAMFILES64\Oracle\VirtualBox Guest Additions"
  RMDir /r "$PROGRAMFILES64\Sandboxie"
  RMDir /r "C:\Tools"
  RMDir /r "C:\ProgramData\SecurityTools"
  Delete "$WINDIR\System32\drivers\VBoxMouse.sys"
  Delete "$WINDIR\System32\drivers\vmhgfs.sys"
  Delete "$COMMONDESKTOP\Wireshark.lnk"
  Delete "C:\Users\Public\Desktop\Wireshark.lnk"
  
  DetailPrint "Artifact cleanup completed"
  
  skip_cleanup:
  
  SetAutoClose true
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "${PRODUCT_NAME} was successfully removed from your computer."
FunctionEnd

Function un.onInit
  ; Check if running as administrator
  UserInfo::GetAccountType
  pop $0
  ${If} $0 != "admin"
    MessageBox MB_ICONSTOP "Administrator privileges required. Please run as administrator."
    SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
  
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "Are you sure you want to completely remove ${PRODUCT_NAME} and all of its components?" \
    IDYES +2
  Abort
FunctionEnd
