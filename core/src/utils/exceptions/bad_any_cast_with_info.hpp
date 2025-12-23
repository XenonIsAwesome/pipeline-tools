#pragma once

#include <nstd/types/any.hpp>
#include <utils/misc_utils.h>

namespace pt::exceptions {
class bad_any_cast_with_info : public nstd::bad_any_cast {
public:
    bad_any_cast_with_info(const std::type_info& from, const std::type_info& to) :
        bad_any_cast() {
        msg = "bad_any_cast: Cannot cast from type [" + utils::demangle(from.name()) +
              "] to type [" + utils::demangle(to.name()) + "]";
    }

    const char* what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};
}  // namespace pt::exceptions

namespace pt::utils {
template<typename T>
T any_cast_with_info(nstd::any a) {
    try {
        return std::move(nstd::any_cast<T>(std::move(a)));
    } catch (const nstd::bad_any_cast&) {
        throw exceptions::bad_any_cast_with_info(a.type(), typeid(T));
    }
}
}  // namespace pt::utils