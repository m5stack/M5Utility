/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for stl/extension
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>
#include <vector>
#include <array>
#include <string>

// Test enums for to_underlying
enum class Color : uint8_t { Red = 0, Green = 1, Blue = 2 };

enum class SignedEnum : int8_t { Negative = -1, Zero = 0, Positive = 1 };

enum class LargeEnum : uint32_t { Small = 1, Large = 0xDEADBEEF };

enum UnscopedEnum { A = 10, B = 20, C = 30 };

TEST(StlExtension, SizeContainer_Vector)
{
    // Empty vector
    {
        std::vector<int> v;
        EXPECT_EQ(m5::stl::size(v), 0U);
    }

    // Non-empty vector
    {
        std::vector<int> v = {1, 2, 3, 4, 5};
        EXPECT_EQ(m5::stl::size(v), 5U);
    }

    // Vector of different type
    {
        std::vector<std::string> v = {"hello", "world"};
        EXPECT_EQ(m5::stl::size(v), 2U);
    }
}

TEST(StlExtension, SizeContainer_StdArray)
{
    {
        std::array<int, 3> arr = {1, 2, 3};
        EXPECT_EQ(m5::stl::size(arr), 3U);
    }

    {
        std::array<double, 10> arr = {};
        EXPECT_EQ(m5::stl::size(arr), 10U);
    }
}

TEST(StlExtension, SizeContainer_String)
{
    {
        std::string s;
        EXPECT_EQ(m5::stl::size(s), 0U);
    }

    {
        std::string s = "hello";
        EXPECT_EQ(m5::stl::size(s), 5U);
    }
}

TEST(StlExtension, SizeRawArray)
{
    // int array
    {
        int arr[5] = {1, 2, 3, 4, 5};
        EXPECT_EQ(m5::stl::size(arr), 5U);
    }

    // char array
    {
        char arr[10] = {};
        EXPECT_EQ(m5::stl::size(arr), 10U);
    }

    // double array
    {
        double arr[3] = {1.0, 2.0, 3.0};
        EXPECT_EQ(m5::stl::size(arr), 3U);
    }

    // Single element
    {
        int arr[1] = {42};
        EXPECT_EQ(m5::stl::size(arr), 1U);
    }

    // Large array
    {
        uint8_t arr[256] = {};
        EXPECT_EQ(m5::stl::size(arr), 256U);
    }
}

TEST(StlExtension, SizeConstexpr)
{
    // Verify constexpr support for raw arrays
    constexpr int arr[7] = {};
    constexpr size_t s   = m5::stl::size(arr);
    static_assert(s == 7, "Size should be 7");

    EXPECT_EQ(s, 7U);
}

TEST(StlExtension, ToUnderlying_ScopedEnum)
{
    // uint8_t underlying type
    {
        EXPECT_EQ(m5::stl::to_underlying(Color::Red), 0);
        EXPECT_EQ(m5::stl::to_underlying(Color::Green), 1);
        EXPECT_EQ(m5::stl::to_underlying(Color::Blue), 2);

        // Verify return type
        auto val = m5::stl::to_underlying(Color::Red);
        static_assert(std::is_same<decltype(val), uint8_t>::value, "Should be uint8_t");
    }

    // int8_t underlying type (signed)
    {
        EXPECT_EQ(m5::stl::to_underlying(SignedEnum::Negative), -1);
        EXPECT_EQ(m5::stl::to_underlying(SignedEnum::Zero), 0);
        EXPECT_EQ(m5::stl::to_underlying(SignedEnum::Positive), 1);

        auto val = m5::stl::to_underlying(SignedEnum::Negative);
        static_assert(std::is_same<decltype(val), int8_t>::value, "Should be int8_t");
    }

    // uint32_t underlying type
    {
        EXPECT_EQ(m5::stl::to_underlying(LargeEnum::Small), 1U);
        EXPECT_EQ(m5::stl::to_underlying(LargeEnum::Large), 0xDEADBEEFU);

        auto val = m5::stl::to_underlying(LargeEnum::Small);
        static_assert(std::is_same<decltype(val), uint32_t>::value, "Should be uint32_t");
    }
}

TEST(StlExtension, ToUnderlying_UnscopedEnum)
{
    // Unscoped enum (default underlying type is unsigned int or int depending on compiler)
    auto val_a = m5::stl::to_underlying(A);
    auto val_b = m5::stl::to_underlying(B);
    auto val_c = m5::stl::to_underlying(C);

    EXPECT_EQ(val_a, decltype(val_a)(10));
    EXPECT_EQ(val_b, decltype(val_b)(20));
    EXPECT_EQ(val_c, decltype(val_c)(30));
}

TEST(StlExtension, ToUnderlying_Constexpr)
{
    // Verify constexpr support
    constexpr auto val = m5::stl::to_underlying(Color::Blue);
    static_assert(val == 2, "Should be 2");

    EXPECT_EQ(val, 2);
}

TEST(StlExtension, ToUnderlying_InExpression)
{
    // Use in arithmetic expressions
    {
        auto sum = m5::stl::to_underlying(Color::Red) + m5::stl::to_underlying(Color::Blue);
        EXPECT_EQ(sum, 2);
    }

    // Use in comparison
    {
        EXPECT_TRUE(m5::stl::to_underlying(Color::Red) < m5::stl::to_underlying(Color::Blue));
    }

    // Use in switch (compile test)
    {
        Color c   = Color::Green;
        int value = 0;
        switch (m5::stl::to_underlying(c)) {
            case 0:
                value = 100;
                break;
            case 1:
                value = 200;
                break;
            case 2:
                value = 300;
                break;
        }
        EXPECT_EQ(value, 200);
    }
}
