#pragma once

#include "util/util.hpp"

namespace vislib::gyro {

template <typename T> struct YPR {
    T yaw;
    T pitch;
    T roll;
    
    YPR() = default;
    YPR(const T& p_yaw, const T& p_pitch, const T& p_roll) noexcept(util::numberNoexcept<T>()) : yaw(p_yaw), pitch(p_pitch), roll(p_roll) {}
    YPR(const YPR<T>& other) noexcept(util::numberNoexcept<T>()) : yaw(other.yaw), pitch(other.pitch), roll(other.roll) {}
};

template <typename T> using Acceleration = util::Vector<T>;
template <typename T> using Speed = util::Vector<T>;

template <typename YPRType, typename AccSpeedType> class GyroData {
public:
    YPR<YPRType> ypr;
    Acceleration<AccSpeedType> acceleration;
    Speed<AccSpeedType> speed;
    
    GyroData() = default;
    
    GyroData(const YPR<YPRType>& ypr, const Acceleration& acceleration, const Speed& speed)
    noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) : ypr(ypr), acceleration(acceleration), speed(speed) {}
    
    GyroData(const GyroData& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>())
    : ypr(other.ypr), acceleration(other.acceleration), speed(other.speed) {}
    
    GyroData(const GyroData&& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) noexcept
    : ypr(util::move(other.ypr)), acceleration(util::move(other.acceleration)), speed(util::move(other.speed)) {}
    
    GyroData(YPR<YPRType>&& ypr, Acceleration<AccSpeedType>&& acceleration, Speed<AccSpeedType>&& speed) noexcept (util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>())
    : ypr(util::move(ypr)), acceleration(util::move(acceleration)), speed(util::move(speed)) {}
};

// getters

template <typename T> class AccelerationGetter {
public:
    virtual util::Result<util::Vector<T>> getAcceleration() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class SpeedGetter {
public:
    virtual util::Result<util::Vector<T>> getSpeed() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class YawGetter {
public:
    virtual util::Result<T> getYaw() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class PitchGetter {
public:
    virtual util::Result<T> getPitch() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class RollGetter {
public:
    virtual util::Result<T> getRoll() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class YPRGetter : public YawGetter<T>, public PitchGetter<T>, public RollGetter<T> {
public:
    virtual util::Result<YPR<T>> getYPR() const noexcept(util::numberNoexcept<T>()) = 0;
};



// calculators
template <typename T> class YawCalculator {
public:
    virtual util::Result<T> calculateYaw() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class PitchCalculator {
public:
    virtual util::Result<T> calculatePitch() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class RollCalculator {
public:
    virtual util::Result<T> calculateRoll() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class YPRCalculator : public YawCalculator<T>, public PitchCalculator<T>, public RollCalculator<T> {
public:
    virtual util::Result<YPR<T>> calculateYPR() const noexcept(util::numberNoexcept<T>()) = 0;
};


} // namespace vislib::gyro