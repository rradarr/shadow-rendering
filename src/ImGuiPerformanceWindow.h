#pragma once

class ImGuiPerformanceWindow {
public:
    void ToggleVisibility() { isVisible = !isVisible; }
    void SetVisibility(bool visible) { isVisible = visible; }
    bool GetVisibility() { return isVisible; }

    /* Will display the window if it is visible. */
    void Display();

private:
    bool isVisible = true;

    bool rollingAvg = true;
};