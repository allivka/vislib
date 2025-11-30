#pragma once

#include "errordef.hpp"
#include "exception.hpp"

namespace vislib::util {

template<typename T> class TimeGetter {
public:
    virtual Result<T> getTime() const = 0;
    virtual ~TimeGetter() = default;
};

template<typename T> class Timer : virtual public TimeGetter<T>{
protected:
    T startTime{};
    T targetTime{};
    TimeGetter<T> getter{};
public:
    Timer(const TimeGetter<T>& timeGetter) noexcept(assignableNoexcept<T>()) : getter(timeGetter) {}

    virtual Result<T> start() noexcept(assignableNoexcept<T>()) {

        Result<T> e = getter.getTime();

        if (e) return e.err();

        startTime = e;

        return startTime;

    }

    virtual Result<T> getTime() const noexcept(numberNoexcept<T>()) override {
        Result<T> e = getter.getTime();

        if (e) return e.err();

        if (e() < startTime) return {ErrorCode::invalidConfiguration, "The measurement start time is ahead of  current time"};

        return e() - startTime;

    }

    virtual ~Timer() = default;
};

}