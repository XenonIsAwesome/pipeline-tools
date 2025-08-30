#pragma once

#include <any>
#include <string>
#include <typeinfo>
#include <utils/utils.h>

namespace pt::utils::exceptions {
    class bad_any_cast_with_info : public std::bad_cast {
    public:
        bad_any_cast_with_info(const std::type_info &from, const std::type_info &to) {
            msg = "bad_any_cast: cannot cast from [" + demangle(from.name()) +
                  "] to [" + demangle(to.name()) + "]";
        }

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}

namespace pt::utils {
    /**
     * Convenience wrapper
     * @tparam T 'To' type
     * @param a 'From' type (`std::any`)
     * @return `std::any` cast to `T`
     * @throws bad_any_cast_with_info
     */
    template<typename T>
    T any_cast_with_info(std::any a) {
        try {
            return std::move(std::any_cast<T>(std::move(a)));
        } catch (const std::bad_any_cast &) {
            throw exceptions::bad_any_cast_with_info(a.type(), typeid(T));
        }
    }
}
