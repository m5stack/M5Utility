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
#include <M5Unified.hpp>
#include <limits>

#if defined(__SIZEOF_INT128__)
#pragma message "Support I128"
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

TEST(Utility, Byteswap_int)
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

namespace {
template <class T>
std::array<uint8_t, sizeof(T)> to_bytes(const T& v)
{
    std::array<uint8_t, sizeof(T)> out{};
    std::memcpy(out.data(), &v, sizeof(T));
    return out;
}

template <class T>
T reverse_bytes_via_memcpy(const T& v)
{
    auto b = to_bytes(v);
    std::reverse(b.begin(), b.end());
    T out{};
    std::memcpy(&out, b.data(), sizeof(T));
    return out;
}

template <class T>
void ExpectBytesEqual(const T& a, const T& b)
{
    auto ba = to_bytes(a);
    auto bb = to_bytes(b);
    ASSERT_EQ(ba.size(), bb.size());
    EXPECT_EQ(0, std::memcmp(ba.data(), bb.data(), ba.size()));
}

template <class T>
void RunOneCase(const T& x)
{
    const T expect = reverse_bytes_via_memcpy(x);
    const T got    = m5::stl::byteswap(x);
    ExpectBytesEqual(got, expect);

    const T roundtrip = m5::stl::byteswap(got);
    ExpectBytesEqual(roundtrip, x);
}

template <typename T>
void FloatTest()
{
    const T pos_zero = T(+0.0);
    const T neg_zero = T(-0.0);
    const T one      = T(1.0);
    const T neg      = T(-2.5);

    RunOneCase(pos_zero);
    RunOneCase(neg_zero);
    RunOneCase(one);
    RunOneCase(neg);

    RunOneCase(std::numeric_limits<T>::min());
    RunOneCase(std::numeric_limits<T>::max());

    if (std::numeric_limits<T>::has_denorm != std::denorm_absent) {
        RunOneCase(std::numeric_limits<T>::denorm_min());
    }

    RunOneCase(std::numeric_limits<T>::infinity());
    RunOneCase(-std::numeric_limits<T>::infinity());

    RunOneCase(std::numeric_limits<T>::quiet_NaN());
}

}  // namespace

TEST(Utility, Byteswap_float)
{
    FloatTest<float>();
    FloatTest<double>();
    FloatTest<long double>();
}
