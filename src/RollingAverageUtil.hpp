#pragma once

#include <deque>

template <class T>
class RollingAverageUtil {
public:
    // RollingAverageUtil() = default;
    // ~RollingAverageUtil();

    void SetRollingWindowSize(unsigned int windowSize);
    void AddObservation(T observationValue);
    T GetAverage();

private:
    // Using a deque instead of a queue to allow pushing 0-value observations to front when resizing the window.
    std::deque<T> observations;
    T sumOfObservations{};
};
