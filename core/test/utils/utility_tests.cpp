#include "../FunctionTest.hpp"

#include <gtest/gtest.h>
#include <string>
#include <utils/misc_utils.h>

/**
 * This tests the `pt::utils::parse_numeric_ranges` with the cases below.
 * The first tuple is the input of the function, the vector is the expected output.
 */
FUNCTION_TEST_IMPL(UtilityTests, ParseNumericRangeTest, pt::utils::parse_numeric_ranges, std::vector<int>, std::string) (
    std::make_pair<std::tuple<std::string>, std::vector<int>>({"1"}, {1}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>({"1-3"}, {1,2,3}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>({"1-3,10"}, {1,2,3,10}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>({"1,10-15"}, {1,10,11,12,13,14,15}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>({"1-3,10-15"}, {1,2,3,10,11,12,13,14,15}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>({"1,2,3,10,15"}, {1,2,3,10,15})
));