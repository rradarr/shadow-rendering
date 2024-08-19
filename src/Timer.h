#pragma once

#include <mutex>

#include "RollingAverageUtil.h"

class Timer
{
private:
    static Timer* instance;
    static std::mutex mutex;

protected:
    Timer();
    ~Timer() { delete instance; };

    double timerFrequency = 0.0;
    long long lastFrameTime = 0;
    double deltaTime = 0.0;
    double fps = 0.0;

    RollingAverageUtil<double> fpsRollingAvg;
    RollingAverageUtil<double> deltaTimeRollingAvg;

public:
    Timer(Timer& other) = delete;
    void operator=(const Timer&) = delete;

    static Timer* GetInstance();

    // Call this once per frame
    void Update();

    double GetFps() { return fps; }
    double GetRollingAvgFps() { return fpsRollingAvg.GetAverage(); }
    double GetDeltaTime() { return deltaTime; }
    double GetRollingAvgDeltaTime() { return deltaTimeRollingAvg.GetAverage(); }
};

