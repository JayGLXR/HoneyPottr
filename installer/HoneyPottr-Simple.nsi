; HoneyPottr Simple NSIS Installer Script
; Copyright (c) 2024 Calvexa Group, LLC
; Licensed under MIT License

!define PRODUCT_NAME "HoneyPottr"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Calvexa Group, LLC"
!define PRODUCT_WEB_SITE "https://github.com/calvexa/honeypottr"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\HoneyPottr.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define SERVICE_NAME "HoneyPottr"

; Modern UI
!include "MUI2.nsh"

; Installer settings
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "HoneyPottr-Installer-v${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES64\HoneyPottr"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin
SetCompressor lzma

; Modern UI Configuration
!define MUI_ABORTWARNING
!define MUI_ICON "..\resources\honeypottr.ico"
!define MUI_UNICON "..\resources\honeypottr.ico"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "Welcome to ${PRODUCT_NAME} Setup"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${PRODUCT_NAME}.$\r$\n$\r$\n${PRODUCT_NAME} is a lightweight honeypot artifact generator that creates system artifacts to deceive malware into self-termination.$\r$\n$\r$\nThis software is provided free of charge by ${PRODUCT_PUBLISHER} under the MIT License.$\r$\n$\r$\nClick Next to continue."
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
!define MUI_FINISHPAGE_TITLE "Installation Complete"
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME} has been successfully installed.$\r$\n$\r$\nThe service has been installed and will start automatically on system boot.$\r$\n$\r$\nClick Finish to complete the installation."
!define MUI_FINISHPAGE_RUN "$INSTDIR\HoneyPottr.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--console"
!define MUI_FINISHPAGE_RUN_TEXT "Test ${PRODUCT_NAME} in console mode"
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
  
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Copy main executable
  File "..\build-windows\bin\HoneyPottr.exe"
  
  ; Copy documentation
  File "..\README.md"
  File "..\INSTALLATION.md"
  File "..\ARCHITECTURE.md"
  File "..\LICENSE"
  File "..\TERMS_OF_SERVICE.txt"
  
  ; Create config directory and copy configuration files
  CreateDirectory "$INSTDIR\config"
  SetOutPath "$INSTDIR\config"
  File "..\config\default_config.json"
  File "..\config\basic_profile.json"
  File "..\config\advanced_profile.json"
  
  ; Create logs directory
  CreateDirectory "$INSTDIR\logs"
  
  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME} Console.lnk" "$INSTDIR\HoneyPottr.exe" "--console"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Documentation.lnk" "$INSTDIR\README.md"
  
  ; Registry entries
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\HoneyPottr.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\HoneyPottr.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" 1
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair" 1
  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninst.exe"
SectionEnd

Section "Windows Service" SecService
  DetailPrint "Installing Windows Service..."
  
  ; Stop service if it exists
  ExecWait 'net stop "${SERVICE_NAME}"' $0
  
  ; Install service
  ExecWait '"$INSTDIR\HoneyPottr.exe" --install' $0
  StrCmp $0 "0" service_install_ok
    MessageBox MB_ICONEXCLAMATION "Failed to install Windows service. Error code: $0"
    Goto service_error
  
  service_install_ok:
  ; Start service
  ExecWait 'net start "${SERVICE_NAME}"' $0
  StrCmp $0 "0" service_start_ok
    MessageBox MB_ICONEXCLAMATION "Service installed but failed to start. You can start it manually from Services.msc"
    Goto service_done
  
  service_start_ok:
    DetailPrint "Service installed and started successfully"
    Goto service_done
  
  service_error:
    DetailPrint "Service installation failed"
  
  service_done:
SectionEnd

Section "Desktop Shortcut" SecDesktop
  CreateShortCut "$DESKTOP\${PRODUCT_NAME} Console.lnk" "$INSTDIR\HoneyPottr.exe" "--console"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} "Core application files and configuration (required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecService} "Install and start as Windows service for automatic operation"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Create desktop shortcut for console mode testing"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Installer functions
Function .onInit
  ; Check if running as administrator
  UserInfo::GetAccountType
  pop $0
  StrCmp $0 "admin" admin_ok
    MessageBox MB_ICONSTOP "Administrator privileges required. Please run as administrator."
    SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
    Quit
  admin_ok:
  
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
  DetailPrint "Stopping service..."
  ExecWait 'net stop "${SERVICE_NAME}"' $0
  
  DetailPrint "Removing service..."
  ExecWait '"$INSTDIR\HoneyPottr.exe" --uninstall' $0
  
  ; Remove files and directories
  Delete "$INSTDIR\HoneyPottr.exe"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\INSTALLATION.md"
  Delete "$INSTDIR\ARCHITECTURE.md"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\TERMS_OF_SERVICE.txt"
  Delete "$INSTDIR\uninst.exe"
  
  RMDir /r "$INSTDIR\config"
  RMDir /r "$INSTDIR\logs"
  RMDir "$INSTDIR"
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME} Console.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Documentation.lnk"
  RMDir "$SMPROGRAMS\${PRODUCT_NAME}"
  Delete "$DESKTOP\${PRODUCT_NAME} Console.lnk"
  
  ; Remove registry entries
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  
  ; Optional: Clean up artifacts
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "Do you want to remove the honeypot artifacts created by ${PRODUCT_NAME}?$\r$\n$\r$\nThis will clean up registry entries and files that were created to simulate honeypot environments." \
    IDNO skip_cleanup
  
  DetailPrint "Cleaning up artifacts..."
  
  ; Remove registry artifacts
  DeleteRegKey HKLM "SOFTWARE\VMware, Inc.\VMware Tools"
  DeleteRegKey HKLM "SOFTWARE\Oracle\VirtualBox Guest Additions"
  DeleteRegKey HKLM "SOFTWARE\Sandboxie"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Wireshark"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Process Monitor"
  DeleteRegKey HKLM "SYSTEM\CurrentControlSet\Services\VBoxService"
  DeleteRegKey HKLM "SYSTEM\CurrentControlSet\Services\VMTools"
  
  ; Remove file artifacts
  RMDir /r "$PROGRAMFILES64\VMware\VMware Tools"
  RMDir /r "$PROGRAMFILES64\Oracle\VirtualBox Guest Additions"
  RMDir /r "$PROGRAMFILES64\Wireshark"
  RMDir /r "$PROGRAMFILES64\Sandboxie"
  RMDir /r "C:\Tools"
  Delete "$WINDIR\System32\drivers\VBoxMouse.sys"
  Delete "$WINDIR\System32\drivers\vmhgfs.sys"
  Delete "$COMMONDESKTOP\Wireshark.lnk"
  
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
  StrCmp $0 "admin" admin_ok
    MessageBox MB_ICONSTOP "Administrator privileges required. Please run as administrator."
    SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
    Quit
  admin_ok:
  
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
    "Are you sure you want to completely remove ${PRODUCT_NAME} and all of its components?" \
    IDYES +2
  Abort
FunctionEnd
