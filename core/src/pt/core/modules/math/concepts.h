#pragma once

#include <concepts>

namespace pt::modules::concepts {
    template<typename In, typename Add, typename Out>
    concept Addable = requires(In in, Add add)
    {
        { in + add } -> std::convertible_to<Out>;
        { in += add } -> std::convertible_to<Out>;
    };
}