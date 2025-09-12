#include "../testing_utils/FunctionTest.h"
#include <utils/utils.h>

FUNCTION_TEST(UtilityTests, ParseNumericRangeTest, pt::utils::parse_numeric_range, std::vector<int>, std::string) (
    std::make_pair<std::tuple<std::string>, std::vector<int>>(std::make_tuple("1"), {1}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>(std::make_tuple("1-3"), {1,2,3}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>(std::make_tuple("1-3,10"), {1,2,3,10}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>(std::make_tuple("1,10-15"), {1,10,11,12,13,14,15}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>(std::make_tuple("1-3,10-15"), {1,2,3,10,11,12,13,14,15}),
    std::make_pair<std::tuple<std::string>, std::vector<int>>(std::make_tuple("1,2,3,10,15"), {1,2,3,10,15})
));