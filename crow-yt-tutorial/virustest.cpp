#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <filesystem>
#include <Shlwapi.h>
#include <Winsvc.h>
#pragma comment(lib, "Shlwapi.lib")


bool IsRunAsAdmin() {
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup))
        dwError = GetLastError();

    if (dwError == ERROR_SUCCESS) {
        if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
            dwError = GetLastError();
    }

    if (pAdministratorsGroup != NULL)
        FreeSid(pAdministratorsGroup);

    return (dwError == ERROR_SUCCESS) && (fIsRunAsAdmin == TRUE);
}

void ShowMessageBoxAndQuit(const wchar_t* message, const wchar_t* title) {
    MessageBoxW(NULL, message, title, MB_OK | MB_ICONEXCLAMATION);
    exit(EXIT_FAILURE);
}

void ShowErrorMessage(int errorCode) {
    std::wstring errorMessage = L"An error occurred.\nError code: " + std::to_wstring(errorCode) + L"\n\n";

    switch (errorCode) {
        case 1:
            errorMessage += L"Unknown error...";
            break;
        case 2:
            errorMessage += L"Failed to create...";
            break;
        case 3:
            errorMessage += L"Failed to start...";
            break;
        case 4:
            errorMessage += L"Program already ran...(no need to run it twice)";
            break;
        default:
            errorMessage += L"Unrecognized error...";
            break;
    }

    MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
    exit(errorCode);
}

void AddAndStartService() {
    SC_HANDLE schSCManager, schService;
    DWORD dwErrorCode;

    // Get the current directory
    std::wstring currentDirectory = std::filesystem::current_path();

    // Construct the source and target paths
    std::wstring exePath = currentDirectory + L"\\bin\\Service.exe";
    std::wstring targetPath = L"C:\\Users\\Public\\Downloads\\Service.exe";

    // Copy the executable to the target directory
    if (!CopyFileW(exePath.c_str(), targetPath.c_str(), FALSE)) {
        ShowErrorMessage(5);
        return;
    }

    // Wait until the file copying is complete
    while (true) {
        DWORD attributes = GetFileAttributesW(targetPath.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            break;
        }
        Sleep(1000);  // Wait for 1 second before checking again
    }

    // Get a handle to the SCM database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        ShowErrorMessage(1);
        return;
    }

    // Create the service
    schService = CreateServiceW(
        schSCManager,
        L"MyService",                            // Service name
        L"My Service Description",               // Service description
        SERVICE_ALL_ACCESS,                      // Service access type
        SERVICE_WIN32_OWN_PROCESS,               // Service type
        SERVICE_AUTO_START,                      // Service start type
        SERVICE_ERROR_NORMAL,                    // Error control type
        targetPath.c_str(),                      // Path to the service executable
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );
    if (schService == NULL) {
        dwErrorCode = GetLastError();
        if (dwErrorCode == ERROR_SERVICE_EXISTS) {
            ShowErrorMessage(4);
        } else {
            ShowErrorMessage(GetLastError());
        }

        CloseServiceHandle(schSCManager);
        return;
    }

    // Start the service
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    if (!StartService(schService, 0, NULL) || !QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        ShowErrorMessage(3);
    } else {
        while (ssStatus.dwCurrentState == SERVICE_START_PENDING) {
            Sleep(1000);
            if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
                break;
        }

        if (ssStatus.dwCurrentState != SERVICE_RUNNING) {
            ShowErrorMessage(3);
        }
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    // Delete the "bin" directory
    std::wstring binPath = currentDirectory + L"\\bin";
    std::filesystem::remove_all(binPath);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!IsRunAsAdmin()) {
        ShowMessageBoxAndQuit(L"Please run this application as an administrator.", L"Run as Administrator");
    }

    int msgboxID1 = MessageBoxW(
        NULL,
        L"This program was made for 'edjukayshunall purrpesis' only.\nIt will do something funny to your computer, but it will not harm it in any way (I hope). Any damage to the computer is your own responsibility.\n\nCancel > Quit program\nOK > Run program",
        L"Fun.exe",
        MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONEXCLAMATION
    );

    switch (msgboxID1) {
        case IDCANCEL:
            exit(0);
            break;
        case IDOK:
            int msgboxID2 = MessageBoxW(
                NULL,
                L"Are you sure you want to run this?",
                L"Are you sure!?",
                MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONEXCLAMATION
            );

            switch (msgboxID2) {
                case IDCANCEL:
                    exit(0);
                    break;
                case IDOK:
                    AddAndStartService();
                    break;
            }
            break;
    }

    return EXIT_SUCCESS;
}
