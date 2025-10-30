#pragma once

namespace vislib::util {

template <typename T> constexpr T&& move(T& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T> constexpr void swap(T& x, T& y) noexcept {
    T temp = move(x);
    x = move(y);
    y = move(temp);
}

template <typename T> void deleter(T *ptr) noexcept {
    delete ptr;
}

template<typename T, void (*DELETER)(T*) = deleter> class UniquePtr {
protected:
    T *ptr = nullptr;

public:
    UniquePtr() = default;

    UniquePtr(const T& v) noexcept {
        ptr = new T(v);
    }

    UniquePtr(T *p_ptr) : ptr(p_ptr) {}

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    ~UniquePtr() noexcept {
        DELETER(ptr);
    }

    T& operator*() const noexcept {
        return *ptr;
    }

    T* operator->() const noexcept {
        return ptr;
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    UniquePtr<T>& operator=(const UniquePtr&) = delete;

    UniquePtr<T>& operator=(UniquePtr&& other) noexcept {
        DELETER(ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    UniquePtr<T>& operator=(const T& v) noexcept {
        if(ptr == nullptr) {
            ptr = new T(v);
        } else {
            *ptr = v;
        }
        return *this;
    }

    T* get() const noexcept {
        return ptr;
    }

    T* release() noexcept {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T *new_ptr = nullptr) noexcept {
        DELETER(ptr);
        ptr = new_ptr;
    }

};

} //namespace vislib::util
