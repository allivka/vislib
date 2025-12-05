#pragma once

#include <assert.h>

namespace vislib::util {

template <typename ReturnType = void, typename... ArgumentTypes>
class CallableInterface {
public:

    virtual ReturnType operator()(ArgumentTypes...) = 0;
    virtual ~CallableInterface() = default;
};

template <typename CallableType, typename ReturnType, typename... ArgumentTypes>
class CallableWrapper : public CallableInterface<ReturnType, ArgumentTypes...> {
    CallableType callable;

public:
    explicit CallableWrapper(const CallableType& c) : callable(c) {}
    explicit CallableWrapper(CallableType&& c) : callable(static_cast<CallableType&&>(c)) {}

    ReturnType operator()(ArgumentTypes... args) override {
        return callable(args...);
    }
};

template <typename ReturnType = void, typename... ArgumentTypes>
class Callable {
    CallableInterface<ReturnType, ArgumentTypes...>* impl = nullptr;

public:
    Callable() = default;

    template<typename C> Callable(C c) {
        using Wrapper = CallableWrapper<C, ReturnType, ArgumentTypes...>;
        impl = new Wrapper(static_cast<C&&>(c));
    }

    ReturnType operator()(ArgumentTypes... args) {
        assert(impl != nullptr);
        return (*impl)(args...);
    }

    ReturnType execute(ArgumentTypes... args) const {
        assert(impl != nullptr);
        return (*impl)(args...);
    }

    CallableInterface<ReturnType, ArgumentTypes...> raw() const {
        assert(impl != nullptr);
        return *impl;
    }

    bool isValid() const noexcept {
        return impl != nullptr;
    }

    void reset() {
        delete impl;
        impl = nullptr;
    }

    ~Callable() {
        delete impl;
    }
};

} // namespace vislib::util
