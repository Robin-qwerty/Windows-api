#include <windows.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>

SERVICE_STATUS_HANDLE g_StatusHandle;
int fileCounter = 1;

// Renamed macro to avoid conflict with function GetCurrentTime
#define GetSystemTickCount() GetTickCount()

void WINAPI ServiceMain(DWORD argc, LPSTR* argv);
void WINAPI ServiceCtrlHandler(DWORD ctrlCode);
void ReportStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint);

// Renamed the function to avoid conflict with the macro
std::string GetFormattedCurrentTime() {
    std::time_t now = std::time(nullptr);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    return std::string(buffer);
}

void WriteStatusToFile() {
    std::ostringstream fileName;
    fileName << "C:\\Users\\Public\\Desktop\\IAmWatchingU" << fileCounter++ << ".txt";

    std::ofstream file(fileName.str(), std::ios::app);
    if (file.is_open()) {
        file << "(+) " << GetFormattedCurrentTime() << " - I am watching u" << std::endl;
        file.close();
    }
}

int main() {
    SERVICE_TABLE_ENTRYA serviceTable[] = {
        {"MyService", ServiceMain},
        {nullptr, nullptr}
    };

    if (!StartServiceCtrlDispatcherA(serviceTable)) {
        return GetLastError();
    }

    return 0;
}

void WINAPI ServiceMain(DWORD argc, LPSTR* argv) {
    g_StatusHandle = RegisterServiceCtrlHandlerA("MyService", ServiceCtrlHandler);
    if (!g_StatusHandle) {
        return;
    }

    ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

    while (true) {
        WriteStatusToFile();

        // Sleep for 30 minutes
        Sleep(30 * 60 * 1000);
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

    SERVICE_STATUS status = {};
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = currentState;
    status.dwControlsAccepted = (currentState == SERVICE_RUNNING) ? SERVICE_ACCEPT_STOP : 0;
    status.dwWin32ExitCode = win32ExitCode;
    status.dwWaitHint = waitHint;
    status.dwCheckPoint = (currentState == SERVICE_START_PENDING || currentState == SERVICE_STOP_PENDING) ? checkPoint++ : 0;

    SetServiceStatus(g_StatusHandle, &status);
}
