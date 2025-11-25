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
template <typename T> using AngularSpeed = util::Vector<T>;

class BaseGyroController {
public:
    virtual util::Error update()  = 0;
    virtual util::Error calibrate() = 0;
};

template <typename T, typename WT = T, typename TimeType = T> struct YPRElementCalculatorConfig {
    WT integralWeight{};
    T offset{};
    util::Integrator<T, TimeType> integrator{};
    
};
template <typename YPRType, typename AccAngularSpeedType> class GyroData {
private:
    using AccT = Acceleration<AccAngularSpeedType>;
    using SpeedT = AngularSpeed<AccAngularSpeedType>;
public:
    YPR<YPRType> ypr;
    Acceleration<AccAngularSpeedType> acceleration;
    AngularSpeed<AccAngularSpeedType> speed;
    
    
    GyroData() = default;
    
    GyroData(const YPR<YPRType>& ypr, const AccT& acceleration, const SpeedT& speed)
        noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) : ypr(ypr), acceleration(acceleration), speed(speed) {}
    
    GyroData(const GyroData& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>())
        : ypr(other.ypr), acceleration(other.acceleration), speed(other.speed) {}
    
    GyroData(GyroData&& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>())
        : ypr(util::move(other.ypr)), acceleration(util::move(other.acceleration)), speed(util::move(other.speed)) {}
    
    GyroData(YPR<YPRType>&& ypr, AccT&& acceleration, SpeedT&& speed) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>())
        : ypr(util::move(ypr)), acceleration(util::move(acceleration)), speed(util::move(speed)) {}
    
    GyroData<YPRType, AccAngularSpeedType>& operator=(const GyroData& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        ypr = other.ypr;
        acceleration = other.acceleration;
        speed = other.speed;
        
        return *this;
    }
    
    GyroData<AccT, SpeedT>& operator=(GyroData&& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        ypr = util::move(other.ypr);
        acceleration =  util::move(other.acceleration);
        speed = util::move(other.speed);
        
        return *this;
    }
};

// getters

template <typename T> class AccelerationGetter {
public:
    virtual util::Result<Acceleration<T>> getAcceleration() const noexcept(util::numberNoexcept<T>()) = 0;
};

template <typename T> class AngularSpeedGetter {
public:
    virtual util::Result<AngularSpeed<T>> getSpeed() const noexcept(util::numberNoexcept<T>()) = 0;
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
    virtual util::Result<YPR<T>> getYPR() const noexcept(util::numberNoexcept<T>()) {
        
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

template <typename T, typename TimeType = T> class YawCalculator : public YawGetter<T> {
protected:
    YPRElementCalculatorConfig<T, T, TimeType> yawConfig{};
    
    util::Error internalYawInit(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    util::Result<T> internalNonIntegralPartYawCalculation() const {
        return T();
    }
    
public:
    
    util::Error initYawCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        yawConfig = config;
        return internalYawInit(yawConfig);
    }

    virtual util::Result<T> calculateYaw(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<T> temp = yawConfig.integrator.update(currentTime, this->getYaw());
        
        if(temp) return temp.err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartYawCalculation();
        
        if(nonIntegral) return nonIntegral.err();
        
        yawConfig.integrator.setIntegral(temp * yawConfig.integralWeight + nonIntegral * (T(1) - yawConfig.integralWeight));
        
        return yawConfig.integrator.getIntegral() + yawConfig.offset;
        
    }
};

template <typename T, typename TimeType = T> class PitchCalculator : public PitchGetter<T> {
protected:
    YPRElementCalculatorConfig<T, T, TimeType> pitchConfig{};
    
    util::Error internalPitchInit(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    util::Result<T> internalNonIntegralPartPitchCalculation() const {
        return T();
    }
    
public:
    
    util::Error initPitchCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        pitchConfig = config;
        return internalPitchInit(pitchConfig);
    }

    virtual util::Result<T> calculatePitch(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<T> temp = pitchConfig.integrator.update(currentTime, this->getPitch());
        
        if(temp) return temp.err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartPitchCalculation();
        
        if(nonIntegral) return nonIntegral.err();
        
        pitchConfig.integrator.setIntegral(temp * pitchConfig.integralWeight + nonIntegral * (T(1) - pitchConfig.integralWeight));
        
        return pitchConfig.integrator.getIntegral() + pitchConfig.offset;
        
    }
};

template <typename T, typename TimeType = T> class RollCalculator : public RollGetter<T> {
protected:
    YPRElementCalculatorConfig<T, T, TimeType> rollConfig{};
    
    util::Error internalRollInit(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    util::Result<T> internalNonIntegralPartRollCalculation() const {
        return T();
    }
    
public:
    
    util::Error initRollCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        rollConfig = config;
        return internalRollInit(rollConfig);
    }

    virtual util::Result<T> calculateRoll(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<T> temp = rollConfig.integrator.update(currentTime, this->getRoll());
        
        if(temp) return temp.err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartRollCalculation();
        
        if(nonIntegral) return nonIntegral.err();
        
        rollConfig.integrator.setIntegral(temp * rollConfig.integralWeight + nonIntegral * (T(1) - rollConfig.integralWeight));
        
        return rollConfig.integrator.getIntegral() + rollConfig.offset;
        
    }
};

template <typename T, typename TimeType = T> class YPRCalculator : public YawCalculator<T>, public PitchCalculator<T>, public RollCalculator<T> {
public:

virtual util::Error initCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& yawConfig,
                                   const YPRElementCalculatorConfig<T, T, TimeType>& pitchConfig,
                                   const YPRElementCalculatorConfig<T, T, TimeType>& rollConfig)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Error err = this->initYawCalculator(yawConfig);
        if(err) return err;
        
        err = this->initPitchCalculator(pitchConfig);
        if(err) return err;
        
        err = this->initRollCalculator(rollConfig);
        if(err) return err;
        
        return util::ErrorCode::success;
    }

virtual util::Result<YPR<T>> calculateYPR(const TimeType& currentTime) const noexcept(util::numberNoexcept<T>()) {
        
        util::Result<T> yaw = this->calculateYaw(currentTime);
        if(yaw) return yaw.err();
        
        util::Result<T> pitch = this->calculatePitch(currentTime);
        if(pitch) return pitch.err();
        
        util::Result<T> roll = this->calculateRoll(currentTime);
        if(roll) return roll.err();
        
        return {yaw(), pitch(), roll()};
    }
};


// controllers

template <typename YPRType, typename AccAngularSpeedType> class GyroDataGetter :
    public YPRGetter<YPRType>, public AccelerationGetter<AccAngularSpeedType>, public AngularSpeedGetter<AccAngularSpeedType> {

public:
    virtual util::Result<GyroData<YPRType, AccAngularSpeedType>> getGyroData() const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->getYPR();
        if(ypr) return ypr.err();
        
        util::Result<util::Vector<AccAngularSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        util::Result<util::Vector<AccAngularSpeedType>> speed = this->getSpeed();
        if(speed) return speed.err();
        
        return {util::move(ypr()), util::move(acceleration()), util::move(speed())};
    }
};

template <typename YPRType, typename AccAngularSpeedType, typename TimeType = YPRType> class GyroDataCalculator :
    public YPRCalculator<YPRType, TimeType>, public AccelerationGetter<AccAngularSpeedType>, public AngularSpeedGetter<AccAngularSpeedType> {

public:
    virtual util::Result<GyroData<YPRType, AccAngularSpeedType>> calculateGyroData() const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->calculateYPR();
        if(ypr) return ypr.err();
        
        util::Result<Acceleration<AccAngularSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        util::Result<AngularSpeed<AccAngularSpeedType>> speed = this->getSpeed();
        if(speed) return speed.err();
        
        return {util::move(ypr()), util::move(acceleration()), util::move(speed())};
    }
};

template <typename YPRType, typename AccAngularSpeedType, typename TimeType = YPRType> class UltimateGyroController :
    public BaseGyroController,
    public GyroDataGetter<YPRType, AccAngularSpeedType>,
    public GyroDataCalculator<YPRType, AccAngularSpeedType, TimeType> {
};

} // namespace vislib::gyro