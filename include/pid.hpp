#pragma once

#include "util/errordef.hpp"
#include "util/types.hpp"

namespace vislib {

template <typename T, typename TimeType = size_t> class PIDRegulator {
protected:
    T Kp;
    T Ki;
    T Kd;
    T errold{};
    T integral{};
    T target{};
    TimeType prevTime{};
    
public:
    PIDRegulator(T Kp, T Ki, T Kd) noexcept : Kp(Kp), Ki(Ki), Kd(Kd) { }
    PIDRegulator(T Kp, T Ki, T Kd, T target) noexcept : Kp(Kp), Ki(Ki), Kd(Kd), target(target) {}

    
    T compute(T measured, T target, TimeType time) noexcept {
        T error = target - measured;
        
        if (prevTime == 0) {
            prevTime = time;
            errold = error;
            return Kp * error;
        }
        
        TimeType timeStep = time - prevTime;
        
        
        integral += error * timeStep;
        
        T derivative = (timeStep > 0) ? (error - errold) / static_cast<T>(timeStep) : 0;
        
        T output = Kp * error + Ki * integral + Kd * derivative;
        
        errold = error;
        prevTime = time;
        
        return output;
        
    }
    
    T compute(T measured, TimeType time) noexcept {
        return compute(measured, this->target, time);
    }
    
    void setTarget(T target) noexcept {
        this->target = target;
    }
    
    T getTarget() const noexcept {
        return target;
    }
    
};

} // namespace vislib
