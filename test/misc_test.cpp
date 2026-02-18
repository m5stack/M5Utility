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
#include <type_traits>

using namespace m5::utility;

TEST(Utility, reverseBitOrder)
{
    EXPECT_EQ(reverseBitOrder((uint8_t)0), 0);
    EXPECT_EQ(reverseBitOrder((uint16_t)0), 0);

    EXPECT_EQ(reverseBitOrder((uint8_t)0xFF), 0xFF);
    EXPECT_EQ(reverseBitOrder((uint16_t)0xFFFF), 0xFFFF);

    EXPECT_EQ(reverseBitOrder((uint8_t)0x0F), 0xF0);
    EXPECT_EQ(reverseBitOrder((uint16_t)0xFF00), 0x00FF);

    EXPECT_EQ(reverseBitOrder((uint8_t)0x4C), 0x32);
    EXPECT_EQ(reverseBitOrder((uint16_t)0x4C4C), 0x3232);

    EXPECT_EQ(reverseBitOrder((uint8_t)0x8E), 0x71);
    EXPECT_EQ(reverseBitOrder((uint8_t)0x65), 0xA6);
    EXPECT_EQ(reverseBitOrder((uint16_t)0x8E65), 0xA671);

    // Single bit patterns
    EXPECT_EQ(reverseBitOrder((uint8_t)0x01), 0x80);
    EXPECT_EQ(reverseBitOrder((uint8_t)0x80), 0x01);
    EXPECT_EQ(reverseBitOrder((uint16_t)0x0001), 0x8000);
    EXPECT_EQ(reverseBitOrder((uint16_t)0x8000), 0x0001);

    // Alternating patterns
    EXPECT_EQ(reverseBitOrder((uint8_t)0xAA), 0x55);
    EXPECT_EQ(reverseBitOrder((uint8_t)0x55), 0xAA);
    EXPECT_EQ(reverseBitOrder((uint16_t)0xAAAA), 0x5555);
    EXPECT_EQ(reverseBitOrder((uint16_t)0x5555), 0xAAAA);
}

TEST(Utility, isValidI2CAddress)
{
    // 7-bit address: Reserved addresses (0x00-0x07)
    EXPECT_FALSE(isValidI2CAddress(0x00));
    EXPECT_FALSE(isValidI2CAddress(0x01));
    EXPECT_FALSE(isValidI2CAddress(0x07));

    // 7-bit address: Valid range (0x08-0x77)
    EXPECT_TRUE(isValidI2CAddress(0x08));
    EXPECT_TRUE(isValidI2CAddress(0x10));
    EXPECT_TRUE(isValidI2CAddress(0x50));
    EXPECT_TRUE(isValidI2CAddress(0x77));

    // 7-bit address: Reserved addresses (0x78-0x7F)
    EXPECT_FALSE(isValidI2CAddress(0x78));
    EXPECT_FALSE(isValidI2CAddress(0x7F));

    // 10-bit address: Valid range (0x80-0x3FF)
    EXPECT_TRUE(isValidI2CAddress(0x80));
    EXPECT_TRUE(isValidI2CAddress(0x100));
    EXPECT_TRUE(isValidI2CAddress(0x200));
    EXPECT_TRUE(isValidI2CAddress(0x3FF));

    // 10-bit address: Out of range
    EXPECT_FALSE(isValidI2CAddress(0x400));
    EXPECT_FALSE(isValidI2CAddress(0xFFFF));
}

TEST(Utility, uint_least_for_bits)
{
    // 1-8 bits -> uint8_t
    static_assert(std::is_same<uint_least_for_bits<1>::type, uint8_t>::value, "1 bit should be uint8_t");
    static_assert(std::is_same<uint_least_for_bits<7>::type, uint8_t>::value, "7 bits should be uint8_t");
    static_assert(std::is_same<uint_least_for_bits<8>::type, uint8_t>::value, "8 bits should be uint8_t");

    // 9-16 bits -> uint16_t
    static_assert(std::is_same<uint_least_for_bits<9>::type, uint16_t>::value, "9 bits should be uint16_t");
    static_assert(std::is_same<uint_least_for_bits<12>::type, uint16_t>::value, "12 bits should be uint16_t");
    static_assert(std::is_same<uint_least_for_bits<16>::type, uint16_t>::value, "16 bits should be uint16_t");

    // 17-32 bits -> uint32_t
    static_assert(std::is_same<uint_least_for_bits<17>::type, uint32_t>::value, "17 bits should be uint32_t");
    static_assert(std::is_same<uint_least_for_bits<24>::type, uint32_t>::value, "24 bits should be uint32_t");
    static_assert(std::is_same<uint_least_for_bits<32>::type, uint32_t>::value, "32 bits should be uint32_t");

    // 33-64 bits -> uint64_t
    static_assert(std::is_same<uint_least_for_bits<33>::type, uint64_t>::value, "33 bits should be uint64_t");
    static_assert(std::is_same<uint_least_for_bits<48>::type, uint64_t>::value, "48 bits should be uint64_t");
    static_assert(std::is_same<uint_least_for_bits<64>::type, uint64_t>::value, "64 bits should be uint64_t");

    // Runtime check (dummy test to ensure compilation)
    EXPECT_EQ(sizeof(uint_least_for_bits<1>::type), 1U);
    EXPECT_EQ(sizeof(uint_least_for_bits<8>::type), 1U);
    EXPECT_EQ(sizeof(uint_least_for_bits<9>::type), 2U);
    EXPECT_EQ(sizeof(uint_least_for_bits<16>::type), 2U);
    EXPECT_EQ(sizeof(uint_least_for_bits<17>::type), 4U);
    EXPECT_EQ(sizeof(uint_least_for_bits<32>::type), 4U);
    EXPECT_EQ(sizeof(uint_least_for_bits<33>::type), 8U);
    EXPECT_EQ(sizeof(uint_least_for_bits<64>::type), 8U);
}
