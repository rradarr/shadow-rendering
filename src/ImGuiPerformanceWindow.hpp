#pragma once

class ImGuiPerformanceWindow {
public:
    // TODO reconsider new approach (RenderEnablingCheckbox) and possibly remove these:
    // void ToggleVisibility() { isVisible = !isVisible; }
    // void SetVisibility(bool visible) { isVisible = visible; }
    // bool GetVisibility() { return isVisible; }

    /* Will display a checkbox that controls the visibility of the perf window.
    Meant to be used within another window.*/
    void DisplayEnablingCheckbox();
    /* Will display the window if it is visible. */
    void Display();

private:
    bool isVisible = true;

    bool rollingAvg = true;
};