#pragma once

#include "errordef.hpp"
#include "exception.hpp"
#include "callable.hpp"
#include "abstract.hpp"

namespace vislib::util {

template <typename T> using TimeGetter = Callable<T>;

template<typename T> class Timer {
protected:
    T startTime{};
    TimeGetter<T> getter{};

public:

    Timer(TimeGetter<T>&& timeGetter) noexcept : getter(move(timeGetter)) {}

    Result<T> start() noexcept(assignableNoexcept<T>()) {

        Result<T> e = getter();

        if (e) return e.Err();

        startTime = e;

        return startTime;

    }

    Result<T> getTime() const noexcept(numberNoexcept<T>()) {
        Result<T> e = getter();

        if (e) return e.Err();

        if (e() < startTime) return Error{ErrorCode::invalidConfiguration, "The measurement start time is ahead of current time"};

        return e() - startTime;

    }

    Result<T> operator()() const noexcept(numberNoexcept<T>()) {
        return getTime();
    }
};

template <typename T> class IncrementTimer {
    protected:

    bool flagPaused = false;
    T timeBuffer{};
    T lastTime{};
    TimeGetter<T> getter{};

    public:

    IncrementTimer(TimeGetter<T>&& getter) noexcept : getter(move(getter)) {}

    Result<T> start() noexcept(assignableNoexcept<T>()) {

        auto e = getter();

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

        auto e = getter();

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

    Result<T> getTime() const noexcept(numberNoexcept<T>()) {
        return timeBuffer;
    }

    Result<T> operator()() const noexcept(numberNoexcept<T>()) {
        return timeBuffer;
    }
};

}