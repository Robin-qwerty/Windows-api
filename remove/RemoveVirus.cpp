#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>

const std::string code = "12341";

int main() {
    const char* serviceName = "MyService";

    std::string enteredCode;
    std::cout << "Enter the code: ";
    std::cin >> enteredCode;

    if (enteredCode != code) {
        std::cout << "Incorrect code entered." << std::endl;
        return 0;
    }

    SC_HANDLE scmHandle = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (scmHandle == nullptr) {
        std::cout << "Failed to open the Service Control Manager." << std::endl;
        return 1;
    }

    SC_HANDLE serviceHandle = OpenServiceA(scmHandle, serviceName, SERVICE_STOP | DELETE);
    if (serviceHandle == nullptr) {
        std::cout << "Failed to open the service." << std::endl;
        CloseServiceHandle(scmHandle);
        return 1;
    }

    SERVICE_STATUS serviceStatus;
    if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus)) {
        std::cout << "Failed to stop the service." << std::endl;
    }
    else {
        std::cout << "The service has been stopped." << std::endl;
    }

    if (!DeleteService(serviceHandle)) {
        std::cout << "Failed to delete the service." << std::endl;
        CloseServiceHandle(serviceHandle);
        CloseServiceHandle(scmHandle);
        return 1;
    }

    std::cout << "The service has been successfully removed." << std::endl;

    CloseServiceHandle(serviceHandle);
    CloseServiceHandle(scmHandle);

    // Get the current module handle
    HMODULE moduleHandle = GetModuleHandle(nullptr);

    // Get the path of the executable
    char modulePath[MAX_PATH];
    GetModuleFileName(moduleHandle, modulePath, MAX_PATH);

    // Schedule the executable file to be deleted during the next system boot
    if (!MoveFileEx(modulePath, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT)) {
        std::cout << "Failed to schedule the file for deletion." << std::endl;
        return 1;
    }

    // Spawn a separate application to display a message
    std::string message = "The service has been removed successfully.";
    std::string title = "Service Removal";
    std::string command = "cmd /c echo msgbox(\"" + message + "\", vbInformation, \"" + title + "\") > temp.vbs && start temp.vbs";
    system(command.c_str());

    // Specify the path of the file to be deleted
    std::string filePath = "C:\\Users\\Public\\Downloads\\Service.exe";

    // Delete the file
    if (!DeleteFileA(filePath.c_str())) {
        std::cout << "Failed to delete the file." << std::endl;
        return 1;
    }

    std::cout << "The file has been successfully deleted." << std::endl;

    return 0;
}
