#pragma once

#include <string>
#include <map>

#include "EngineStateModel.hpp"

class ImGuiRenderingOptions {
public:
    ImGuiRenderingOptions();
    ~ImGuiRenderingOptions();

    void Display();

private:
    void DisplayShadowMapOptions();
    void DisplayVarianceShadowMapOptions();
};