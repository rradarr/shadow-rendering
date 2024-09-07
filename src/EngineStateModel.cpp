#include "stdafx.hpp"
#include "EngineStateModel.hpp"

const char* RenderingState::renderingModeNames[3]{
    "Default",
    "Wireframe",
    "Normals Debug"
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
