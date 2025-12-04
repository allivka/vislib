#pragma once

#include "util/util.hpp"

namespace vislib {
    template <typename Port_t> class CallbackTable;
    class Callback;
    template <typename Port_t> class CallbackPortChecker;
    template <typename Port_t> class CallbackPortInitializer;
    template <typename Port_t> class CallbackPortAttacher;
}

class vislib::Callback {
public:
    virtual void operator()() = 0;
    virtual ~Callback() = default;
};

template <typename Port_t> class vislib::CallbackPortChecker {
public:
    virtual bool operator()(const Port_t&) = 0;
    virtual ~CallbackPortChecker() = default;
};

template <typename Port_t> class vislib::CallbackPortInitializer {
public:
    virtual util::Error operator()(const Port_t&) = 0;
    virtual ~CallbackPortInitializer() = default;
};

template <typename Port_t> class vislib::CallbackPortAttacher {
public:
    virtual util::Error operator()(const Port_t&, util::UniquePtr<Callback>) = 0;
    virtual ~CallbackPortAttacher() = default;
};

template <typename Port_t> class vislib::CallbackTable {
protected:
    util::Array<Port_t> ports;
    util::UniquePtr<CallbackPortAttacher<Port_t>> attacher;
    util::UniquePtr<CallbackPortChecker<Port_t>> checker;
    util::Array<util::UniquePtr<Callback>> callbacks;


public:
    CallbackTable(const util::Array<Port_t>& ports,
        util::UniquePtr<CallbackPortInitializer<Port_t>> initializer,
        util::UniquePtr<CallbackPortAttacher<Port_t>> attacher,
        util::UniquePtr<CallbackPortChecker<Port_t>> checker) : ports(ports), attacher(attacher), checker(checker) {

        Port_t max = ports[0];
        for (size_t i = 0; i < ports.Size(); i++) max = max >= ports[i] ? max : ports[i];

        callbacks = util::Array<util::UniquePtr<Callback>>(max + 1);

        for (size_t i = 0; i < ports.Size(); i++) {
            auto e = initializer->operator()(ports[i]);

            if (e) {
                //Shit this is bad.
                //I have no ideas how to handle this error inside of constructor.
                //Let's assume it works perfectly fine every single time
            };
        }

        for (size_t i = 0; i < callbacks.Size(); i++) callbacks[i] = nullptr;

    }

    bool isCallbackPort(const Port_t& port) const {
        bool found = false;

        for (size_t i = 0; i < ports.Size(); i++) {
            if (ports[i] == port) {
                found = true;
                break;
            }
        }

        return found;
    }

    util::Error setCallback(const Port_t& port, util::UniquePtr<Callback> callback) {

        if (!isCallbackPort()) return
        {util::ErrorCode::invalidArgument, "The port " + util::to_string(static_cast<size_t>(port)) + " is not set for callbacks"};
        callbacks[static_cast<size_t>(port)] = util::move(callback);

        auto e = attacher->operator()(port);

        if (e) return e;

        return {};
    }

    util::Error manualProcess(const Port_t& port) {

        if (!isCallbackPort(port)) return
        {util::ErrorCode::invalidArgument, "The port " + util::to_string(static_cast<size_t>(port)) + " is not set for callbacks"};

        if (callbacks[static_cast<size_t>(port)] == nullptr) return
        {util::ErrorCode::invalidConfiguration, "The port " + util::to_string(static_cast<size_t>(port)) + " has no set callback"};

        if (checker->operator()(port)) callbacks[static_cast<size_t>(port)]->operator()();

        return {};

    }
};
