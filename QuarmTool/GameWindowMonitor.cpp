#include "GameWindowMonitor.h"
#include "QuarmTool.h"
#include <TlHelp32.h>
#include <string>

std::string GetWindowTitle(HWND hWnd)
{
    const int bufferSize = 1024; // Adjust the buffer size as needed
    char buffer[bufferSize];
    GetWindowTextA(hWnd, buffer, bufferSize);
    return std::string(buffer);
}

// Callback function used by EnumWindows
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return true;
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId == static_cast<DWORD>(lParam))
    {
        // Process the window here
        if (GetWindowTitle(hwnd).find("Client") != std::string::npos)
        {
            qt->pGameWindow->Handle=hwnd;
        }
    }

    return TRUE; // Continue enumeration
}


// Function to enumerate all windows for a given process ID
void EnumerateWindowsForProcess(DWORD processId)
{
    EnumWindows(EnumWindowsCallback, static_cast<LPARAM>(processId));
}


void UpdateGameWindowHandle(const std::wstring& processName)
{
    DWORD processId = 0;
    // Create a snapshot of all processes in the system
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W processEntry;  // Use PROCESSENTRY32W for wide-character strings
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        // Iterate through processes in the snapshot
        if (Process32FirstW(snapshot, &processEntry))
        {
            do
            {
                // Check if the current process matches the specified process name
                if (_wcsicmp(processEntry.szExeFile, processName.c_str()) == 0)
                {
                    processId = processEntry.th32ProcessID;
                    //break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }

        // Close the snapshot handle
        CloseHandle(snapshot);
    }

    // Get the main window handle of the specified process
    if (processId != 0)
    {
        EnumerateWindowsForProcess(processId);
    }
}



void GameWindowMonitor::monitor_process()
{
    while (!end_thread)
    {
        UpdateGameWindowHandle(L"eqgame.exe");
        if (Handle)
        {
            GetWindowRect(Handle, &Rect);
            Width = Rect.right - Rect.left;
            Height = Rect.bottom - Rect.top;
        }
        Sleep(1000);
    }
}
GameWindowMonitor::GameWindowMonitor()
{
    monitor_process_thread = std::thread(&GameWindowMonitor::monitor_process, this);
    monitor_process_thread.detach();
}
GameWindowMonitor::~GameWindowMonitor()
{
    end_thread = true;
    if (monitor_process_thread.joinable())
        monitor_process_thread.join();
}