#include "stdafx.h"
#include "Timer.h"


Timer* Timer::instance{ nullptr };
std::mutex Timer::mutex;

Timer::Timer()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);

    // seconds
    timerFrequency = double(li.QuadPart);

    // milliseconds
    //timerFrequency = double(li.QuadPart) / 1000.0;

    // microseconds
    //timerFrequency = double(li.QuadPart) / 1000000.0;

    QueryPerformanceCounter(&li);
    lastFrameTime = li.QuadPart;
}

void Timer::UpdateRollingAvgFps()
{
    fpsTotal += fps;
    fpsSamples++;

    if(fpsSamples >= rollingAvgFpsWindow) {
        rollingAvgFps = fpsTotal / fpsSamples;
        fpsTotal = 0.0;
        fpsSamples = 0;
    }
}

Timer *Timer::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new Timer();
    }
    return instance;
}

void Timer::Update()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);

    frameDelta = double(li.QuadPart - lastFrameTime) / timerFrequency;

    if (frameDelta > 0) {
        fps = 1.0 / frameDelta;
    }

    lastFrameTime = li.QuadPart;

    UpdateRollingAvgFps();
}
