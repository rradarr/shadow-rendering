#pragma once

class ImGuiPerformanceWindow {
public:
    /* Will display a checkbox that controls the visibility of the perf window.
    Meant to be used within another window.*/
    void DisplayEnablingCheckbox();
    /* Will display the window if it is visible. */
    void Display();

private:
    bool isVisible = true;

    bool rollingAvg = true;
};