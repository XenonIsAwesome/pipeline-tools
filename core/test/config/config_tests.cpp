#include "../FunctionTest.hpp"

#include <config/config.h>
#include <config/convert.h>
#include <gtest/gtest.h>
#include <string>

TEST(ConfigTests, ConvertStringToString) {
    EXPECT_EQ(pt::config::convert<std::string>("test"), std::string("test"));
}

TEST(ConfigTests, ConvertStringToUnsigned) {
    EXPECT_EQ(pt::config::convert<uint32_t>("10"), 10);
}

TEST(ConfigTests, ConvertStringToSigned) {
    EXPECT_EQ(pt::config::convert<int32_t>("-40"), -40);
}

TEST(ConfigTests, ConvertStringToFloatingPoint) {
    EXPECT_EQ(pt::config::convert<double>("12.1"), 12.1);
}

TEST(ConfigTests, GetExitingEnv) {
    auto pipeline_id = pt::config::get<size_t>("CI_PIPELINE_ID");
    ASSERT_TRUE(pipeline_id.has_value());

    auto expected_pipeline_id_str = getenv("CI_PIPELINE_ID");

    ASSERT_NE(expected_pipeline_id_str, nullptr);
    auto expected_pipeline_id = std::stoull(expected_pipeline_id_str);

    EXPECT_EQ(expected_pipeline_id, pipeline_id);
}

TEST(ConfigTests, GetNonexistingEnv) {
    auto value = pt::config::get<size_t>("THERES_NO_WAY_THIS_EXISTS");
    EXPECT_FALSE(value.has_value());
}