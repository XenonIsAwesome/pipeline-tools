#pragma once

#include <functional>
#include <gtest/gtest.h>

template<typename Ret, typename... Args>
class FunctionTest : public ::testing::TestWithParam<std::pair<std::tuple<Args...>, Ret>> {};

#define FUNCTION_TEST_IMPL(SUITE_NAME, TEST_NAME, FUNCTION, ...) \
    using TEST_NAME = FunctionTest<__VA_ARGS__>;                 \
    TEST_P(TEST_NAME, Run) {                                     \
        auto& [args, expected] = GetParam();                     \
        EXPECT_EQ(std::apply(FUNCTION, args), expected);         \
    }                                                            \
    INSTANTIATE_TEST_SUITE_P(SUITE_NAME, TEST_NAME, ::testing::Values