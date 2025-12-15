#pragma once

template<typename In, typename Add, typename Out>
concept Addable = requires(In input, Add addition) {
    { input + addition } -> std::convertible_to<Out>;
};