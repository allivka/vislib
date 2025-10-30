#pragma once

#include <math.h>
#include "containers.hpp"
#include "memory.hpp"

namespace vislib::util {

template <typename T> T absF(const T& x) noexcept {
    if (x < static_cast<T>(0)) return x * static_cast<T>(-1);
    return x;
}

template <typename T> inline T square(const T& x) noexcept {
    return x * x;
}

template <typename T> inline T sqF(const T& x) noexcept {
    return x * x;
}

template <typename T> char signF(const T& x) noexcept {
    if (x < static_cast<T>(0)) return -1;
    if (x > static_cast<T>(0)) return 1;
    return 0;
}

template <typename T> T simpleMul(const T& value, size_t count) noexcept {
    T buffer = value;
    
    for(size_t i = 1; i < count; i++) {
        buffer += value;
    }
    
    return buffer;
}

template <typename T> T simplePow(const T& value, size_t count) noexcept {
    T buffer = value;
    
    for(size_t i = 1; i < count; i++) {
        buffer *= value;
    }
    
    return buffer;
}

template <typename T> T minF(const T& x, const T& y) noexcept {
    if(x < y) return x;
    return y;
}

template <typename T> T maxF(const T& x, const T& y) noexcept {
    if(x > y) return x;
    return y;
}

template <typename T> T minEq(const T& x, const T& y) noexcept {
    if(x <= y) return x;
    return y;
}

template <typename T> T maxEq(const T& x, const T& y) noexcept {
    if(x >= y) return x;
    return y;
}

double cosDegrees(double angle) noexcept {
    return cos(angle * M_PI / 180.0);
}

double sinDegrees(double angle) noexcept {
    return sin(angle * M_PI / 180.0);
}

double deg2Rad(double angle) noexcept {
    return angle * M_PI / 180.0;
}

double rad2Deg(double angle) noexcept {
    return angle * 180.0 / M_PI;
}

template <typename T> class Range {
public:
    T lowest = 0;
    T highest = 0;

    template<typename D> static D map(D x, D in_min, D in_max, D out_min, D out_max) noexcept {
        if (in_max == in_min) {
            return out_min;
        }
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    template<typename D> static D map(D x, const Range<D>& in, const Range<D>& out) noexcept {
        return map(x, in.lowest, in.highest, out.lowest, out.highest);
    }

    Range() = default;
    Range(const Range&) = default;
    Range(T p_lowest, T p_highest) noexcept : lowest(p_lowest), highest(p_highest) {}

    bool contains(T v) const noexcept {
        return v >= lowest && v <= highest;
    }

    T restrict(T v) const noexcept {
        if (v < lowest) return lowest;
        if (v > highest) return highest;
        return v;
    }

    T mapValueFromRange(T v, Range<T> r) const noexcept {
        return map(v, r, *this);
    }

    T mapValueToRange(T v, Range<T> r) const noexcept {
        return map(v, *this, r);
    }

};

template <typename T> class Vector {
protected:
    Array<T> data;
public:
    Vector() = default;
    Vector(const Vector&) = default;
    Vector(Vector&&) = default;
    Vector& operator=(const Vector&) = default;
    Vector& operator=(Vector&&) = default;
    
    explicit Vector(const Array<T>& arr) noexcept : data(arr) { }
    explicit Vector(Array<T>&& arr) noexcept : data(util::move(arr)) { }
    
    operator Array<T>&() noexcept {
        return data;
    }
    
    operator const Array<T>&() const noexcept {
        return data;
    }
    
    Array<T>& raw() noexcept {
        return data;
    }
    
    const Array<T>& raw() const noexcept {
        return data;
    }
    
    Vector operator+(const Vector& other) const noexcept {
        Vector<T> temp = *this;
        for(size_t i = 0; i < minF(temp.Size(), other.Size()); i++) {
            temp.at(i) += other.at(i);
        }
        return temp;
    }
    
    Vector& operator+=(const Vector& other) noexcept {
        for(size_t i = 0; i < minF(this->Size(), other.Size()); i++) {
            this->at(i) += other.at(i);
        }
        return *this;
    }
    
    Vector operator-(const Vector& other) const noexcept {
        Vector<T> temp = *this;
        for(size_t i = 0; i < minF(temp.Size(), other.Size()); i++) {
            temp.at(i) -= other.at(i);
        }
        return temp;
    }
    
    Vector& operator-=(const Vector& other) noexcept {
        for(size_t i = 0; i < minF(this->Size(), other.Size()); i++) {
            this->at(i) -= other.at(i);
        }
        return *this;
    }
    
    Vector operator*(const T& value) const noexcept {
        Vector<T> temp = *this;
        for(size_t i = 0; i < temp.Size(); i++) {
            temp.at(i) *= value;
        }
        return temp;
    }
    
    Vector& operator*=(const T& value) noexcept {
        for(size_t i = 0; i < this->Size(); i++) {
            this->at(i) *= value;
        }
        return *this;
    }
    
    Vector operator/(const T& value) const noexcept {
        if(value == 0) return *this;
        
        Vector<T> temp = *this;
        for(size_t i = 0; i < temp.Size(); i++) {
            temp.at(i) /= value;
        }
        
        return temp;
    }
    
    Vector& operator/=(const T& value) noexcept {
        if(value == 0) return *this;
        
        for(size_t i = 0; i < this->Size(); i++) {
            this->at(i) /= value;
        }
        
        return *this;
    }
    
    Vector operator-() const noexcept {
        Vector<T> temp = *this;
        for(size_t i = 0; i < temp.Size(); i++) {
            temp.at(i) = -temp.at(i);
        }
        return temp;
    }
    
    double module() const noexcept {
        double buffer = 0;
        
        for(size_t i = 0; i < this->Size(); i++) {
            buffer += this->at(i) * this->at(i);
        }
        
        return sqrt(buffer);
        
    }
    
    double dot(const Vector& other) const noexcept {
        double buffer = 0;
        
        for(size_t i = 0; i < minF(other.Size(), this->Size()); i++) {
            buffer += this->at(i) * other.at(i);
        }
        
        return buffer;
    }
    
    Vector normal() const noexcept {
        return *this / this->module();
    }
    
    void normalize() noexcept {
        auto m = this->module();
        if (m != 0) *this /= m;
    }
    
};

template<typename T> Vector<T> operator*(const T& val, const Vector<T>& vec) noexcept {
    return vec * val;
}

} //namespace vislib::util