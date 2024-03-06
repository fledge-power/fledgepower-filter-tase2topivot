#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::GTEST_FLAG(repeat) = 1;
    testing::GTEST_FLAG(shuffle) = false;
    testing::GTEST_FLAG(death_test_style) = "threadsafe";

    return RUN_ALL_TESTS();
}

