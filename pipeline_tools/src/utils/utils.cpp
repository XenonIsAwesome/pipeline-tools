#include <utils/utils.h>
#include <cxxabi.h>

std::string pt::utils::demangle(const char *name) {
#if defined(__GNUG__)
    int status = 0;
    char *demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string out = (status == 0 && demangled) ? demangled : name;
    std::free(demangled);
    return out;
#else
    return name; // fallback: raw typeid name
#endif
}
