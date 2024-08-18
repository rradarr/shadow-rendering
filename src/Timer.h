#pragma once

#include <mutex>

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
    double frameDelta = 0.0;
    double fps = 0.0;
    
    double rollingAvgFps = 0.0;
    double fpsTotal = 0.0;
    unsigned int fpsSamples = 0;
    unsigned int rollingAvgFpsWindow = 60;
    void UpdateRollingAvgFps();

public:
    Timer(Timer& other) = delete;
    void operator=(const Timer&) = delete;

    static Timer* GetInstance();

    // Call this once per frame
    void Update();

    void SetRollingAvgFpsWindow(unsigned int windowInFrames) { rollingAvgFpsWindow = windowInFrames; }

    double GetFps() { return fps; }
    double GetRollingAvgFps() { return rollingAvgFps; }
    double GetDeltaTime() { return frameDelta; }
};

