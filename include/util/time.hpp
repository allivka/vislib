#pragma once

#include "errordef.hpp"
#include "exception.hpp"

namespace vislib::util {

template<typename T, typename Derived> class TimeGetter {
public:
    Result<T> getTime() const {
        return static_cast<const Derived*>(this)->getTimeImplementation();
    }
    
protected:
    Result<T> getTimeImplementation() const noexcept(numberNoexcept<T>()) {
        return Error{ErrorCode::invalidConfiguration, "getTimeImplementation not implemented"};
    }
};

template<typename T, typename Getter> class Timer : public TimeGetter<T, Timer<T, Getter>> {
protected:
    T startTime{};
    TimeGetter<T, Getter> getter{};

public:
    
    Timer(const TimeGetter<T, Getter>& timeGetter) noexcept : getter(timeGetter) {}
    
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

        if (e() < startTime) return Error{ErrorCode::invalidConfiguration, "The measurement start time is ahead of current time"};

        return e() - startTime;

    }
};

template <typename T, typename Getter> class IncrementTimer : public TimeGetter<T, Getter> {
    protected:
    
    bool flagPaused = false;
    T timeBuffer{};
    T lastTime{};
    TimeGetter<T, Getter> getter{};
    
    public:
    friend class TimeGetter<T, Timer<T, Getter>>;
    IncrementTimer(const TimeGetter<T, Getter>& getter) noexcept : getter(getter) {}

    Result<T> start() noexcept(assignableNoexcept<T>()) {

        auto e = getter.getTime();
        
        if(e) return e.Err();
        lastTime = e();

        if (!flagPaused) {
            timeBuffer = T{};
        }

        flagPaused = false;

        return lastTime;
    }
    
    Result<T> update() noexcept(numberNoexcept<T>()) {

        if (flagPaused) return timeBuffer;

        auto e = getter.getTime();
        
        if(e) return e.Err();
        
        T diff = e() - lastTime;
        
        if(diff <= 0) timeBuffer += e();
        else timeBuffer += diff;
        
        return timeBuffer;
        
    }

    Result<T> pause() noexcept(numberNoexcept<T>()) {
        flagPaused = true;
        return timeBuffer;
    }

    bool isPaused() const noexcept {
        return flagPaused;
    }

    void clear() noexcept(numberNoexcept<T>()) {
        flagPaused = false;
        timeBuffer = T{};
        lastTime = T{};
    }
    
    Result<T> operator++() noexcept(numberNoexcept<T>()) {
        return update();
    }

    Result<T> operator++(int) noexcept(numberNoexcept<T>()) {
        return update();
    }
    
    protected:
    
    Result<T> getTimeImplementation() const noexcept(numberNoexcept<T>()) {
        return timeBuffer;
    }
};

}