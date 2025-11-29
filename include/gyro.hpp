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
    
    YPR<T>& operator=(const YPR<T>& other) noexcept(util::numberNoexcept<T>()) = default;
    
    YPR<T>& operator=(YPR<T>&& other) noexcept(util::numberNoexcept<T>()) = default;
};

template <typename T> using Acceleration = util::Vector<T>;
template <typename T> using AngularSpeed = util::Vector<T>;

class BaseGyroController {
public:
    virtual util::Error update()  = 0;
    virtual util::Error calibrate() = 0;
    virtual ~BaseGyroController() = default;
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
    
    GyroData<YPRType, AccAngularSpeedType>& operator=(const GyroData& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) = default;
    
    GyroData<YPRType, AccAngularSpeedType>& operator=(GyroData&& other) noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
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
    virtual ~AccelerationGetter() = 0;
};

template <typename T> class AngularSpeedGetter {
public:
    virtual util::Result<AngularSpeed<T>> getAngularSpeed() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~AngularSpeedGetter() = 0;
};

template <typename T> class YawGetter {
public:
    virtual util::Result<T> getYaw() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~YawGetter() = 0;
};

template <typename T> class PitchGetter {
public:
    virtual util::Result<T> getPitch() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~PitchGetter() = 0;
};

template <typename T> class RollGetter {
public:
    virtual util::Result<T> getRoll() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~RollGetter() = 0;
};

template <typename T> class YPRGetter : virtual public YawGetter<T>, virtual public PitchGetter<T>, virtual public RollGetter<T> {
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
    
    virtual ~YPRGetter() = 0;
};

// calculators

template <typename T, typename TimeType = T> class YawCalculator : virtual public YawGetter<T>, virtual public AngularSpeedGetter<T> {
protected:
    YPRElementCalculatorConfig<T, T, TimeType> yawConfig{};
    
    virtual util::Error internalYawInit(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    virtual util::Result<T> internalNonIntegralPartYawCalculation() const {
        return T();
    }
    
public:
    
    virtual util::Error initYawCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        yawConfig = config;
        yawConfig.integrator.setIntegral(yawConfig.integrator.getIntegral() + yawConfig.offset);
        
        return internalYawInit(yawConfig);
    }

    virtual util::Result<T> calculateYaw(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<AngularSpeed<T>> angularSpeed = this->getAngularSpeed();
        if(angularSpeed) return angularSpeed.err();
        
        util::Result<T> temp = yawConfig.integrator.update(currentTime, angularSpeed().at(0));
        if(temp) return temp.err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartYawCalculation();
        if(nonIntegral) return nonIntegral.err();
        
        yawConfig.integrator.setIntegral(temp * yawConfig.integralWeight + nonIntegral * (T(1) - yawConfig.integralWeight));
        
        return yawConfig.integrator.getIntegral();
        
    }
    
    virtual ~YawCalculator() = 0;
};

template <typename T, typename TimeType = T> class PitchCalculator : virtual public PitchGetter<T>, virtual public AngularSpeedGetter<T> {
protected:
    YPRElementCalculatorConfig<T, T, TimeType> pitchConfig{};
    
    virtual util::Error internalPitchInit(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    virtual util::Result<T> internalNonIntegralPartPitchCalculation() const {
        return T();
    }
    
public:
    
    virtual util::Error initPitchCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        pitchConfig = config;
        pitchConfig.integrator.setIntegral(pitchConfig.integrator.getIntegral() + pitchConfig.offset);
        
        return internalPitchInit(pitchConfig);
    }

    virtual util::Result<T> calculatePitch(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<AngularSpeed<T>> angularSpeed = this->getAngularSpeed();
        if(angularSpeed) return angularSpeed.err();
        
        util::Result<T> temp = pitchConfig.integrator.update(currentTime, angularSpeed().at(1));
        if(temp) return temp.err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartPitchCalculation();
        if(nonIntegral) return nonIntegral.err();
        
        pitchConfig.integrator.setIntegral(temp * pitchConfig.integralWeight + nonIntegral * (T(1) - pitchConfig.integralWeight));
        
        return pitchConfig.integrator.getIntegral();
        
    }
    
    virtual ~PitchCalculator() = 0;
};

template <typename T, typename TimeType = T> class RollCalculator : virtual public RollGetter<T>, virtual public AngularSpeedGetter<T> {
protected:
    YPRElementCalculatorConfig<T, T, TimeType> rollConfig{};
    
    virtual util::Error internalRollInit(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    virtual util::Result<T> internalNonIntegralPartRollCalculation() const {
        return T();
    }
    
public:
    
    virtual util::Error initRollCalculator(const YPRElementCalculatorConfig<T, T, TimeType>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        rollConfig = config;
        rollConfig.integrator.setIntegral(rollConfig.integrator.getIntegral() + rollConfig.offset);
        
        return internalRollInit(rollConfig);
    }

    virtual util::Result<T> calculateRoll(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<AngularSpeed<T>> angularSpeed = this->getAngularSpeed();
        if(angularSpeed) return angularSpeed.err();
        
        util::Result<T> temp = rollConfig.integrator.update(currentTime, angularSpeed().at(2));
        if(temp) return temp.err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartRollCalculation();
        if(nonIntegral) return nonIntegral.err();
        
        rollConfig.integrator.setIntegral(temp * rollConfig.integralWeight + nonIntegral * (T(1) - rollConfig.integralWeight));
        
        return rollConfig.integrator.getIntegral();
        
    }
    
    virtual ~RollCalculator() = 0;
};

template <typename T, typename TimeType = T> class YPRCalculator : virtual public YawCalculator<T>, virtual public PitchCalculator<T>, virtual public RollCalculator<T> {
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
    
    virtual ~YPRCalculator() = 0;
};


//extended calculators

template <typename T, typename TimeType = T> class PitchCalculatorWithAcceleration : virtual public PitchCalculator<T, TimeType>, virtual public AccelerationGetter<T> {
protected:
    virtual util::Result<T> internalNonIntegralPartPitchCalculation() const override {
        util::Result<Acceleration<T>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        T accX = acceleration().at(0);
        T accY = acceleration().at(1);
        T accZ = acceleration().at(2);
        
        T temp = sqrt(accY * accY + accZ * accZ);
        if(temp == T(0)) return T(0);
        
        
        return util::rad2Deg(atan2(accY, temp));
    }
    
    virtual ~PitchCalculatorWithAcceleration() = 0;
};

template <typename T, typename TimeType = T> class RollCalculatorWithAcceleration : virtual public RollCalculator<T, TimeType>, virtual public AccelerationGetter<T> {
protected:
    virtual util::Result<T> internalNonIntegralPartRollCalculation() const override {
        util::Result<Acceleration<T>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        T accY = acceleration().at(1);
        T accZ = acceleration().at(2);
        
        if(accZ == T(0)) return T(90);
        
        
        return util::rad2Deg(atan2(accY, accZ));
    }
    
    virtual ~RollCalculatorWithAcceleration() = 0;
};

template <typename T, typename TimeType = T> class YPRCalculatorWithAcceleration
    : public YPRCalculator<T, TimeType>, public PitchCalculatorWithAcceleration<T, TimeType>, public RollCalculatorWithAcceleration<T, TimeType> {
    virtual ~YPRCalculatorWithAcceleration() = 0;
};

// controllers

template <typename YPRType, typename AccAngularSpeedType> class GyroDataGetter
    : public YPRGetter<YPRType>, virtual public AccelerationGetter<AccAngularSpeedType>, virtual public AngularSpeedGetter<AccAngularSpeedType> {

public:
    virtual util::Result<GyroData<YPRType, AccAngularSpeedType>> getGyroData() const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->getYPR();
        if(ypr) return ypr.err();
        
        util::Result<util::Vector<AccAngularSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        util::Result<util::Vector<AccAngularSpeedType>> speed = this->getAngularSpeed();
        if(speed) return speed.err();
        
        return {util::move(ypr()), util::move(acceleration()), util::move(speed())};
    }
    
    virtual ~GyroDataGetter() = 0;
};

template <typename YPRType, typename AccAngularSpeedType, typename TimeType = YPRType> class GyroDataCalculator
    : public YPRCalculator<YPRType, TimeType>, virtual public AccelerationGetter<AccAngularSpeedType>, virtual public AngularSpeedGetter<AccAngularSpeedType> {

public:
    virtual util::Result<GyroData<YPRType, AccAngularSpeedType>> calculateGyroData(const TimeType& current) const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->calculateYPR(current);
        if(ypr) return ypr.err();
        
        util::Result<Acceleration<AccAngularSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.err();
        
        util::Result<AngularSpeed<AccAngularSpeedType>> speed = this->getAngularSpeed();
        if(speed) return speed.err();
        
        return {util::move(ypr()), util::move(acceleration()), util::move(speed())};
    }
    
    virtual ~GyroDataCalculator() = 0;
};

template <typename YPRType, typename AccAngularSpeedType, typename TimeType = YPRType> class GyroDataCalculatorWithAcceleration
    : public YPRCalculatorWithAcceleration<YPRType, TimeType>, public GyroDataCalculator<YPRType, AccAngularSpeedType, TimeType> {
    virtual ~GyroDataCalculatorWithAcceleration() = 0;
};

template <typename YPRType, typename AccAngularSpeedType, typename TimeType = YPRType> class UltimateGyroController :
    public BaseGyroController,
    public GyroDataGetter<YPRType, AccAngularSpeedType>,
    public GyroDataCalculatorWithAcceleration<YPRType, AccAngularSpeedType, TimeType> {
    
    virtual ~UltimateGyroController() = 0;
};

// Definitions for pure-virtual destructors

template <typename T>
AccelerationGetter<T>::~AccelerationGetter() = default;

template <typename T>
AngularSpeedGetter<T>::~AngularSpeedGetter() = default;

template <typename T>
YawGetter<T>::~YawGetter() = default;

template <typename T>
PitchGetter<T>::~PitchGetter() = default;

template <typename T>
RollGetter<T>::~RollGetter() = default;

template <typename T>
YPRGetter<T>::~YPRGetter() = default;

template <typename T, typename TimeType>
YawCalculator<T, TimeType>::~YawCalculator() = default;

template <typename T, typename TimeType>
PitchCalculator<T, TimeType>::~PitchCalculator() = default;

template <typename T, typename TimeType>
RollCalculator<T, TimeType>::~RollCalculator() = default;

template <typename T, typename TimeType>
YPRCalculator<T, TimeType>::~YPRCalculator() = default;

template <typename T, typename TimeType>
PitchCalculatorWithAcceleration<T, TimeType>::~PitchCalculatorWithAcceleration() = default;

template <typename T, typename TimeType>
RollCalculatorWithAcceleration<T, TimeType>::~RollCalculatorWithAcceleration() = default;

template <typename T, typename TimeType>
YPRCalculatorWithAcceleration<T, TimeType>::~YPRCalculatorWithAcceleration() = default;

template <typename YPRType, typename AccAngularSpeedType>
GyroDataGetter<YPRType, AccAngularSpeedType>::~GyroDataGetter() = default;

template <typename YPRType, typename AccAngularSpeedType, typename TimeType>
GyroDataCalculator<YPRType, AccAngularSpeedType, TimeType>::~GyroDataCalculator() = default;

template <typename YPRType, typename AccAngularSpeedType, typename TimeType>
GyroDataCalculatorWithAcceleration<YPRType, AccAngularSpeedType, TimeType>::~GyroDataCalculatorWithAcceleration() = default;

template <typename YPRType, typename AccAngularSpeedType, typename TimeType>
UltimateGyroController<YPRType, AccAngularSpeedType, TimeType>::~UltimateGyroController() = default;

} // namespace vislib::gyro
