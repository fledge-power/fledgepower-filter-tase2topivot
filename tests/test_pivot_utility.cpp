#include <gtest/gtest.h>

#include "tase2_pivot_utility.hpp"

TEST(PivotTASE2PluginUtility, Join)
{
    ASSERT_STREQ(TASE2PivotUtility::join({}).c_str(), "");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST"}).c_str(), "TEST");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST", "TOAST", "TASTE"}).c_str(), "TEST, TOAST, TASTE");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST", "", "TORTOISE"}).c_str(), "TEST, , TORTOISE");
    ASSERT_STREQ(TASE2PivotUtility::join({}, "-").c_str(), "");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST"}, "-").c_str(), "TEST");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST", "TOAST", "TASTE"}, "-").c_str(), "TEST-TOAST-TASTE");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST", "", "TORTOISE"}, "-").c_str(), "TEST--TORTOISE");
    ASSERT_STREQ(TASE2PivotUtility::join({}, "").c_str(), "");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST"}, "").c_str(), "TEST");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST", "TOAST", "TASTE"}, "").c_str(), "TESTTOASTTASTE");
    ASSERT_STREQ(TASE2PivotUtility::join({"TEST", "", "TORTOISE"}, "").c_str(), "TESTTORTOISE");
}

TEST(PivotTASE2PluginUtility, Logs)
{
    std::string text("This message is at level %s");
    ASSERT_NO_THROW(TASE2PivotUtility::log_debug(text.c_str(), "debug"));
    ASSERT_NO_THROW(TASE2PivotUtility::log_info(text.c_str(), "info"));
    ASSERT_NO_THROW(TASE2PivotUtility::log_warn(text.c_str(), "warning"));
    ASSERT_NO_THROW(TASE2PivotUtility::log_error(text.c_str(), "error"));
    ASSERT_NO_THROW(TASE2PivotUtility::log_fatal(text.c_str(), "fatal"));
}