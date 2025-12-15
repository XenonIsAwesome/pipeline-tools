#pragma once

#include <utility>

template<typename Derived>
std::shared_ptr<Derived> pt::utils::Singleton<Derived>::instance = nullptr;

template <typename Derived>
template <typename... Args>
std::shared_ptr<Derived> pt::utils::Singleton<Derived>::getInstance(Args... arguments) {
    if (instance == nullptr) {
        instance = std::make_shared<Derived>(std::forward<Args>(arguments)...);
    }
    return instance;
}