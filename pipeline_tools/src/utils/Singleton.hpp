#pragma once

namespace pt::utils {

template<typename Derived>
class Singleton {
public:
    Singleton() = default;
    virtual ~Singleton() = default;

    template<typename ...Args>
    Derived* getInstance(Args... args) {
        if (instance == nullptr) {
            instance = new Derived(args...);
        }
        return instance;
    }

    Derived* getInstance() {
        return getInstance<>();
    }



private:
    Derived* instance{nullptr};
};

}