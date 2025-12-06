#pragma once

#include "util/util.hpp"

namespace vislib {
    template <typename Port_t> class CallbackTable;
    using Callback = util::Callable<void>;
    using CallbackExecutor = util::Callable<util::Error, Callback>;
    template <typename Port_t> using CallbackPortChecker = util::Callable<bool, Port_t>;
    template <typename Port_t> using CallbackPortInitializer = util::Callable<util::Error, Port_t>;
    template <typename Port_t> using CallbackPortAttacher = util::Callable<util::Error, Port_t>;
    
    inline util::Error defaultCallbackExecutor(Callback&& c) {
        c();
        return {};
    }
}

template <typename Port_t> class vislib::CallbackTable {
protected:
    util::Array<Port_t> ports;
    CallbackPortAttacher<Port_t> attacher;
    CallbackPortChecker<Port_t> checker;
    CallbackExecutor executor;
    util::Array<Callback> callbacks;

    bool initialized = false;

public:

    util::Error InitCallbackTable(const util::Array<Port_t>& ports,
        CallbackPortInitializer<Port_t>&& initializer,
        CallbackPortAttacher<Port_t>&& attacher,
        CallbackPortChecker<Port_t>&& checker,
        CallbackExecutor&& executor ) {

        this->ports = ports;
        this->attacher = util::move(attacher);
        this->checker = util::move(checker);
        this->executor = util::move(executor);

        Port_t max = ports[0];
        for (size_t i = 0; i < ports.Size(); i++) max = max >= ports[i] ? max : ports[i];

        callbacks = util::Array<Callback>(max + 1);

        for (size_t i = 0; i < ports.Size(); i++) {
            auto e = initializer.execute(ports[i]);

            if (e) return e;
        }

        initialized = true;

        return {};
    }

    constexpr bool isInitialized() const noexcept {
        return initialized;
    }

    bool isCallbackPort(const Port_t& port) const {
        bool found = false;

        if (!initialized) return false;

        for (size_t i = 0; i < ports.Size(); i++) {
            if (ports[i] == port) {
                found = true;
                break;
            }
        }

        return found;
    }

    util::Error setCallback(const Port_t& port, const Callback& callback) {

        if (!initialized) return {util::ErrorCode::invalidConfiguration, "The callback table wasn't initialized"};

        if (!isCallbackPort(port)) return
        {util::ErrorCode::invalidArgument, "The port " + util::to_string(static_cast<size_t>(port)) + " is not set for callbacks"};
        callbacks[static_cast<size_t>(port)] = callback;

        auto e = attacher.execute(port);

        if (e) return e;

        return {};
    }

    util::Error manualProcess(const Port_t& port) {
        if (!initialized) return {util::ErrorCode::invalidConfiguration, "The callback table wasn't initialized"};

        if (!isCallbackPort(port)) return
        {util::ErrorCode::invalidArgument, "The port " + util::to_string(static_cast<size_t>(port)) + " is not set for callbacks"};

        if (!callbacks[static_cast<size_t>(port)].isValid()) return
        {util::ErrorCode::invalidConfiguration, "The port " + util::to_string(static_cast<size_t>(port)) + " has no set callback"};

        if (!checker.execute(port)) return {};
        
        util::Error e = executor.execute(callbacks[static_cast<size_t>(port)]);
        
        if(e) return e;

        return {};

    }

    util::Error manualProcess() {
        if (!initialized) return {util::ErrorCode::invalidConfiguration, "The callback table wasn't initialized"};

        for (size_t i = 0; i < ports.Size(); i++) {
            if (!callbacks[ports[i]].isValid() || !checker.execute(ports[i])) continue;
            callbacks[ports[i]].execute();
        }
        return {};
    }
};
