#ifndef HINAPE_FLUID_ENGINE_TIMER_H_
#define HINAPE_FLUID_ENGINE_TIMER_H_

#include <chrono>

namespace HinaPE::FluidEngine
{

//! Simple timer class.
class Timer
{
public:
    //! Constructs the timer and start ticking.
    Timer();

    //! Returns the time duration since the creation or reset in seconds.
    double durationInSeconds() const;

    //! Resets the timer.
    void reset();

private:
    std::chrono::steady_clock _clock;
    std::chrono::steady_clock::time_point _startingPoint;
};

}  // namespace HinaPE::FluidEngine

#endif  // HINAPE_FLUID_ENGINE_TIMER_H_