#include <stdio.h>
#include <windows.h>

int main(void) { // creating a message box that says hello world
    
    int msgboxID = MessageBoxW(
        NULL,
        L"My first message",
        L"Hello world!",
        MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONEXCLAMATION 
    );

    switch (msgboxID)
    {
    case IDCANCEL:
        // TODO: add code
        printf("(+) Button clicked: %ld", msgboxID);
        break;
    case IDOK:
        // TODO: add code
        printf("(+) Button clicked: %ld", msgboxID);
        break;
    }

    return EXIT_SUCCESS;
    
}