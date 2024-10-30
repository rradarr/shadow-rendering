#include "stdafx.hpp"

#include "ConsoleHelper.hpp"
#include "VoyagerEngine.hpp"
#include "WindowsApplication.hpp"

_Use_decl_annotations_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#ifdef _DEBUG
    if (!CreateNewConsole(1024))
        return -1;
    std::cout << "Debug enabled." << std::endl;    
#endif // _DEBUG

    std::cout << "Starting application..." << std::endl;

    // Screen resolutions: (default: 1500, 1000)
    // 1280, 720
    // 1920, 1080
    // 2560, 1440
    VoyagerEngine voyager(1920, 1080, "Shadow Rendering");
    int returnCode = WindowsApplication::Run(&voyager, hInstance, nCmdShow);

    return returnCode;
}