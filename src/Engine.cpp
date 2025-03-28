#include "stdafx.hpp"
#include "Engine.hpp"

Engine::Engine(UINT windowWidth, UINT windowHeight, std::string windowName) :
    windowWidth(windowWidth), windowHeight(windowHeight), windowName(windowName)
{
    aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

Engine::~Engine()
{
}
