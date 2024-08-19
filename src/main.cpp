#include "stdafx.hpp"

#include "ConsoleHelper.hpp"
#include "VoyagerEngine.hpp"
#include "WindowsApplication.hpp"

_Use_decl_annotations_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#ifdef _DEBUG
    if (!CreateNewConsole(1024))
        return -1;
#endif // _DEBUG

    std::cout << "Hello World!" << std::endl;

    VoyagerEngine voyager(1500, 1000, "Voyager Game");
    int returnCode = WindowsApplication::Run(&voyager, hInstance, nCmdShow);

    return returnCode;
}