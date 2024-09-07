#pragma once

#include <mutex>

#include "EngineStateModel.hpp"

/* Is the delegate for input from the window, such as keyboard and mouse.
Handles the input with logic and stores the results in a model. */
class MainInputController {
private:
    static MainInputController* instance;
    static std::mutex mutex;

protected:
    // MainInputController() = delete;
    MainInputController(EngineStateModel* model);
    ~MainInputController() { delete instance; }

    EngineStateModel* stateModel;
    InputState& modelInputState;

    RenderingState::RenderingMode lastRenderingMode;

public:
    MainInputController(MainInputController& other) = delete;
    void operator=(const MainInputController&) = delete;

    static MainInputController* GetInstance();

    void HandleKeyDown(UINT8 keyCode);
    void HandleKeyUp(UINT8 keyCode);
    void HandleMouseMove(int mouseX, int mouseY);

    /* Process mouse delta and position cursor if needed. */
    void EarlyUpdate(HWND windowHandle);
};
