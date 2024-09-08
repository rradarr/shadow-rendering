#pragma once

#include <mutex>

struct RenderingState {
    enum class RenderingMode {
        DEFAULT = 0,
        WIREFRAME = 1,
        NORMALS_DEBUG = 2,
    };
    RenderingMode chosenRenderingMode = RenderingMode::DEFAULT;
    static const char* renderingModeNames[3];

    unsigned int selectedSceneID = 0;

    bool renderGUI = true;
};

struct InputState {
    bool isFlying = false; // TODO is this input state?
    DirectX::XMFLOAT2 mousePos;
    DirectX::XMVECTOR mouseDelta = { 0, 0 };
    DirectX::XMFLOAT3 keyboradMovementInput  = { 0, 0, 0 };
    struct KeyboradInput {
        bool keyDownCTRL;
    } keyboardInput;
};

struct WindowState {
    DirectX::XMFLOAT2 windowCenter;
};

/* The dreaded global state. Used as a singleton: first get an instance, then
have fun. Will try my best to not just put everything here...*/
class EngineStateModel {
private:
    static EngineStateModel* instance;
    static std::mutex mutex;

protected:
    EngineStateModel() {};
    ~EngineStateModel() { delete instance; }

    RenderingState renderingState;
    InputState inputState;
    WindowState windowState;

public:
    EngineStateModel(EngineStateModel& other) = delete;
    void operator=(const EngineStateModel&) = delete;

    static EngineStateModel* GetInstance();

    RenderingState& GetRenderingState() { return renderingState; }
    InputState& GetInputState() { return inputState; }
    WindowState& GetWindowState() {return windowState; }
};
