#pragma once

#include <mutex>

/* The dreaded global state. Used as a singleton: first get an instance, then
have fun. Will try my best to not just put everything here...*/
class GlobalEngineState {
private:
    static GlobalEngineState* instance;
    static std::mutex mutex;

protected:
    GlobalEngineState() {};
    ~GlobalEngineState() { delete instance; }

    struct RenderingState {
        int chosenRenderingMode = 0;
    } renderingState;

public:
    GlobalEngineState(GlobalEngineState& other) = delete;
    void operator=(const GlobalEngineState&) = delete;

    static GlobalEngineState* GetInstance();

    RenderingState& GetRenderingState() { return renderingState; }
};