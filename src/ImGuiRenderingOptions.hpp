#pragma once

#include <string>
#include <map>

class ImGuiRenderingOptions {
public:
    ImGuiRenderingOptions();
    ~ImGuiRenderingOptions();

    void Display();
private:
    const char* renderingModes[2];
    int currentRenderingMode = 0;
};