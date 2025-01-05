#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

// Helper function to convert std::string to std::wstring
std::wstring StringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

// Function to install the service
bool InstallService(const std::string& serviceName, const std::string& servicePath) {
    std::wstring wServiceName = StringToWString(serviceName);
    std::wstring wServicePath = StringToWString(servicePath);

    // Open the Service Control Manager
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm) {
        std::cerr << "Failed to open the Service Control Manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Create the service
    SC_HANDLE service = CreateServiceW(
        scm,
        wServiceName.c_str(),
        wServiceName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        wServicePath.c_str(),
        NULL, NULL, NULL, NULL, NULL);

    if (!service) {
        std::cerr << "Failed to create the service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }

    std::cout << "Service installed successfully!" << std::endl;

    // Start the service
    if (!StartService(service, 0, NULL)) {
        std::cerr << "Failed to start the service. Error: " << GetLastError() << std::endl;
    } else {
        std::cout << "Service started successfully!" << std::endl;
    }

    // Clean up
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

int main() {
    // Get the current executable's path
    char currentPath[MAX_PATH];
    GetModuleFileNameA(NULL, currentPath, MAX_PATH);

    // Extract the directory path
    std::string directoryPath = std::string(currentPath);
    size_t lastSlash = directoryPath.find_last_of("\\/");
    directoryPath = directoryPath.substr(0, lastSlash + 1);

    // Define the name for the new .exe file
    std::string newExeName = "PopupService.exe";
    std::string newExePath = directoryPath + newExeName;

    // Code for the new .exe file (popup every minute)
    const char* exeCode = R"code(
#include <windows.h>
#include <thread>
#include <chrono>

void ShowPopup() {
    MessageBox(NULL, "This is a scheduled popup!", "Popup Service", MB_OK | MB_ICONINFORMATION);
}

int main() {
    while (true) {
        ShowPopup();
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
    return 0;
}
)code";

    // Save the code to a temporary file
    std::string tempCodeFile = directoryPath + "temp_code.cpp";
    std::ofstream outFile(tempCodeFile);
    if (!outFile) {
        std::cerr << "Failed to create the temporary file for the new .exe." << std::endl;
        return 1;
    }
    outFile << exeCode;
    outFile.close();

    // Compile the temporary file into a new .exe using GCC
    std::string compileCommand = "g++ " + tempCodeFile + " -o " + newExePath + " -mwindows -static";
    int result = system(compileCommand.c_str());

    // Check if the compilation succeeded
    if (result == 0) {
        std::cout << "Successfully created: " << newExePath << std::endl;
    } else {
        std::cerr << "Compilation failed. Make sure GCC is installed and in the PATH." << std::endl;
        return 1;
    }

    // Clean up the temporary file
    if (remove(tempCodeFile.c_str()) != 0) {
        std::cerr << "Failed to remove the temporary file." << std::endl;
    }

    // Install and start the service
    std::string serviceName = "PopupService";
    if (!InstallService(serviceName, newExePath)) {
        std::cerr << "Failed to install the service." << std::endl;
        return 1;
    }

    return 0;
}
