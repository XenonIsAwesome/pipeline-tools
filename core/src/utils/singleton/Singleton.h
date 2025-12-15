#pragma once

#include <memory>

namespace pt::utils {
/**
 * @brief Singleton CTRP class.
 * Derived classes of this singleton will inherit the `getInstance` method
 * @tparam Derived The derived type
 */
template<typename Derived>
class Singleton {
public:
    /**
     * Getting the singleton instance, it is also created here for the first time.
     * @tparam Args variadic arguments for the constructors.
     * @param arguments The arguments for the constructor of the derived type.
     * @return derived object reference
     */
    template<typename... Args>
    static std::shared_ptr<Derived> getInstance(Args... arguments);

    Singleton& operator=(const Singleton&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    static std::shared_ptr<Derived> instance;
};
}  // namespace pt::utils

#include "Singleton.tpp"