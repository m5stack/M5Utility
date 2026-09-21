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

TEST(Conversion, Conversion)
{
    {
        SCOPED_TRACE("8bit:4");
        std::pair<uint8_t, int8_t> table[] = {
            // zero
            {0x00, 0},
            // only sign bit
            {0x08, -8},
            // all bits
            {0x0F, -1},
            // all signed bits
            {0x07, 7},
        };
        for (auto&& e : table) {
            EXPECT_EQ(m5::utility::unsigned_to_signed<4>(e.first), e.second) << e.first;
        }
    }

    {
        SCOPED_TRACE("8bit:8");
        std::pair<uint8_t, int8_t> table[] = {
            // zero
            {0, 0},
            // only sign bit
            {0x80, -128},
            // all bits
            {0xFF, -1},
            // all signed bits
            {0x7F, 127},
        };
        for (auto&& e : table) {
            EXPECT_EQ(m5::utility::unsigned_to_signed<8>(e.first), e.second) << e.first;
        }
    }

    {
        SCOPED_TRACE("16bit:10");
        std::pair<uint16_t, int16_t> table[] = {
            // zero
            {0x0000, 0},
            // only sign bit
            {0x0200, -512},
            // all bits
            {0x03FF, -1},
            // all signed bits
            {0x01FF, 511},
            //
            {0x0100, 256},
        };
        for (auto&& e : table) {
            EXPECT_EQ(m5::utility::unsigned_to_signed<10>(e.first), e.second) << e.first;
        }
    }

    {
        SCOPED_TRACE("32bit:20");
        std::pair<uint32_t, int32_t> table[] = {
            // zero
            {0x00000000, 0},
            // only sign bit
            {0x00080000, -524288},
            // all bits
            {0x000FFFFF, -1},
            // all signed bits
            {0x0007FFFF, 524287},
            //
            {0x00040000, 262144},
        };
        for (auto&& e : table) {
            EXPECT_EQ(m5::utility::unsigned_to_signed<20>(e.first), e.second) << e.first;
        }
    }

    {
        SCOPED_TRACE("64bit:47");
        std::pair<uint64_t, int64_t> table[] = {
            // zero
            {0x0000000000000000ULL, 0},
            // only sign bit
            {0x0000400000000000ULL, -70368744177664LL},
            // all bits
            {0x00007FFFFFFFFFFFULL, -1LL},
            // all signed bits
            {0x00003FFFFFFFFFFFULL, 70368744177663LL},
            //
            {0x0000200000000000ULL, 35184372088832},
        };
        for (auto&& e : table) {
            EXPECT_EQ(m5::utility::unsigned_to_signed<47>(e.first), e.second) << e.second;
        }
    }
}

TEST(Conversion, saturate_cast)
{
    using m5::utility::saturate_cast;

    // Narrowing: saturates instead of wrapping
    EXPECT_EQ(saturate_cast<uint16_t>(size_t{0x0000}), 0x0000U);
    EXPECT_EQ(saturate_cast<uint16_t>(size_t{0xFFFE}), 0xFFFEU);
    EXPECT_EQ(saturate_cast<uint16_t>(size_t{0xFFFF}), 0xFFFFU);
    EXPECT_EQ(saturate_cast<uint16_t>(size_t{0x10000}), 0xFFFFU);
    EXPECT_EQ(saturate_cast<uint16_t>(size_t{0x1FFFF}), 0xFFFFU);
    EXPECT_EQ(saturate_cast<uint8_t>(uint32_t{0x1FF}), 0xFFU);

    // Widening: the value is kept
    EXPECT_EQ(saturate_cast<uint32_t>(uint8_t{0xFF}), 0xFFU);
    EXPECT_EQ(saturate_cast<uint64_t>(uint16_t{0x1234}), 0x1234U);

    // Same width
    EXPECT_EQ(saturate_cast<uint16_t>(uint16_t{0xFFFF}), 0xFFFFU);

    // Usable at compile time
    constexpr uint16_t c = saturate_cast<uint16_t>(size_t{0x20000});
    EXPECT_EQ(c, 0xFFFFU);
}

TEST(Conversion, unsigned_to_signed_runtime_bits)
{
    using m5::utility::unsigned_to_signed;

    // The runtime-width overload must agree with the compile-time one
    EXPECT_EQ(unsigned_to_signed(uint32_t{0x00FFFFFF}, 24), unsigned_to_signed<24>(uint32_t{0x00FFFFFF}));
    EXPECT_EQ(unsigned_to_signed(uint32_t{0x00FFFFFF}, 24), -1);
    EXPECT_EQ(unsigned_to_signed(uint32_t{0x007FFFFF}, 24), 0x7FFFFF);
    EXPECT_EQ(unsigned_to_signed(uint32_t{0x00800000}, 24), -8388608);

    // Every width of a 16-bit value, against the compile-time overload
    EXPECT_EQ(unsigned_to_signed(uint16_t{0x0FFF}, 12), unsigned_to_signed<12>(uint16_t{0x0FFF}));
    EXPECT_EQ(unsigned_to_signed(uint16_t{0x0800}, 12), unsigned_to_signed<12>(uint16_t{0x0800}));
    EXPECT_EQ(unsigned_to_signed(uint16_t{0xFFFF}, 16), unsigned_to_signed<16>(uint16_t{0xFFFF}));

    // 1 bit: 0 or -1
    EXPECT_EQ(unsigned_to_signed(uint8_t{0x00}, 1), 0);
    EXPECT_EQ(unsigned_to_signed(uint8_t{0x01}, 1), -1);

    // The full width of the type, which must not shift by the width itself
    EXPECT_EQ(unsigned_to_signed(uint8_t{0xFF}, 8), -1);
    EXPECT_EQ(unsigned_to_signed(uint8_t{0x7F}, 8), 127);
    EXPECT_EQ(unsigned_to_signed(uint64_t{0xFFFFFFFFFFFFFFFFULL}, 64), -1);
    EXPECT_EQ(unsigned_to_signed(uint64_t{0x7FFFFFFFFFFFFFFFULL}, 64), 0x7FFFFFFFFFFFFFFFLL);

    // 0 bits yields 0
    EXPECT_EQ(unsigned_to_signed(uint32_t{0xFFFFFFFF}, 0), 0);

    // Bits above the requested width are ignored
    EXPECT_EQ(unsigned_to_signed(uint32_t{0xFFFFF001}, 12), 1);

    // Usable at compile time
    constexpr auto c = unsigned_to_signed(uint16_t{0x0FFF}, 12);
    EXPECT_EQ(c, -1);
}

TEST(Conversion, unsigned_to_signed_runtime_bits_sweep)
{
    using m5::utility::unsigned_to_signed;

    // Sweep every width of a 16-bit value and compare against a reference
    for (size_t bits = 1; bits <= 16; ++bits) {
        for (unsigned int i = 0; i <= 0xFFFF; i += 97) {
            const uint16_t v       = static_cast<uint16_t>(i);
            const uint32_t mask    = (1U << bits) - 1U;
            const uint32_t low     = v & mask;
            const int32_t expected = (low & (1U << (bits - 1)))
                                         ? static_cast<int32_t>(low) - static_cast<int32_t>(1U << bits)
                                         : static_cast<int32_t>(low);
            EXPECT_EQ(unsigned_to_signed(v, bits), static_cast<int16_t>(expected)) << "bits=" << bits << " v=" << i;
        }
    }
}
