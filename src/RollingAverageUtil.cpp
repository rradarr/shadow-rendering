#include "RollingAverageUtil.hpp"

#include <numeric>

template <class T>
inline void RollingAverageUtil<T>::SetRollingWindowSize(unsigned int windowSize)
{
    unsigned int oldObservationsSize = static_cast<unsigned int>(observations.size());
    
    if(oldObservationsSize == windowSize) {
        return;
    }
    else if(oldObservationsSize > windowSize) {
        // If new window is smaller, remove last values and recalculate sum.
        for(int i = oldObservationsSize - windowSize; i > 0; i--) {
            observations.pop_front();
        }
        sumOfObservations = std::accumulate(observations.begin(), observations.end(), T{});
    }
    else {
        // If new window is larger, fill back of values with zeroes.
        for(int i = windowSize - oldObservationsSize; i > 0; i--) {
            observations.push_front(T{});
        }
    }
}

template <class T>
inline void RollingAverageUtil<T>::AddObservation(T observationValue)
{
    sumOfObservations -= observations.front();
    observations.pop_front();
    observations.push_back(observationValue);
    sumOfObservations += observations.back();
}

template <class T>
inline T RollingAverageUtil<T>::GetAverage()
{
    return sumOfObservations / observations.size();
}

template class RollingAverageUtil<double>;