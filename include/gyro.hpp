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
    
    YPR(YPR<T>&& other) noexcept(util::numberNoexcept<T>()) : yaw(util::move(other.yaw)), pitch(util::move(other.pitch)), roll(util::move(other.roll)) {}
    
    YPR(T&& p_yaw, T&& p_pitch, T&& p_roll) noexcept(util::numberNoexcept<T>()) : yaw(util::move(p_yaw)), pitch(util::move(p_pitch)), roll(util::move(p_roll)) {}
    
    YPR<T>& operator=(const YPR<T>& other) noexcept(util::numberNoexcept<T>()) {
        yaw = other.yaw;
        pitch = other.pitch;
        roll = other.roll;
        return *this;
    }
    
    YPR<T>& operator=(YPR<T>&& other) noexcept(util::numberNoexcept<T>()) {
        yaw = util::move(other.yaw);
        pitch = util::move(other.pitch);
        roll = util::move(other.roll);
        return *this;
    }
};

template <typename T> using Acceleration = util::Vector<T>;
template <typename T> using Speed = util::Vector<T>;

template <typename YPRType, typename AccSpeedType> class GyroData {
private:
    using AccT = Acceleration<AccSpeedType>;
    using SpeedT = Speed<AccSpeedType>;
public:
    YPR<YPRType> ypr;
    Acceleration<AccSpeedType> acceleration;
    Speed<AccSpeedType> speed;
    
    
    GyroData() = default;
    
    GyroData(const YPR<YPRType>& ypr, const AccT& acceleration, const SpeedT& speed)
        noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) : ypr(ypr), acceleration(acceleration), speed(speed) {}
    
    GyroData(const GyroData& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>())
        : ypr(other.ypr), acceleration(other.acceleration), speed(other.speed) {}
    
    GyroData(GyroData&& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>())
        : ypr(util::move(other.ypr)), acceleration(util::move(other.acceleration)), speed(util::move(other.speed)) {}
    
    GyroData(YPR<YPRType>&& ypr, AccT&& acceleration, SpeedT&& speed) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>())
        : ypr(util::move(ypr)), acceleration(util::move(acceleration)), speed(util::move(speed)) {}
    
    GyroData<AccT, SpeedT>& operator=(const GyroData& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) {
        ypr = other.ypr;
        acceleration = other.acceleration;
        speed = other.speed;
        
        return *this;
    }
    
    GyroData<AccT, SpeedT>& operator=(GyroData&& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) {
        ypr = util::move(other.ypr);
        acceleration =  util::move(other.acceleration);
        speed = util::move(other.speed);
        
        return *this;
    }
};

// getters

template <typename T> class AccelerationGetter {
public:
    virtual inline util::Result<Acceleration<T>> getAcceleration() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class SpeedGetter {
public:
    virtual inline util::Result<Speed<T>> getSpeed() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class YawGetter {
public:
    virtual inline util::Result<T> getYaw() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class PitchGetter {
public:
    virtual inline util::Result<T> getPitch() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class RollGetter {
public:
    virtual inline util::Result<T> getRoll() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class YPRGetter : public YawGetter<T>, public PitchGetter<T>, public RollGetter<T> {
public:
    virtual inline util::Result<YPR<T>> getYPR() const noexcept(util::numberNoexcept<T>()) {
        
        util::Result<T> yaw = this->getYaw();
        if(yaw) return yaw.err();
        
        util::Result<T> pitch = this->getPitch();
        if(pitch) return pitch.err();
        
        util::Result<T> roll = this->getRoll();
        if(roll) return roll.err();
        
        return {yaw(), pitch(), roll()};
    }
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
virtual util::Result<YPR<T>> calculateYPR() const noexcept(util::numberNoexcept<T>()) {
        
        util::Result<T> yaw = this->calculateYaw();
        if(yaw) return yaw.err();
        
        util::Result<T> pitch = this->calculatePitch();
        if(pitch) return pitch.err();
        
        util::Result<T> roll = this->calculateRoll();
        if(roll) return roll.err();
        
        return {yaw(), pitch(), roll()};
    }
};


// ultimate

template <typename YPRType, typename AccSpeedType> class GyroDataGetter :
    public YPRGetter<YPRType>, public AccelerationGetter<AccSpeedType>, public SpeedGetter<AccSpeedType> {

public:
    virtual inline util::Result<GyroData<YPRType, AccSpeedType>> getGyroData() const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->getYPR();
        if(ypr) return ypr.err();
        
        util::Result<util::Vector<AccSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        util::Result<util::Vector<AccSpeedType>> speed = this->getSpeed();
        if(speed) return speed.err();
        
        return {util::move(ypr()), util::move(acceleration()), util::move(speed())};
    }
};

template <typename YPRType, typename AccSpeedType> class GyroDataCalculator :
    public YPRCalculator<YPRType>, public AccelerationGetter<AccSpeedType>, public SpeedGetter<AccSpeedType> {

public:
    virtual inline util::Result<GyroData<YPRType, AccSpeedType>> getGyroData() const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->calculateYPR();
        if(ypr) return ypr.err();
        
        util::Result<util::Vector<AccSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        util::Result<util::Vector<AccSpeedType>> speed = this->getSpeed();
        if(speed) return speed.err();
        
        return {util::move(ypr()), util::move(acceleration()), util::move(speed())};
    }
};

} // namespace vislib::gyro