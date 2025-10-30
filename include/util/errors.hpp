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

    ReturnResult(T v) noexcept : errorFlag(false), value(v) { }

    ReturnResult(E e) noexcept : errorFlag(true), err(e) { }

    bool isError() const noexcept { return errorFlag; }
    bool isOK() const noexcept { return !errorFlag; }

    bool getValue(T& out) const noexcept {
        if (!errorFlag) {
            out = value;
            return true;
        }
        return false;
    }

    bool getError(E& out) const noexcept {
        if (errorFlag) {
            out = err;
            return true;
        }
        return false;
    }

    operator T() const noexcept { return value; }
    operator E() const noexcept { return err; }

    operator bool() const noexcept { return errorFlag; }

    T operator()() const noexcept { return value; }

    T Value() const noexcept { return value; }

    E Err() const noexcept { return err; }
};

template <typename T> class Result : public ReturnResult<T, Error> {
public:
    Result(T v) noexcept : ReturnResult<T, Error>(v) {}
    Result(Error e) noexcept : ReturnResult<T, Error>(e) {}
};

template <typename T> class Result<T&> : public ReturnResult<T*, Error> {
public:
    Result(T& v) noexcept : ReturnResult<T*, Error>(&v) {}
    Result(Error e) noexcept : ReturnResult<T*, Error>(e) {}
    
    T& operator()() const noexcept { return *(this->value); }

    T& Value() const noexcept { return *(this->value); }
};


} //namespace vislib::util
