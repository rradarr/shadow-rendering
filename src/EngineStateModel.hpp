#pragma once

#include <mutex>

struct RenderingState {
    enum class RenderingMode {
        DEFAULT = 0,
        WIREFRAME = 1,
        NORMALS_DEBUG = 2,
        SHADOW_PROJECTION = 3,
        SHADOW_MAP = 4,
        NUM_RENDERING_MODES
    };
    RenderingMode chosenRenderingMode = RenderingMode::DEFAULT;
    static const char* renderingModeNames[static_cast<int>(RenderingMode::NUM_RENDERING_MODES)];

    unsigned int selectedSceneID = 0;

    bool renderGUI = true;

    // Shadow map related settings.
    enum class PCFMode {
        SIMPLE = 0,
        MANUAL_FILTER = 1,
        RANDOM_OFFESTS_FILTER = 2,
        PCSS_RANDOM_OFFSETS = 3,
        NUM_PCF_MODES
    };
    // PCFMode chosenPFCMode = PCFMode::SIMPLE;
    PCFMode chosenPFCMode = PCFMode::PCSS_RANDOM_OFFSETS;
    static const char* pcfModeNames[static_cast<int>(PCFMode::NUM_PCF_MODES)];
    unsigned int manualPCFKernelSize = 3;
    float pcfSampleOffset = 1.f;
    float worldSpaceLightSize = 0.1f;
    // float inShaderDepthBias = 0.f;
    float inShaderDepthBias = -0.003f;
    float ambientStrength = 0.02f;
    int useBilinearFiltering = 0;
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
