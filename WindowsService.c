#include <windows.h>
#include <stdio.h>

SERVICE_STATUS_HANDLE g_StatusHandle;

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD ctrlCode);
void ReportStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint);

int main(int argc, char *argv[]) {
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { TEXT("MyService"), ServiceMain },
        { NULL, NULL }
    };
    if (!StartServiceCtrlDispatcher(serviceTable)) {
        printf("StartServiceCtrlDispatcher failed: %d\n", GetLastError());
        return 1;
    }
    return 0;
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
    g_StatusHandle = RegisterServiceCtrlHandler(TEXT("MyService"), ServiceCtrlHandler);
    if (!g_StatusHandle) {
        return;
    }
    ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
    ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

    while (1) {
        FILE *fp = fopen("C:\\Users\\robin\\OneDrive\\Documents\\code\\windows-api\\WindowsServiceStatus.txt", "w");
        if (fp != NULL) {
            fprintf(fp, "Service is running\n");
            fclose(fp);
        }
        Sleep(5000);
    }
}

void WINAPI ServiceCtrlHandler(DWORD ctrlCode) {
    switch (ctrlCode) {
        case SERVICE_CONTROL_STOP:
            ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
            ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
            break;
        default:
            break;
    }
}

void ReportStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint) {
    static DWORD checkPoint = 1;
    SERVICE_STATUS status = {
        .dwServiceType = SERVICE_WIN32_OWN_PROCESS,
        .dwCurrentState = currentState,
        .dwControlsAccepted = SERVICE_ACCEPT_STOP,
        .dwWin32ExitCode = win32ExitCode,
        .dwWaitHint = waitHint
    };
    if (currentState == SERVICE_START_PENDING) {
        status.dwControlsAccepted = 0;
    }
    if (currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED) {
        status.dwCheckPoint = 0;
    } else {
        status.dwCheckPoint = checkPoint++;
    }
    SetServiceStatus(g_StatusHandle, &status);
}
