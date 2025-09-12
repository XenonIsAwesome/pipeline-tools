#pragma once

namespace pt::utils {

template<typename Derived>
class Singleton {
public:
    Singleton() = default;
    virtual ~Singleton() = default;

    template<typename ...Args>
    static Derived* getInstance(Args... args) {
        if (instance == nullptr) {
            instance = new Derived(std::forward<Args>(args)...);
        }
        return instance;
    }

    static Derived* getInstance() {
        return getInstance<>();
    }

private:
    static Derived* instance;
};

template<typename Derived>
Derived* Singleton<Derived>::instance = nullptr;

}