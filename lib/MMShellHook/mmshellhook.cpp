// Code taken from
// http://www.codeguru.com/cpp/w-p/system/keyboard/article.php/c5655/Using-Multimedia-Keyboard-Keys-in-Your-Own-Program.htm

#include "mmshellhook.h"

#include <tchar.h>
#include <windows.h>
#include <winuser.h>

#include <crtdbg.h>
#include <iostream>
#include <fstream>
/////////////////////////////////////////////////////////////////////////////
// Storage for the global data in the DLL

#pragma data_seg(".shared")
HWND hNotifyWnd = NULL;
HHOOK hShellHook = NULL;							// Handle to the Shell hook
#pragma data_seg( )
/////////////////////////////////////////////////////////////////////////////

// Per-instance DLL variables
HINSTANCE hInstance = NULL;							// This instance of the DLL

LRESULT CALLBACK ShellProc (int nCode, WPARAM wParam, LPARAM lParam);

// The DLL's main procedure
BOOL WINAPI DllMain (HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    // Find out why we're being called
    switch (ul_reason_for_call)
    {

    case DLL_PROCESS_ATTACH:
        std::cout << "MMShellHook : Hook DLL loaded" << std::endl;
        // Save the instance handle
        hInstance = (HINSTANCE)hInst;
        // ALWAYS return TRUE to avoid breaking unhookable applications!!!
        return TRUE;

    case DLL_PROCESS_DETACH:
        // qDebug() << "MMShellHook : Hook DLL unloaded";
        return TRUE;
    default:
        return TRUE;
    }
}

// Add the new hook

MMSHELLHOOKSHARED_EXPORT BOOL SetMMShellHook(HWND hWnd)
{
    std::cout << "SetMMShellHook(" << hWnd << ")" << std::endl;

    // Don't add the hook if the window ID is NULL
    if (hWnd == NULL)
        return FALSE;

    // Don't add a hook if there is already one added
    if (hNotifyWnd != NULL)
        return FALSE;

    // Add the ShellProc hook
    hShellHook = SetWindowsHookEx(
                    WH_SHELL,					// Hook in before msg reaches app
                    (HOOKPROC) ShellProc,			// Hook procedure
                    hInstance,						// This DLL instance
                    0L								// Hook in to all apps
                    );


    // Check that it worked
    if (hShellHook != NULL)
    {

        hNotifyWnd = hWnd;						// Save the WinRFB window handle

        std::cout << "Notify Window is" << hWnd << std::endl;
        return TRUE;
    }
    // The hook failed, so return an error code
    return FALSE;
}


// Remove the hook from the system
MMSHELLHOOKSHARED_EXPORT int UnSetMMShellHook(HWND hWnd)
{
    std::cout << "UnSetMMShellHook(" << hWnd << ")" << std::endl;
    BOOL unHooked = TRUE;

    // Is the window handle valid?
    if (hWnd == NULL)
        return -1;

    // Is the correct application calling UnSetHook?
    if (hWnd != hNotifyWnd)
        return 0;
    // Unhook the procs
    if (hNotifyWnd != NULL)
    {
        unHooked = UnhookWindowsHookEx(hShellHook);
        hShellHook = NULL;
    }
    // If we managed to unhook then reset
    if (unHooked)
    {
        hNotifyWnd = NULL;
    }

    return unHooked;

}



// Hook procedure for Shell hook

LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // std::ofstream debug;
    // debug.open("c:\\mmgdeskhook.txt", std::ios::app);
    // debug << "ShellProc(" << nCode << "," << wParam << "," << lParam << ")" << std::endl;
    // Do we have to handle this message?
    if (nCode == HSHELL_APPCOMMAND)
    {
        // qDebug() << "Received HSHELL_APPCOMANND";

        // Process the hook if the hNotifyWnd window handle is valid
        if (hNotifyWnd != NULL)
        {
            short AppCommand = GET_APPCOMMAND_LPARAM(lParam);
            switch (AppCommand)
            {
            case APPCOMMAND_MEDIA_NEXTTRACK:
            case APPCOMMAND_MEDIA_PLAY_PAUSE:
            case APPCOMMAND_MEDIA_PREVIOUSTRACK:
            case APPCOMMAND_MEDIA_STOP:
                // debug << "PostMessage: WM_APPCOMMAND" << std::endl;
                ::PostMessage(hNotifyWnd,WM_APPCOMMAND,wParam,lParam);
                // debug.close();
                return 1; // dont call CallNextHookEx, instead return non-zero, because we have handled the message (see MSDN doc)

            }
        }
    }

    // debug.close();
    // Call the next handler in the chain
    return CallNextHookEx (hShellHook, nCode, wParam, lParam);
}
