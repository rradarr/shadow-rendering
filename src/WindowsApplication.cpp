#include "stdafx.h"
#include "WindowsApplication.h"

#include "Engine.h"

HWND WindowsApplication::windowHandle = nullptr;
HCURSOR WindowsApplication::cursor = NULL;

int WindowsApplication::Run(Engine* gameEngine, HINSTANCE hInstance, int nCmdShow)
{
    // register window class
    const char windowClassName[] = "Game Window";

    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc     = WindowProc;
    windowClass.hInstance       = hInstance;
    windowClass.lpszClassName   = windowClassName;

    RegisterClass(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(gameEngine->GetWidth()), static_cast<LONG>(gameEngine->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window
    windowHandle = CreateWindowEx(
        0,                              // Optional window styles
        windowClassName,
        gameEngine->GetTitle().c_str(),     // Window text
        WS_OVERLAPPEDWINDOW,                // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,       // Window position
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, // Window size
        NULL,                               // Parent window
        NULL,                               // Menu
        hInstance,                          // Instance handle
        gameEngine                          // Additional application data
    );

    if (windowHandle == NULL)
    {
        // raise some exception or smthn
    }

    ShowWindow(windowHandle, nCmdShow);

    gameEngine->OnInit(windowHandle);

    // run the message loop
    MSG msg = { };
    bool running = true;
    while (running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                running = false;
            if (msg.message == WM_PAINT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);  // this will call the WindowProc callback
        }
        gameEngine->OnUpdate();
        gameEngine->OnRender();
    }

    gameEngine->OnDestroy();

    // Return this part of the WM_QUIT message to Windows.
    return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK WindowsApplication::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Engine* gameEngine = reinterpret_cast<Engine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg)
    {
    case WM_CREATE:
        {
            // Save the DXSample* passed in to CreateWindow.
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        }
        return 0;

    case WM_KEYDOWN:
        gameEngine->OnKeyDown(static_cast<UINT8>(wParam));
        return 0;

    case WM_KEYUP:
        gameEngine->OnKeyUp(static_cast<UINT8>(wParam));
        return 0;

    case WM_MOUSEMOVE:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            gameEngine->OnMouseMove(xPos, yPos);
        }
        return 0;

    case WM_SETFOCUS:
        {
            gameEngine->OnGotFocus();
            cursor = GetCursor();
            SetCursor(NULL);
        }
        return 0;

    case WM_KILLFOCUS:
        {
            gameEngine->OnLostFocus();
            SetCursor(cursor);
        }
        return 0;

    case WM_PAINT:
        {
            // update and render the game


            // if I don't paint, WM_PAINT shows up constantly
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hwnd, &ps);

            //// All painting occurs here, between BeginPaint and EndPaint.
            //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

            //EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
