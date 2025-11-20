#pragma once

#include "memory.hpp"
#include <cassert>


namespace vislib::util {

template <typename To, typename From> To downcast(const From* from) {
    assert(dynamic_cast<To>(from) != nullptr);
    return static_cast<To>(from);
}

} //namespace vislib::util