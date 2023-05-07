#include <windows.h>

int main(void) { // creating a message box that says hello world
    
    MessageBoxW(
        NULL,
        L"My first message",
        L"Hello world!",
        MB_YESNOCANCEL | MB_ICONEXCLAMATION 
    );

    return EXIT_SUCCESS;
    
}