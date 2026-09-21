/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for M5Utility
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>
#include <cstdint>
#include <utility>

using namespace m5::utility::log;

namespace {
using pf_t = std::pair<const char* /* full path */, const char* /* filename */>;

pf_t table[] = {
    {"", ""},         {nullptr, ""},
    {"aaa", "aaa"},   {"ソソソソ", "ソソソソ"},
    {"a/b.c", "b.c"}, {"c:/aaa/bbb/ccc/ddd.eee", "ddd.eee"},
};

}  // namespace

TEST(Log, pathToFilename)
{
    {
        constexpr auto fn0 = pathToFilename("");
        EXPECT_STREQ(fn0, "");
        constexpr auto fn1 = pathToFilename("ABC");
        EXPECT_STREQ(fn1, "ABC");
        constexpr auto fn2 = pathToFilename("a:/bb/ccc/dddd/eee.f");
        EXPECT_STREQ(fn2, "eee.f");
    }

    for (auto&& e : table) {
        EXPECT_STREQ(pathToFilename(e.first), e.second) << e.first;
    }
}

TEST(Log, logMacroArgumentCount)
{
    // Compile both the empty variadic case and a call beyond the former
    // fixed dispatcher limit without producing test output.
    if (false) {
        M5_LIB_LOGI("plain");
        M5_LIB_LOGI("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    }
}

TEST(Log, timestampType)
{
    // timestamp_t is std::chrono::milliseconds where <chrono> is usable and a
    // 32-bit struct otherwise; only the default construction and count() are
    // common to both, and that is all the log path relies on
    const timestamp_t ts{};
    EXPECT_EQ(static_cast<uint64_t>(ts.count()), 0ULL);
}

TEST(Log, elapsedTime)
{
    const auto t0 = elapsedTime();
    m5::utility::delay(10);
    const auto t1 = elapsedTime();

    EXPECT_GE(static_cast<uint64_t>(t1.count()), static_cast<uint64_t>(t0.count()))
        << "elapsedTime() must not go backwards";
    EXPECT_GE(static_cast<uint64_t>(t1.count() - t0.count()), 5ULL) << "elapsedTime() should advance with the clock";
}

TEST(Log, macros)
{
    // Exercises M5_UTILITY_LOG_TS_FMT / M5_UTILITY_LOG_TS_VAL, whose format
    // specifier follows timestamp_t and so differs between the <chrono> path
    // and the 32-bit fallback. A mismatch there misaligns the varargs.
    M5_LIB_LOGE("Error");
    M5_LIB_LOGW("Warn");
    M5_LIB_LOGI("Info");
    M5_LIB_LOGD("Debug");
    M5_LIB_LOGV("Verbose");
}

TEST(Log, dump)
{
    constexpr uint8_t test[] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,
        0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67,
        0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    };

    // Offsets and lengths that are not a multiple of the dump width: the dump
    // must stop at the end of the array
    M5_DUMPE(test + 0, sizeof(test) - 0);
    M5_DUMPE(test + 1, sizeof(test) - 1);
    M5_DUMPE(test + 7, sizeof(test) - 7);
    M5_DUMPE(test + 11, sizeof(test) - 11);
    M5_DUMPE(test + 19, sizeof(test) - 19);

    // Degenerate inputs
    M5_DUMPE(test, 0);
    M5_DUMPE(test, 1);
}
