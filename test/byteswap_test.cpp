/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for M5Utility
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>
#include <limits>
#include <cstring>

#if defined(__SIZEOF_INT128__)
#pragma message("Support I128")
namespace {

using u128_t = unsigned __int128;
using i128_t = __int128;

constexpr u128_t make_u128(uint64_t hi, uint64_t lo)
{
    return ((u128_t)hi << 64) | (u128_t)lo;
}

constexpr i128_t make_i128(int64_t hi, uint64_t lo)
{
    return (i128_t)(((u128_t)((uint64_t)hi) << 64) | (u128_t)lo);
}

}  // namespace

#endif

TEST(Byteswap, Fixed)
{
    constexpr uint8_t u8{127};
    constexpr int8_t i8{-128};
    constexpr uint16_t u16{0x1234};
    constexpr int16_t i16{-0x1234};  // 0xEDCC
    constexpr uint32_t u32{0x12345678};
    constexpr int32_t i32{-0x12345678};  // 0xEDCBA988
    constexpr uint64_t u64{0x123456789ABCDEF0};
    constexpr int64_t i64{-0x123456789ABCDEF0};  // 0xEDCBA98765432110

    {
        constexpr auto a = m5::stl::byteswap(u8);
        EXPECT_EQ(a, u8);
        constexpr auto b = m5::stl::byteswap(i8);
        EXPECT_EQ(b, i8);

        EXPECT_EQ(m5::stl::byteswap(a), u8);
        EXPECT_EQ(m5::stl::byteswap(b), i8);
    }

    {
        constexpr auto a = m5::stl::byteswap(u16);
        EXPECT_EQ(a, 0x3412);
        constexpr auto b = m5::stl::byteswap(i16);
        EXPECT_EQ(b, (int16_t)0xCCED);

        EXPECT_EQ(m5::stl::byteswap(a), u16);
        EXPECT_EQ(m5::stl::byteswap(b), i16);
    }

    {
        constexpr auto a = m5::stl::byteswap(u32);
        EXPECT_EQ(a, 0x78563412);
        constexpr auto b = m5::stl::byteswap(i32);
        EXPECT_EQ(b, (int32_t)0x88A9CBED);

        EXPECT_EQ(m5::stl::byteswap(a), u32);
        EXPECT_EQ(m5::stl::byteswap(b), i32);
    }

    {
        constexpr auto a = m5::stl::byteswap(u64);
        EXPECT_EQ(a, 0xF0DEBC9A78563412);
        constexpr auto b = m5::stl::byteswap(i64);
        EXPECT_EQ(b, (int64_t)0x1021436587A9CBED);

        EXPECT_EQ(m5::stl::byteswap(a), u64);
        EXPECT_EQ(m5::stl::byteswap(b), i64);
    }

#if defined(__SIZEOF_INT128__)
    constexpr u128_t u128 = make_u128(u64, u64);
    constexpr i128_t i128 = make_i128(i64, (uint64_t)i64);

    {
        constexpr u128_t expect_u = make_u128(0xF0DEBC9A78563412ULL, 0xF0DEBC9A78563412ULL);
        constexpr i128_t expect_i = (i128_t)make_u128(0x1021436587A9CBEDULL, 0x1021436587A9CBEDULL);

        constexpr auto a = m5::stl::byteswap(u128);
        EXPECT_EQ(a, expect_u);
        constexpr auto b = m5::stl::byteswap(i128);
        EXPECT_EQ(b, expect_i);

        EXPECT_EQ(m5::stl::byteswap(a), u128);
        EXPECT_EQ(m5::stl::byteswap(b), i128);
    }
#endif
}

// Verify portable and default (builtin on GCC/Clang) implementations produce identical results
TEST(Byteswap, Portable)
{
    constexpr uint8_t u8{127};
    constexpr int8_t i8{-128};
    constexpr uint16_t u16{0x1234};
    constexpr int16_t i16{-0x1234};
    constexpr uint32_t u32{0x12345678};
    constexpr int32_t i32{-0x12345678};
    constexpr uint64_t u64{0x123456789ABCDEF0};
    constexpr int64_t i64{-0x123456789ABCDEF0};

    EXPECT_EQ(m5::stl::byteswap_with_portable(u8), m5::stl::byteswap(u8));
    EXPECT_EQ(m5::stl::byteswap_with_portable(i8), m5::stl::byteswap(i8));
    EXPECT_EQ(m5::stl::byteswap_with_portable(u16), m5::stl::byteswap(u16));
    EXPECT_EQ(m5::stl::byteswap_with_portable(i16), m5::stl::byteswap(i16));
    EXPECT_EQ(m5::stl::byteswap_with_portable(u32), m5::stl::byteswap(u32));
    EXPECT_EQ(m5::stl::byteswap_with_portable(i32), m5::stl::byteswap(i32));
    EXPECT_EQ(m5::stl::byteswap_with_portable(u64), m5::stl::byteswap(u64));
    EXPECT_EQ(m5::stl::byteswap_with_portable(i64), m5::stl::byteswap(i64));

    // Also verify round-trip with portable
    EXPECT_EQ(m5::stl::byteswap_with_portable(m5::stl::byteswap_with_portable(u16)), u16);
    EXPECT_EQ(m5::stl::byteswap_with_portable(m5::stl::byteswap_with_portable(u32)), u32);
    EXPECT_EQ(m5::stl::byteswap_with_portable(m5::stl::byteswap_with_portable(u64)), u64);

#if defined(__SIZEOF_INT128__)
    constexpr u128_t u128 = make_u128(u64, u64);
    constexpr i128_t i128 = make_i128(i64, (uint64_t)i64);

    EXPECT_EQ(m5::stl::byteswap_with_portable(u128), m5::stl::byteswap(u128));
    EXPECT_EQ(m5::stl::byteswap_with_portable(i128), m5::stl::byteswap(i128));
    EXPECT_EQ(m5::stl::byteswap_with_portable(m5::stl::byteswap_with_portable(u128)), u128);
#endif
}

TEST(Byteswap, ValueIntegral)
{
    using m5::stl::byteswap;
    using m5::stl::byteswap_value;

    // Integral and enum are handed to byteswap()
    EXPECT_EQ(byteswap_value(uint16_t{0x1234}), byteswap(uint16_t{0x1234}));
    EXPECT_EQ(byteswap_value(uint32_t{0x12345678}), byteswap(uint32_t{0x12345678}));
    EXPECT_EQ(byteswap_value(int16_t{0x1234}), byteswap(int16_t{0x1234}));

    enum class E : uint16_t { Value = 0x1234 };
    EXPECT_EQ(byteswap_value(E::Value), byteswap(E::Value));
}

TEST(Byteswap, ValueFloat)
{
    using m5::stl::byteswap_value;

    // Reversing twice must return the original value
    const float f = 1.25f;
    EXPECT_FLOAT_EQ(byteswap_value(byteswap_value(f)), f);

    const double d = -3.5;
    EXPECT_DOUBLE_EQ(byteswap_value(byteswap_value(d)), d);

    // The bytes really are reversed: 1.0f is 0x3F800000, so the swap is 0x0000803F
    const float one     = 1.0f;
    const float swapped = byteswap_value(one);
    uint32_t bits{};
    std::memcpy(&bits, &swapped, sizeof(bits));
    EXPECT_EQ(bits, 0x0000803FU);

    // Special values are reversed as raw bit patterns
    const float zero = 0.0f;
    EXPECT_FLOAT_EQ(byteswap_value(byteswap_value(zero)), zero);
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_DOUBLE_EQ(byteswap_value(byteswap_value(inf)), inf);
}
