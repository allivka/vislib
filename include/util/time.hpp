#pragma once

#include "errordef.hpp"
#include "exception.hpp"

namespace vislib::util {

template<typename T, typename Derived> class TimeGetter {
public:
    Result<T> getTime() const {
        return static_cast<const Derived*>(this)->getTimeImplementation();
    }
    
    Result<T> getTimeImplementation() const noexcept(numberNoexcept<T>()) {
        return Error{ErrorCode::invalidConfiguration, "getTimeImplementation not implemented"};
    }
};

template<typename T, typename Getter> class Timer : public TimeGetter<T, Timer<T, Getter>> {
protected:
    T startTime{};
    T targetTime{};
    TimeGetter<T, Getter> getter{};

public:
    Timer(const TimeGetter<T, Getter>& timeGetter) noexcept(assignableNoexcept<T>()) : getter(timeGetter) {}

    Result<T> start() noexcept(assignableNoexcept<T>()) {

        Result<T> e = getter.getTime();

        if (e) return e.Err();

        startTime = e;

        return startTime;

    }
    
protected:
    
    Result<T> getTimeImplementation() const noexcept(numberNoexcept<T>()) {
        Result<T> e = getter.getTime();

        if (e) return e.Err();

        if (e() < startTime) return Error{ErrorCode::invalidConfiguration, "The measurement start time is ahead of  current time"};

        return e() - startTime;

    }
};

}