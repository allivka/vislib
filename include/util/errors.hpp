#pragma once

#include "containers.hpp"

namespace vislib::util {

class Error {
public:
    ErrorCode errcode;
    String msg = "Success";

    operator ErrorCode() const noexcept { return errcode; }

    operator long long() const noexcept { return static_cast<long long>(errcode); }

    operator String() const noexcept { return msg; }

    operator const char*() const noexcept { return msg.c_str(); }

    explicit operator bool() const noexcept { return errcode != ErrorCode::success; }

    virtual bool operator==(const Error& err) const noexcept {
        return errcode == err.errcode;
    }

    Error() noexcept : errcode(ErrorCode::success), msg("Successful operation") {}

    Error(ErrorCode code, String p_msg = "Default error") noexcept : errcode(code), msg(p_msg) {
        if (code == ErrorCode::success) msg = "Successful operation";
        else if(p_msg == "Default error") msg = "Undefined error occur";
    }
};

template <typename T, typename E> class ReturnResult {
protected:

    bool errorFlag = false;
    T value;
    E err;

public:

    ReturnResult(const T& v) noexcept(noexcept(T(v))) : errorFlag(false), value(v) { }

    ReturnResult(const E& e) noexcept(noexcept(E(e))) : errorFlag(true), err(e) { }

    inline constexpr bool isError() const noexcept { return errorFlag; }
    inline constexpr bool isOK() const noexcept { return !errorFlag; }

    bool getValue(T& out) const noexcept(noexcept(out = value)) {
        if (!errorFlag) {
            out = value;
            return true;
        }
        return false;
    }

    bool getError(E& out) const noexcept(noexcept(out = err)) {
        if (errorFlag) {
            out = err;
            return true;
        }
        return false;
    }

    operator T() const noexcept(noexcept(T(value))) { return value; }
    operator E() const noexcept(noexcept(E(err))) { return err; }

    inline constexpr operator bool() const noexcept { return errorFlag; }

    T operator()() const noexcept(noexcept(T(value))) { return value; }

    inline constexpr T Value() const noexcept(noexcept(T(value))) { return value; }

    inline constexpr E Err() const noexcept(noexcept(E(err))) { return err; }
};

template <typename T>
class Result : public ReturnResult<T, Error> {
public:
    Result(T v) noexcept(noexcept(ReturnResult<T, Error>(v)))
        : ReturnResult<T, Error>(v) {}

    Result(Error e) noexcept(noexcept(ReturnResult<T, Error>(e)))
        : ReturnResult<T, Error>(e) {}
};

template <typename T>
class Result<T&> : public ReturnResult<T*, Error> {
public:
    Result(T& v) noexcept : ReturnResult<T*, Error>(&v) {}
    Result(Error e) noexcept : ReturnResult<T*, Error>(e) {}

    T& operator()() const noexcept { return *(this->value); }
    T& Value() const noexcept { return *(this->value); }
};


} //namespace vislib::util
