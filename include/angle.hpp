#pragma once

#include "math.hpp"

namespace vislib::core {

template <typename T = double, typename O = double> class Angle {
protected:
    T value{};
    
public:
    
    enum class CastType {
        degrees,
        radians
    };
    
    CastType castType = CastType::degrees;
    
    static inline T fixDegrees(T angle) {
        angle = fmod(angle, 360);
        if(angle < 0)  angle += 360;
        
        return angle;
    }
    
    static Angle fromDegrees(const T& value) {
        return Angle(fixDegrees(value));
    }
    
    static Angle fromRadians(const T& value) {
        return Angle(fixDegrees(rad2Deg(value)));
    }
    
    static Angle fromDeg(const T& value) {
        return Angle(fixDegrees(value));
    }
    
    static Angle fromRad(const T& value) {
        return Angle(fixDegrees(rad2Deg(value)));
    }
    
    Angle(const T& value) : value(fixDegrees(value)) {}
    
    Angle() noexcept = default;
    Angle(const Angle&) noexcept = default;
    Angle(Angle&&) noexcept = default;
    Angle& operator=(const Angle&) noexcept = default;
    Angle& operator=(Angle&&) noexcept = default;
    ~Angle() = default;
    
    inline void setDegrees(const T& value) {
        this->value = fixDegrees(value);
    }
    
    inline void setRadians(const T& value) {
        this->value = fixDegrees(rad2Deg(value));
    }
    
    inline T degrees() const noexcept {
        return value;
    }
    
    inline T radians() const noexcept {
        return deg2Rad(value);
    }
    
    inline T deg() const noexcept {
        return value;
    }
    
    inline T rad() const noexcept {
        return deg2Rad(value);
    }
    
    explicit inline operator T() const noexcept {
        return (castType == CastType::radians) ? deg2Rad(value) : value;
    }
    
    inline Angle operator-() const {
        return Angle(fixDegrees(-value));
    }
    
    inline Angle& operator++() {
        value = fixDegrees(value + 1);
        return *this;
    }
    
    inline Angle& operator++(int) {
        value = fixDegrees(value + 1);
        return *this;
    }
    
    inline Angle& operator--() {
        value = fixDegrees(value - 1);
        return *this;
    }
    
    inline Angle& operator--(int) {
        value = fixDegrees(value - 1);
        return *this;
    }
    
    inline Angle& operator+=(const O& v) {
        value = fixDegrees(value + v);
        return *this;
    }
    
    inline Angle& operator-=(const O& v) {
        value = fixDegrees(value - v);
        return *this;
    }
    
    inline Angle& operator*=(const O& v) {
        value = fixDegrees(value * v);
        return *this;
    }
    
    inline Angle& operator/=(const O& v) {
        value = fixDegrees(value / v);
        return *this;
    }
    
    inline Angle& operator%=(const O& v) {
        value = fixDegrees(fmod(static_cast<double>(value), static_cast<double>(v)));
        return *this;
    }
    
    inline Angle operator+(const O& v) const {
        return Angle(fixDegrees(value + v));
    }
    
    inline Angle operator-(const O& v) const {
        return Angle(fixDegrees(value - v));
    }
    
    inline Angle operator*(const O& v) const {
        return Angle(fixDegrees(value * v));
    }
    
    inline Angle operator/(const O& v) const {
        return Angle(fixDegrees(value / v));
    }
    
    inline Angle operator%(const O& v) const {
        return Angle(fixDegrees(fmod(static_cast<double>(value), static_cast<double>(v))));
    }
    
    inline Angle& operator+=(const Angle& v) {
        value = fixDegrees(value + v.value);
        return *this;
    }
    
    inline Angle& operator-=(const Angle& v) {
        value = fixDegrees(value - v.value);
        return *this;
    }
    
    inline Angle& operator*=(const Angle& v) {
        value = fixDegrees(value * v.value);
        return *this;
    }
    
    inline Angle& operator/=(const Angle& v) {
        value = fixDegrees(value / v.value);
        return *this;
    }
    
    inline Angle& operator%=(const Angle& v) {
        value = fixDegrees(fmod(static_cast<double>(value), static_cast<double>(v.value)));
        return *this;
    }
    
    inline Angle operator+(const Angle& v) const {
        return Angle(fixDegrees(value + v.value));
    }
    
    inline Angle operator-(const Angle& v) const {
        return Angle(fixDegrees(value - v.value));
    }
    
    inline Angle operator*(const Angle& v) const {
        return Angle(fixDegrees(value * v.value));
    }
    
    inline Angle operator/(const Angle& v) const {
        return Angle(fixDegrees(value / v.value));
    }
    
    inline Angle operator%(const Angle& v) const {
        return Angle(fixDegrees(fmod(static_cast<double>(value), static_cast<double>(v.value))));
    }
    
    inline friend Angle operator+(const O& v, const Angle& a) {
        return Angle(fixDegrees(a.value + v));
    }
    
    inline friend Angle operator*(const O& v, const Angle& a) {
        return Angle(fixDegrees(a.value * v));
    }

    inline bool operator==(const Angle& v) const noexcept { return value == v.value; }
    inline bool operator!=(const Angle& v) const noexcept { return value != v.value; }
    inline bool operator<(const Angle& v) const noexcept { return value < v.value; }
    inline bool operator>(const Angle& v) const noexcept { return value > v.value; }
    inline bool operator<=(const Angle& v) const noexcept { return value <= v.value; }
    inline bool operator>=(const Angle& v) const noexcept { return value >= v.value; }

    inline bool operator==(const O& v) const noexcept { return value == v; }
    inline bool operator!=(const O& v) const noexcept { return value != v; }
    inline bool operator<(const O& v) const noexcept { return value < v; }
    inline bool operator>(const O& v) const noexcept { return value > v; }
    inline bool operator<=(const O& v) const noexcept { return value <= v; }
    inline bool operator>=(const O& v) const noexcept { return value >= v; }

    inline friend bool operator==(const O& lhs, const Angle& rhs) noexcept { return lhs == rhs.value; }
    inline friend bool operator!=(const O& lhs, const Angle& rhs) noexcept { return lhs != rhs.value; }
    inline friend bool operator<(const O& lhs, const Angle& rhs) noexcept { return lhs < rhs.value; }
    inline friend bool operator>(const O& lhs, const Angle& rhs) noexcept { return lhs > rhs.value; }
    inline friend bool operator<=(const O& lhs, const Angle& rhs) noexcept { return lhs <= rhs.value; }
    inline friend bool operator>=(const O& lhs, const Angle& rhs) noexcept { return lhs >= rhs.value; }
};

} //namespace vislib::core