#include <windows.h>

void ShowPopup() {
    MessageBoxA(NULL, "This is a scheduled popup!", "Popup Service", MB_OK | MB_ICONINFORMATION);  // Use MessageBoxA
}

int main() {
    while (true) {
        ShowPopup();
        Sleep(300000);  // Sleep for 1 minute (60000 milliseconds)
    }
    return 0;
}
