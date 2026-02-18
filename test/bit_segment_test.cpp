/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for bit_segment
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>

using namespace m5::utility;

TEST(BitSegment, Constructor)
{
    // Default constructor
    {
        BitSegment<4, uint8_t> seg;
        EXPECT_EQ(seg.raw(), 0);
        EXPECT_EQ(seg.upper(), 0U);
        EXPECT_EQ(seg.lower(), 0U);
    }

    // Value constructor
    {
        BitSegment<4, uint8_t> seg(0x35);  // upper=3, lower=5
        EXPECT_EQ(seg.raw(), 0x35);
        EXPECT_EQ(seg.upper(), 3U);
        EXPECT_EQ(seg.lower(), 5U);
    }

    // Copy constructor
    {
        BitSegment<4, uint8_t> original(0xAB);
        BitSegment<4, uint8_t> copied(original);
        EXPECT_EQ(copied.raw(), 0xAB);
        EXPECT_EQ(copied.upper(), 0xAU);
        EXPECT_EQ(copied.lower(), 0xBU);
    }
}

TEST(BitSegment, Assignment)
{
    // Copy assignment
    {
        BitSegment<4, uint8_t> original(0x12);
        BitSegment<4, uint8_t> assigned;
        assigned = original;
        EXPECT_EQ(assigned.raw(), 0x12);
    }

    // Value assignment
    {
        BitSegment<4, uint8_t> seg;
        seg = 0x34;
        EXPECT_EQ(seg.raw(), 0x34);
        EXPECT_EQ(seg.upper(), 3U);
        EXPECT_EQ(seg.lower(), 4U);
    }

    // Self assignment (via reference to avoid warning)
    {
        BitSegment<4, uint8_t> seg(0x56);
        BitSegment<4, uint8_t>& ref = seg;
        seg                         = ref;
        EXPECT_EQ(seg.raw(), 0x56);
    }
}

TEST(BitSegment, Cast)
{
    // Bool cast - zero is false
    {
        BitSegment<4, uint8_t> seg(0);
        EXPECT_FALSE(static_cast<bool>(seg));
    }

    // Bool cast - non-zero is true
    {
        BitSegment<4, uint8_t> seg(1);
        EXPECT_TRUE(static_cast<bool>(seg));

        BitSegment<4, uint8_t> seg2(0x80);
        EXPECT_TRUE(static_cast<bool>(seg2));
    }

    // Implicit conversion to base_type
    {
        BitSegment<4, uint8_t> seg(0x42);
        uint8_t value = seg;
        EXPECT_EQ(value, 0x42);
    }
}

TEST(BitSegment, GetterSetter_uint8)
{
    // 4 lower bits in uint8_t (4 upper bits)
    using BS = BitSegment<4, uint8_t>;

    // Getter
    {
        BS seg(0xA5);                  // binary: 1010 0101
        EXPECT_EQ(seg.upper(), 0xAU);  // upper 4 bits
        EXPECT_EQ(seg.lower(), 0x5U);  // lower 4 bits
        EXPECT_EQ(seg.raw(), 0xA5);
    }

    // Setter - upper
    {
        BS seg(0x00);
        seg.upper(0x0F);
        EXPECT_EQ(seg.upper(), 0x0FU);
        EXPECT_EQ(seg.lower(), 0x00U);
        EXPECT_EQ(seg.raw(), 0xF0);
    }

    // Setter - lower
    {
        BS seg(0x00);
        seg.lower(0x0F);
        EXPECT_EQ(seg.upper(), 0x00U);
        EXPECT_EQ(seg.lower(), 0x0FU);
        EXPECT_EQ(seg.raw(), 0x0F);
    }

    // Setter - both
    {
        BS seg(0x00);
        seg.upper(0x0C);
        seg.lower(0x03);
        EXPECT_EQ(seg.raw(), 0xC3);
    }

    // Setter - raw
    {
        BS seg(0x00);
        seg.raw(0xDE);
        EXPECT_EQ(seg.raw(), 0xDE);
        EXPECT_EQ(seg.upper(), 0x0DU);
        EXPECT_EQ(seg.lower(), 0x0EU);
    }

    // Setter - masking (values larger than segment)
    {
        BS seg(0x00);
        seg.upper(0xFF);  // Should be masked to 4 bits
        EXPECT_EQ(seg.upper(), 0x0FU);

        seg.lower(0xFF);  // Should be masked to 4 bits
        EXPECT_EQ(seg.lower(), 0x0FU);
    }
}

TEST(BitSegment, GetterSetter_uint16)
{
    // 8 lower bits in uint16_t (8 upper bits)
    using BS = BitSegment<8, uint16_t>;

    {
        BS seg(0x1234);
        EXPECT_EQ(seg.upper(), 0x12U);
        EXPECT_EQ(seg.lower(), 0x34U);
    }

    {
        BS seg(0x0000);
        seg.upper(0xAB);
        seg.lower(0xCD);
        EXPECT_EQ(seg.raw(), 0xABCD);
    }
}

TEST(BitSegment, GetterSetter_uint32)
{
    // 16 lower bits in uint32_t (16 upper bits)
    using BS = BitSegment<16, uint32_t>;

    {
        BS seg(0x12345678);
        EXPECT_EQ(seg.upper(), 0x1234U);
        EXPECT_EQ(seg.lower(), 0x5678U);
    }

    {
        BS seg(0x00000000);
        seg.upper(0xDEAD);
        seg.lower(0xBEEF);
        EXPECT_EQ(seg.raw(), 0xDEADBEEFU);
    }
}

TEST(BitSegment, AsymmetricBits)
{
    // 3 lower bits in uint8_t (5 upper bits)
    {
        using BS = BitSegment<3, uint8_t>;
        BS seg(0b11111111);                // 0xFF
        EXPECT_EQ(seg.lower(), 0b111U);    // 7
        EXPECT_EQ(seg.upper(), 0b11111U);  // 31
    }

    // 5 lower bits in uint8_t (3 upper bits)
    {
        using BS = BitSegment<5, uint8_t>;
        BS seg(0xFF);
        EXPECT_EQ(seg.lower(), 0b11111U);  // 31
        EXPECT_EQ(seg.upper(), 0b111U);    // 7
    }

    // 1 lower bit in uint8_t (7 upper bits)
    {
        using BS = BitSegment<1, uint8_t>;
        BS seg(0xFF);
        EXPECT_EQ(seg.lower(), 1U);
        EXPECT_EQ(seg.upper(), 0x7FU);  // 127
    }

    // 7 lower bits in uint8_t (1 upper bit)
    {
        using BS = BitSegment<7, uint8_t>;
        BS seg(0xFF);
        EXPECT_EQ(seg.lower(), 0x7FU);  // 127
        EXPECT_EQ(seg.upper(), 1U);
    }
}

TEST(BitSegment, SignedType)
{
    // For signed types, sign bit is excluded from upper segment
    // int8_t: 1 sign bit + 7 data bits
    // With LowerBits=4, upper has 3 bits (7-4=3)

    using BS = BitSegment<4, int8_t>;

    // Check constants
    EXPECT_TRUE(+BS::SIGNED);
    EXPECT_EQ(+BS::LOWER_BITS, 4U);
    EXPECT_EQ(+BS::UPPER_BITS, 3U);  // 8 - 4 - 1(sign) = 3

    // Positive value
    {
        BS seg(0x35);  // 0011 0101
        EXPECT_EQ(seg.lower(), 5U);
        EXPECT_EQ(seg.upper(), 3U);
    }

    // Set upper/lower
    {
        BS seg(0);
        seg.upper(0x07);  // max for 3 bits
        seg.lower(0x0F);  // max for 4 bits
        EXPECT_EQ(seg.upper(), 7U);
        EXPECT_EQ(seg.lower(), 15U);
        EXPECT_EQ(seg.raw(), 0x7F);  // 0111 1111
    }
}

TEST(BitSegment, CompareWithSameType)
{
    using BS = BitSegment<4, uint8_t>;

    BS a(0x12);
    BS b(0x12);
    BS c(0x34);

    // Equality
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);

    // Less than
    EXPECT_FALSE(a < b);
    EXPECT_TRUE(a < c);
    EXPECT_FALSE(c < a);

    // Greater than
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(a > c);
    EXPECT_TRUE(c > a);

    // Less than or equal
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a <= c);
    EXPECT_FALSE(c <= a);

    // Greater than or equal
    EXPECT_TRUE(a >= b);
    EXPECT_FALSE(a >= c);
    EXPECT_TRUE(c >= a);
}

TEST(BitSegment, CompareWithInt)
{
    using BS = BitSegment<4, uint8_t>;

    BS seg(0x42);

    // BitSegment vs int
    EXPECT_TRUE(seg == 0x42);
    EXPECT_FALSE(seg == 0x43);
    EXPECT_TRUE(seg != 0x43);
    EXPECT_FALSE(seg != 0x42);
    EXPECT_TRUE(seg < 0x50);
    EXPECT_FALSE(seg < 0x40);
    EXPECT_TRUE(seg > 0x40);
    EXPECT_FALSE(seg > 0x50);
    EXPECT_TRUE(seg <= 0x42);
    EXPECT_TRUE(seg <= 0x50);
    EXPECT_TRUE(seg >= 0x42);
    EXPECT_TRUE(seg >= 0x40);

    // int vs BitSegment
    EXPECT_TRUE(0x42 == seg);
    EXPECT_FALSE(0x43 == seg);
    EXPECT_TRUE(0x43 != seg);
    EXPECT_FALSE(0x42 != seg);
    EXPECT_TRUE(0x40 < seg);
    EXPECT_FALSE(0x50 < seg);
    EXPECT_TRUE(0x50 > seg);
    EXPECT_FALSE(0x40 > seg);
    EXPECT_TRUE(0x42 <= seg);
    EXPECT_TRUE(0x40 <= seg);
    EXPECT_TRUE(0x42 >= seg);
    EXPECT_TRUE(0x50 >= seg);
}

TEST(BitSegment, ConstexprSupport)
{
    // Verify constexpr support
    constexpr BitSegment<4, uint8_t> seg1;
    constexpr BitSegment<4, uint8_t> seg2(0x35);
    constexpr BitSegment<4, uint8_t> seg3(seg2);

    static_assert(seg1.raw() == 0, "Default should be 0");
    static_assert(seg2.raw() == 0x35, "Value should be 0x35");
    static_assert(seg2.upper() == 3, "Upper should be 3");
    static_assert(seg2.lower() == 5, "Lower should be 5");
    static_assert(seg3.raw() == seg2.raw(), "Copy should match");

    // Just to make gtest happy
    EXPECT_EQ(seg1.raw(), 0);
    EXPECT_EQ(seg2.raw(), 0x35);
}

TEST(BitSegment, BoundaryValues)
{
    using BS = BitSegment<4, uint8_t>;

    // All zeros
    {
        BS seg(0x00);
        EXPECT_EQ(seg.upper(), 0U);
        EXPECT_EQ(seg.lower(), 0U);
    }

    // All ones
    {
        BS seg(0xFF);
        EXPECT_EQ(seg.upper(), 0x0FU);
        EXPECT_EQ(seg.lower(), 0x0FU);
    }

    // Upper max, lower min
    {
        BS seg(0xF0);
        EXPECT_EQ(seg.upper(), 0x0FU);
        EXPECT_EQ(seg.lower(), 0x00U);
    }

    // Upper min, lower max
    {
        BS seg(0x0F);
        EXPECT_EQ(seg.upper(), 0x00U);
        EXPECT_EQ(seg.lower(), 0x0FU);
    }
}

TEST(BitSegment, PreserveOtherBits)
{
    using BS = BitSegment<4, uint8_t>;

    // Setting upper should preserve lower
    {
        BS seg(0x35);
        seg.upper(0x0A);
        EXPECT_EQ(seg.upper(), 0x0AU);
        EXPECT_EQ(seg.lower(), 0x05U);  // Preserved
        EXPECT_EQ(seg.raw(), 0xA5);
    }

    // Setting lower should preserve upper
    {
        BS seg(0x35);
        seg.lower(0x0B);
        EXPECT_EQ(seg.upper(), 0x03U);  // Preserved
        EXPECT_EQ(seg.lower(), 0x0BU);
        EXPECT_EQ(seg.raw(), 0x3B);
    }
}
