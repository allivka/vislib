#pragma once

#include "util/util.hpp"

namespace vislib::gyro {

template <typename T> struct YPR {
    T yaw;
    T pitch;
    T roll;
    
    YPR() = default;
    
    YPR(const T& p_yaw, const T& p_pitch, const T& p_roll) noexcept(util::numberNoexcept<T>()) : yaw(p_yaw), pitch(p_pitch), roll(p_roll) {}
    
    YPR(const YPR<T>& ) noexcept(util::numberNoexcept<T>()) = default;
    
    YPR(YPR<T>&&) noexcept(util::numberNoexcept<T>()) = default;
    
    YPR<T>& operator=(const YPR<T>& other) noexcept(util::numberNoexcept<T>()) = default;
    
    YPR<T>& operator=(YPR<T>&& other) noexcept(util::numberNoexcept<T>()) = default;
};

template <typename T> using Acceleration = util::Vector<T>;
template <typename T> using AngularSpeed = util::Vector<T>;

template <typename UpdateParameterType> class BaseGyroController {
public:
    virtual util::Error update(UpdateParameterType)  = 0;
    virtual util::Error calibrate() = 0;
    virtual ~BaseGyroController() = default;
};

template <typename T, typename TimeType = T, typename WT = T> struct YPRElementCalculatorConfig {
    WT integralWeight = WT(1);
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
    virtual ~AccelerationGetter() = default;
};

template <typename T> class AngularSpeedGetter {
public:
    virtual util::Result<AngularSpeed<T>> getAngularSpeed() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~AngularSpeedGetter() = default;
};

template <typename T> class YawGetter {
public:
    virtual util::Result<T> getYaw() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~YawGetter() = default;
};

template <typename T> class PitchGetter {
public:
    virtual util::Result<T> getPitch() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~PitchGetter() = default;
};

template <typename T> class RollGetter {
public:
    virtual util::Result<T> getRoll() const noexcept(util::numberNoexcept<T>()) = 0;
    virtual ~RollGetter() = default;
};

template <typename T> class YPRGetter : virtual public YawGetter<T>, virtual public PitchGetter<T>, virtual public RollGetter<T> {
public:
    virtual util::Result<YPR<T>> getYPR() const noexcept(util::numberNoexcept<T>()) {
        
        util::Result<T> yaw = this->getYaw();
        if(yaw) return yaw.Err();
        
        util::Result<T> pitch = this->getPitch();
        if(pitch) return pitch.Err();
        
        util::Result<T> roll = this->getRoll();
        if(roll) return roll.Err();
        
        return YPR<T>{yaw(), pitch(), roll()};
    }
    
    virtual ~YPRGetter() = default;
};

// calculators

template <typename T, typename TimeType = T, typename WT = T> class YawCalculator : virtual public YawGetter<T>, virtual public AngularSpeedGetter<T> {
protected:
    YPRElementCalculatorConfig<T, TimeType, WT> yawConfig{};
    
    virtual util::Error internalYawInit(const YPRElementCalculatorConfig<T, TimeType, WT>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    virtual util::Result<T> internalNonIntegralPartYawCalculation() const {
        return T();
    }
    
public:
    
    virtual util::Error initYawCalculator(const YPRElementCalculatorConfig<T, TimeType, WT>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        yawConfig = config;
        yawConfig.integrator.setIntegral(yawConfig.integrator.getIntegral() + yawConfig.offset);
        
        return internalYawInit(yawConfig);
    }

    virtual util::Result<T> calculateYaw(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<AngularSpeed<T>> angularSpeed = this->getAngularSpeed();
        if(angularSpeed) return angularSpeed.Err();
        
        util::Result<T> temp = yawConfig.integrator.update(currentTime, angularSpeed().at(0));
        if(temp) return temp.Err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartYawCalculation();
        if(nonIntegral) return nonIntegral.Err();
        
        yawConfig.integrator.setIntegral(temp() * yawConfig.integralWeight + nonIntegral() * (T(1) - yawConfig.integralWeight));
        
        return yawConfig.integrator.getIntegral();
        
    }
    
    virtual ~YawCalculator() = default;
};

template <typename T, typename TimeType = T, typename WT = T> class PitchCalculator : virtual public PitchGetter<T>, virtual public AngularSpeedGetter<T> {
protected:
    YPRElementCalculatorConfig<T, TimeType, WT> pitchConfig{};
    
    virtual util::Error internalPitchInit(const YPRElementCalculatorConfig<T, TimeType, WT>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    virtual util::Result<T> internalNonIntegralPartPitchCalculation() const {
        return T();
    }
    
public:
    
    virtual util::Error initPitchCalculator(const YPRElementCalculatorConfig<T, TimeType, WT>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        pitchConfig = config;
        pitchConfig.integrator.setIntegral(pitchConfig.integrator.getIntegral() + pitchConfig.offset);
        
        return internalPitchInit(pitchConfig);
    }

    virtual util::Result<T> calculatePitch(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<AngularSpeed<T>> angularSpeed = this->getAngularSpeed();
        if(angularSpeed) return angularSpeed.Err();
        
        util::Result<T> temp = pitchConfig.integrator.update(currentTime, angularSpeed().at(1));
        if(temp) return temp.Err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartPitchCalculation();
        if(nonIntegral) return nonIntegral.Err();
        
        pitchConfig.integrator.setIntegral(temp() * pitchConfig.integralWeight + nonIntegral() * (T(1) - pitchConfig.integralWeight));
        
        return pitchConfig.integrator.getIntegral();
        
    }
    
    virtual ~PitchCalculator() = default;
};

template <typename T, typename TimeType = T, typename WT = T> class RollCalculator : virtual public RollGetter<T>, virtual public AngularSpeedGetter<T> {
protected:
    YPRElementCalculatorConfig<T, TimeType, WT> rollConfig{};
    
    virtual util::Error internalRollInit(const YPRElementCalculatorConfig<T, TimeType, WT>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        return util::ErrorCode::success;
    };
    
    virtual util::Result<T> internalNonIntegralPartRollCalculation() const {
        return T();
    }
    
public:
    
    virtual util::Error initRollCalculator(const YPRElementCalculatorConfig<T, TimeType, WT>& config)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        rollConfig = config;
        rollConfig.integrator.setIntegral(rollConfig.integrator.getIntegral() + rollConfig.offset);
        
        return internalRollInit(rollConfig);
    }

    virtual util::Result<T> calculateRoll(const TimeType& currentTime)
    noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
        
        util::Result<AngularSpeed<T>> angularSpeed = this->getAngularSpeed();
        if(angularSpeed) return angularSpeed.Err();
        
        util::Result<T> temp = rollConfig.integrator.update(currentTime, angularSpeed().at(2));
        if(temp) return temp.Err();
        
        util::Result<T> nonIntegral = internalNonIntegralPartRollCalculation();
        if(nonIntegral) return nonIntegral.Err();
        
        rollConfig.integrator.setIntegral(temp() * rollConfig.integralWeight + nonIntegral() * (T(1) - rollConfig.integralWeight));
        
        return rollConfig.integrator.getIntegral();
        
    }
    
    virtual ~RollCalculator() = default;
};

template <typename T, typename TimeType = T, typename WT = T> class YPRCalculator :
    virtual public YawCalculator<T, TimeType, WT>,
    virtual public PitchCalculator<T, TimeType, WT>,
    virtual public RollCalculator<T, TimeType, WT> {
public:

    virtual util::Error initCalculator(const YPRElementCalculatorConfig<T, TimeType, WT>& yawConfig,
                                    const YPRElementCalculatorConfig<T, TimeType, WT>& pitchConfig,
                                    const YPRElementCalculatorConfig<T, TimeType, WT>& rollConfig)
        noexcept(util::numberNoexcept<T>() && util::numberNoexcept<TimeType>()) {
            
            util::Error err = this->initYawCalculator(yawConfig);
            if(err) return err;
            
            err = this->initPitchCalculator(pitchConfig);
            if(err) return err;
            
            err = this->initRollCalculator(rollConfig);
            if(err) return err;
            
            return util::ErrorCode::success;
        }

    virtual util::Result<YPR<T>> calculateYPR(const TimeType& currentTime) noexcept(util::numberNoexcept<T>()) {
        
        util::Result<T> yaw = this->calculateYaw(currentTime);
        if(yaw) return yaw.Err();
        
        util::Result<T> pitch = this->calculatePitch(currentTime);
        if(pitch) return pitch.Err();
        
        util::Result<T> roll = this->calculateRoll(currentTime);
        if(roll) return roll.Err();
        
        return YPR<T>{yaw(), pitch(), roll()};
    }
    
    virtual ~YPRCalculator() = default;
};


//extended calculators

template <typename T, typename TimeType = T, typename WT = T> class PitchCalculatorWithAcceleration :
    virtual public PitchCalculator<T, TimeType, WT>,
    virtual public AccelerationGetter<T> {
protected:
    virtual util::Result<T> internalNonIntegralPartPitchCalculation() const override {
        util::Result<Acceleration<T>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.Err();
        
        // T accX = acceleration().at(0);
        T accY = acceleration().at(1);
        T accZ = acceleration().at(2);
        
        T temp = sqrt(accY * accY + accZ * accZ);
        if(temp == T(0)) return T(0);
        
        
        return util::rad2Deg(atan2(accY, temp));
    }

public:

    virtual ~PitchCalculatorWithAcceleration() = default;
};

template <typename T, typename TimeType = T, typename WT = T> class RollCalculatorWithAcceleration : virtual public RollCalculator<T, TimeType, WT>, virtual public AccelerationGetter<T> {
protected:
    virtual util::Result<T> internalNonIntegralPartRollCalculation() const override {
        util::Result<Acceleration<T>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.Err();
        
        T accY = acceleration().at(1);
        T accZ = acceleration().at(2);
        
        if(accZ == T(0)) return T(90);
        
        
        return util::rad2Deg(atan2(accY, accZ));
    }

public:

    virtual ~RollCalculatorWithAcceleration() = default;
};

template <typename T, typename TimeType = T, typename WT = T> class YPRCalculatorWithAcceleration
    : public virtual YPRCalculator<T, TimeType>, public PitchCalculatorWithAcceleration<T, TimeType>, public RollCalculatorWithAcceleration<T, TimeType, WT> {
public:
    virtual ~YPRCalculatorWithAcceleration() = default;
};

// controllers

template <typename YPRType, typename AccAngularSpeedType> class GyroDataGetter
    : public YPRGetter<YPRType>, virtual public AccelerationGetter<AccAngularSpeedType>, virtual public AngularSpeedGetter<AccAngularSpeedType> {

public:
    virtual util::Result<GyroData<YPRType, AccAngularSpeedType>> getGyroData() const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->getYPR();
        if(ypr) return ypr.Err();
        
        util::Result<util::Vector<AccAngularSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.Err();
        
        util::Result<util::Vector<AccAngularSpeedType>> speed = this->getAngularSpeed();
        if(speed) return speed.Err();
        
        return GyroData<YPRType, AccAngularSpeedType>{ypr(), acceleration(), speed()};
    }
    
    virtual ~GyroDataGetter() = default;
};

template <typename YPRType, typename TimeType = YPRType, typename WT = YPRType, typename AccAngularSpeedType = YPRType> class GyroDataCalculator
    : virtual public YPRCalculator<YPRType, TimeType, WT>, virtual public AccelerationGetter<AccAngularSpeedType>, virtual public AngularSpeedGetter<AccAngularSpeedType> {

public:
    virtual util::Result<GyroData<YPRType, AccAngularSpeedType>> calculateGyroData(const TimeType& current) const noexcept(util::numberNoexcept<YPRType>() && util::numberNoexcept<AccAngularSpeedType>()) {
        
        util::Result<YPR<YPRType>> ypr = this->calculateYPR(current);
        if(ypr) return ypr.Err();
        
        util::Result<Acceleration<AccAngularSpeedType>> acceleration = this->getAcceleration();
        if(acceleration) return acceleration.Err();
        
        util::Result<AngularSpeed<AccAngularSpeedType>> speed = this->getAngularSpeed();
        if(speed) return speed.Err();
        
        return GyroData<YPRType, AccAngularSpeedType>{ypr(), acceleration(), speed()};
    }
    
    virtual ~GyroDataCalculator() = default;
};

template <typename YPRType, typename TimeType = YPRType, typename WT = YPRType, typename AccAngularSpeedType = YPRType> class GyroDataCalculatorWithAcceleration
    : virtual public YPRCalculatorWithAcceleration<YPRType, TimeType, WT>, public GyroDataCalculator<YPRType, TimeType, WT, AccAngularSpeedType> {
public:
    
    using YPRCalculatorWithAcceleration<YPRType, TimeType, WT>::calculateYPR;

    virtual ~GyroDataCalculatorWithAcceleration() = default;
};

template <typename YPRType, typename TimeType = YPRType, typename WT = YPRType, typename AccAngularSpeedType = YPRType, typename UpdateParameterType = TimeType> class UltimateGyroCalculator :
    public BaseGyroController<UpdateParameterType>,
    public GyroDataGetter<YPRType, AccAngularSpeedType>,
    public GyroDataCalculatorWithAcceleration<YPRType, TimeType, WT, AccAngularSpeedType> {

public:
    
    virtual inline vislib::util::Result<double> getYaw() const noexcept override {
        return this->yawConfig.integrator.getIntegral();
    }

    virtual inline vislib::util::Result<double> getPitch() const noexcept override {
        return this->pitchConfig.integrator.getIntegral();
    }

    virtual inline vislib::util::Result<double> getRoll() const noexcept override {
        return this->rollConfig.integrator.getIntegral();
    }
    
    virtual inline util::Error calibrate() noexcept override {
        
        this->yawConfig.offset += this->getYaw()();
        this->yawConfig.integrator.setIntegral(this->yawConfig.offset);
        
        this->rollConfig.offset += this->getRoll()();
        this->rollConfig.integrator.setIntegral(this->rollConfig.offset);
        
        this->pitchConfig.offset += this->getPitch()();
        this->pitchConfig.integrator.setIntegral(this->pitchConfig.offset);
        
        return {};
    }
    
    virtual inline util::Error update(UpdateParameterType currentTime) override {
        auto e = this->calculateYPR(currentTime);

        if (e) return e.Err();

        return {};
    }
    
    virtual ~UltimateGyroCalculator() = default;
};

template <typename YPRType, typename TimeType = YPRType, typename WT = YPRType, typename AccAngularSpeedType = YPRType, typename UpdateParameterType = TimeType> class UltimateGyroGetter :
    public BaseGyroController<UpdateParameterType>,
    public GyroDataGetter<YPRType, AccAngularSpeedType> {

public:
    
    virtual ~UltimateGyroGetter() = default;
};

} // namespace vislib::gyro
