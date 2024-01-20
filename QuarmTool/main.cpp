#include "UI.h"
#include "QuarmTool.h"
#ifdef _WINDLL

HANDLE hCurrentUIThread = nullptr;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        UI::hCurrentModule = hinstDLL;
        hCurrentUIThread = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)UI::Render, nullptr, NULL, nullptr);
    }

    if (fdwReason == DLL_PROCESS_DETACH)
        TerminateThread(hCurrentUIThread, 0);

    return TRUE;
}

#else



class OutputDebugStringStreamBuf : public std::stringbuf
{
public:
    virtual int sync()
    {
        OutputDebugStringA(str().c_str());
        str("");
        return 0;
    }
};


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{

    OutputDebugStringStreamBuf debugStreamBuf;
    std::ostream debugStream(&debugStreamBuf);
    std::cout.rdbuf(&debugStreamBuf);
    std::cout << "Hello, OutputDebugString!" << std::endl;

    UI::Render();

    return 0;
}

#endif