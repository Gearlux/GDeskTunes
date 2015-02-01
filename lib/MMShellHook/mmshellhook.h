#ifndef MMSHELLHOOK_H
#define MMSHELLHOOK_H

#include "mmshellhook_global.h"

#include "windows.h"

extern "C"
{
    // DLL functions:
    // Set the hook
    MMSHELLHOOKSHARED_EXPORT BOOL SetMMShellHook(HWND hWnd	);
    // Remove it (1: success, 0: no success, -1: hWnd is null)
    MMSHELLHOOKSHARED_EXPORT int UnSetMMShellHook(HWND hWnd);
}

#endif // MMSHELLHOOK_H
