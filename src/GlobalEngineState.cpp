#include "stdafx.hpp"
#include "GlobalEngineState.hpp"

GlobalEngineState* GlobalEngineState::instance{ nullptr };
std::mutex GlobalEngineState::mutex;

GlobalEngineState *GlobalEngineState::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new GlobalEngineState();
    }
    return instance;
}
