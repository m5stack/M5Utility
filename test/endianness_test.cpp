/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for stl/endianness
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>
#include <cstring>

TEST(Endianness, ExactlyOneEndianType)
{
    // Verify exactly one of little, big, or other is true
    int count = 0;
    if (m5::endian::little) ++count;
    if (m5::endian::big) ++count;
    if (m5::endian::other) ++count;

    EXPECT_EQ(count, 1) << "Exactly one endian type should be true";
}

TEST(Endianness, MutuallyExclusive)
{
    // If little, then not big and not other
    if (m5::endian::little) {
        EXPECT_FALSE(m5::endian::big);
        EXPECT_FALSE(m5::endian::other);
    }

    // If big, then not little and not other
    if (m5::endian::big) {
        EXPECT_FALSE(m5::endian::little);
        EXPECT_FALSE(m5::endian::other);
    }

    // If other, then not little and not big
    if (m5::endian::other) {
        EXPECT_FALSE(m5::endian::little);
        EXPECT_FALSE(m5::endian::big);
    }
}

TEST(Endianness, ConstexprValues)
{
    // Verify constexpr support
    constexpr bool is_little = m5::endian::little;
    constexpr bool is_big    = m5::endian::big;
    constexpr bool is_other  = m5::endian::other;

    // Static assertions (compile-time checks)
    static_assert(m5::endian::little || m5::endian::big || m5::endian::other,
                  "At least one endian type must be true");
    static_assert((int)m5::endian::little + (int)m5::endian::big + (int)m5::endian::other == 1,
                  "Exactly one endian type must be true");

    // Runtime check (just to use the variables)
    EXPECT_TRUE(is_little || is_big || is_other);
}

TEST(Endianness, EnumClass)
{
    // Verify enum class exists and has expected members
    [[maybe_unused]] m5::stl::endian e_little = m5::stl::endian::little;
    [[maybe_unused]] m5::stl::endian e_big    = m5::stl::endian::big;
    [[maybe_unused]] m5::stl::endian e_native = m5::stl::endian::native;

    // Native should match either little or big (or neither for exotic platforms)
    bool native_is_little = (e_native == e_little);
    bool native_is_big    = (e_native == e_big);

    // Consistency with m5::endian namespace
    EXPECT_EQ(native_is_little, m5::endian::little);
    EXPECT_EQ(native_is_big, m5::endian::big);
}

TEST(Endianness, RuntimeVerification)
{
    // Runtime verification of endianness using actual memory layout
    uint32_t value = 0x01020304;
    uint8_t bytes[4];
    std::memcpy(bytes, &value, sizeof(value));

    bool runtime_little = (bytes[0] == 0x04);  // LSB first
    bool runtime_big    = (bytes[0] == 0x01);  // MSB first

    // Compare with compile-time detection
    if (m5::endian::little) {
        EXPECT_TRUE(runtime_little) << "Compile-time says little endian, but runtime disagrees";
        EXPECT_FALSE(runtime_big);
    } else if (m5::endian::big) {
        EXPECT_TRUE(runtime_big) << "Compile-time says big endian, but runtime disagrees";
        EXPECT_FALSE(runtime_little);
    }
    // For 'other' endianness, we can't make assumptions about byte order
}

TEST(Endianness, ByteOrderConsistency)
{
    // Test with 16-bit value
    uint16_t val16 = 0x0102;
    uint8_t bytes16[2];
    std::memcpy(bytes16, &val16, sizeof(val16));

    if (m5::endian::little) {
        EXPECT_EQ(bytes16[0], 0x02);  // LSB
        EXPECT_EQ(bytes16[1], 0x01);  // MSB
    } else if (m5::endian::big) {
        EXPECT_EQ(bytes16[0], 0x01);  // MSB
        EXPECT_EQ(bytes16[1], 0x02);  // LSB
    }

    // Test with 64-bit value
    uint64_t val64 = 0x0102030405060708ULL;
    uint8_t bytes64[8];
    std::memcpy(bytes64, &val64, sizeof(val64));

    if (m5::endian::little) {
        EXPECT_EQ(bytes64[0], 0x08);  // LSB
        EXPECT_EQ(bytes64[7], 0x01);  // MSB
    } else if (m5::endian::big) {
        EXPECT_EQ(bytes64[0], 0x01);  // MSB
        EXPECT_EQ(bytes64[7], 0x08);  // LSB
    }
}
