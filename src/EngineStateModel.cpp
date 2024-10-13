#include "stdafx.hpp"
#include "EngineStateModel.hpp"

const char* RenderingState::renderingModeNames[static_cast<int>(RenderingMode::NUM_RENDERING_MODES)]{
    "Default",
    "Wireframe",
    "Normals Debug",
    "Shadows - projection shadows",
    "Shadows - simple shadow mapping"
 };

EngineStateModel* EngineStateModel::instance{ nullptr };
std::mutex EngineStateModel::mutex;

EngineStateModel *EngineStateModel::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new EngineStateModel();
    }
    return instance;
}
