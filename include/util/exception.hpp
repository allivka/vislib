#pragma once

namespace vislib::util {

template <typename T> T&& declval() noexcept;

template <typename T> inline constexpr bool basicArithmeticNoexcept() noexcept {
    return noexcept(declval<T>() + declval<T>()) &&
    noexcept(declval<T>() - declval<T>()) &&
    noexcept(declval<T>() * declval<T>()) &&
    noexcept(declval<T>() / declval<T>()) &&
    noexcept(-declval<T>());
}

template <typename T> inline constexpr bool arithmeticNoexcept() noexcept {
    return basicArithmeticNoexcept<T>() &&
    noexcept(declval<T>() == declval<T>()) &&
    noexcept(declval<T>() != declval<T>()) &&
    noexcept(declval<T>() <= declval<T>()) &&
    noexcept(declval<T>() >= declval<T>()) &&
    noexcept(declval<T>() > declval<T>()) &&
    noexcept(declval<T>() < declval<T>()) &&
    noexcept(declval<T&>() = declval<T>()) &&
    noexcept(declval<T&>() += declval<T>()) &&
    noexcept(declval<T&>() -= declval<T>()) &&
    noexcept(declval<T&>() *= declval<T>()) &&
    noexcept(declval<T&>() /= declval<T>());
}

template <typename T, typename U, typename... Rest> inline constexpr bool basicArithmeticNoexcept() noexcept {
    return basicArithmeticNoexcept<T>() && basicArithmeticNoexcept<U, Rest...>();
}

template <typename T, typename U, typename... Rest> inline constexpr bool arithmeticNoexcept() noexcept {
    return arithmeticNoexcept<T>() && arithmeticNoexcept<U, Rest...>();
}


} // namespace vislib::util