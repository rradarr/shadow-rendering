#include "stdafx.hpp"
#include "MainInputController.hpp"

MainInputController* MainInputController::instance{ nullptr };
std::mutex MainInputController::mutex;

void MainInputController::HandleKeyDown(UINT8 keyCode)
{
    if(modelInputState.isFlying){
        switch (keyCode) {
        case 0x57: // W
            modelInputState.keyboradMovementInput.z = 1;
            break;
        case 0x53: // S
            modelInputState.keyboradMovementInput.z = -1;
            break;
        case 0x44: // D
            modelInputState.keyboradMovementInput.x = 1;
            break;
        case 0x41: // A
            modelInputState.keyboradMovementInput.x = -1;
            break;
        case 0x20: // SPACE
            modelInputState.keyboradMovementInput.y = 1;
            break;
        case 0x11: // CTRL
            modelInputState.keyboradMovementInput.y = -1;
            break;
        };
    }
    else {
        switch (keyCode) {
        case 0x11: // CTRL
            modelInputState.keyboardInput.keyDownCTRL = true;
            break;
        };
    }
}

void MainInputController::HandleKeyUp(UINT8 keyCode)
{
    switch (keyCode) {
        case 0x48: // H
        stateModel->GetRenderingState().renderGUI = 
            !stateModel->GetRenderingState().renderGUI;
        break;
    };

    if(modelInputState.isFlying) {
        switch (keyCode) {
        case 0x57: // W
            modelInputState.keyboradMovementInput.z = 
                modelInputState.keyboradMovementInput.z == 1? 0 : modelInputState.keyboradMovementInput.z;
            break;
        case 0x53: // S
            modelInputState.keyboradMovementInput.z = 
                modelInputState.keyboradMovementInput.z == -1? 0 : modelInputState.keyboradMovementInput.z;
            break;
        case 0x44: // D
            modelInputState.keyboradMovementInput.x = 
                modelInputState.keyboradMovementInput.x == 1 ? 0 : modelInputState.keyboradMovementInput.x;
            break;
        case 0x41: // A
            modelInputState.keyboradMovementInput.x = 
                modelInputState.keyboradMovementInput.x == -1 ? 0 : modelInputState.keyboradMovementInput.x;
            break;
        case 0x20: // SPACE
            modelInputState.keyboradMovementInput.y = 
                modelInputState.keyboradMovementInput.y == 1 ? 0 : modelInputState.keyboradMovementInput.y;
            break;
        case 0x11: // CTRL
            modelInputState.keyboradMovementInput.y = 
                modelInputState.keyboradMovementInput.y == -1 ? 0 : modelInputState.keyboradMovementInput.y;
            break;
        case 0x1B: // ESC
            modelInputState.keyboradMovementInput.x = 0;
            modelInputState.keyboradMovementInput.y = 0;
            modelInputState.keyboradMovementInput.z = 0;
            modelInputState.isFlying = false;
            ShowCursor(TRUE); // TODO: this is very iffy, probably should emit an event that is handled somewhere...
            break;
        };
    }
    else {
        switch(keyCode){
        case 0x58: // X
            if(stateModel->GetRenderingState().chosenRenderingMode != RenderingState::RenderingMode::WIREFRAME) {
                lastRenderingMode = stateModel->GetRenderingState().chosenRenderingMode;
                stateModel->GetRenderingState().chosenRenderingMode = RenderingState::RenderingMode::WIREFRAME;
            }
            else {
                stateModel->GetRenderingState().chosenRenderingMode = lastRenderingMode;
            }
            break;
        case 0x46: // F
            if(modelInputState.keyboardInput.keyDownCTRL){
                modelInputState.isFlying = true;
                ShowCursor(FALSE); // TODO: as above: this is very iffy, probably should emit an event that is handled somewhere...
            }
            break;
        case 0x11: // CTRL
            modelInputState.keyboardInput.keyDownCTRL = false;
            break;
        }
    }
}

void MainInputController::HandleMouseMove(int mouseX, int mouseY)
{
    if (modelInputState.isFlying)
    {
        modelInputState.mousePos = { static_cast<float>(mouseX), static_cast<float>(mouseY) };
    }
    else {
        modelInputState.mousePos = stateModel->GetWindowState().windowCenter;
    }
}

void MainInputController::EarlyUpdate(HWND windowHandle)
{
    if (modelInputState.isFlying)
    {
        DirectX::XMFLOAT2 windowCenter = stateModel->GetWindowState().windowCenter;

        modelInputState.mouseDelta = DirectX::XMVectorSubtract(
            DirectX::XMLoadFloat2(&modelInputState.mousePos), DirectX::XMLoadFloat2(&windowCenter));

        POINT center = { LONG(windowCenter.x), LONG(windowCenter.y) };
        ClientToScreen(windowHandle, &center);
        SetCursorPos(center.x, center.y);
    }
    else {
        // Keep mouse delta of 0 when not flying.
        modelInputState.mouseDelta = {0.f, 0.f};
    }
}

MainInputController::MainInputController(EngineStateModel *model) : modelInputState(model->GetInputState())
{
    stateModel = model;
}

MainInputController *MainInputController::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new MainInputController(EngineStateModel::GetInstance());
    }
    return instance;
}
