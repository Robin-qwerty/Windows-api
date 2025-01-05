#include <stdio.h>
#include <Windows.h>

int main(void) { // creating a process that start mspaint

    // BOOL CreateProcessW(
    //     [in, optional]      LPCWSTR               lpApplicationName,
    //     [in, out, optional] LPWSTR                lpCommandLine,
    //     [in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
    //     [in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,
    //     [in]                BOOL                  bInheritHandles,
    //     [in]                DWORD                 dwCreationFlags,
    //     [in, optional]      LPVOID                lpEnvironment,
    //     [in, optional]      LPCWSTR               lpCurrentDirectory,
    //     [in]                LPSTARTUPINFOW        lpStartupInfo,
    //     [out]               LPPROCESS_INFORMATION lpProcessInformation
    // );

    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    if (!CreateProcessW(
        L"C:\\Windows\\System32\\mspaint.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        BELOW_NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &si,
        &pi)) {
        printf("(-) failed to create process, error %ld", GetLastError());
        return EXIT_FAILURE;
    }

    printf("(+) process started! PID: %ld", pi.dwProcessId);
    return EXIT_SUCCESS;
}