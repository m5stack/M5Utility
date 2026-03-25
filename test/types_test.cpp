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

using namespace m5::types;

// ============================================================
// U16 tests
// ============================================================
TEST(Utility, Types_U16)
{
    // Constructor
    constexpr big_uint16_t bg0;
    constexpr little_uint16_t lt0;
    EXPECT_EQ(bg0.value, 0U);
    EXPECT_EQ(lt0.value, 0U);

    big_uint16_t bg1{0x1234};
    little_uint16_t lt1{0x1234};
    EXPECT_EQ(bg1.u8[0], 0x12);
    EXPECT_EQ(bg1.u8[1], 0x34);
    EXPECT_EQ(lt1.u8[0], 0x34);
    EXPECT_EQ(lt1.u8[1], 0x12);

    constexpr big_uint16_t bg2{0x12, 0x34};
    constexpr little_uint16_t lt2{0x12, 0x34};
    EXPECT_EQ(bg2.u8[0], 0x12);
    EXPECT_EQ(bg2.u8[1], 0x34);
    EXPECT_EQ(lt2.u8[0], 0x12);
    EXPECT_EQ(lt2.u8[1], 0x34);

    big_uint16_t bg3    = bg1;
    little_uint16_t lt3 = lt1;
    EXPECT_EQ(bg3.u8[0], 0x12);
    EXPECT_EQ(bg3.u8[1], 0x34);
    EXPECT_EQ(lt3.u8[0], 0x34);
    EXPECT_EQ(lt3.u8[1], 0x12);

    big_uint16_t bg4    = std::move(bg3);
    little_uint16_t lt4 = std::move(lt3);
    EXPECT_EQ(bg4.u8[0], 0x12);
    EXPECT_EQ(bg4.u8[1], 0x34);
    EXPECT_EQ(lt4.u8[0], 0x34);
    EXPECT_EQ(lt4.u8[1], 0x12);

    // Assignment
    big_uint16_t bg5;
    little_uint16_t lt5;
    bg5 = bg4;
    lt5 = lt4;
    EXPECT_EQ(bg5.u8[0], 0x12);
    EXPECT_EQ(bg5.u8[1], 0x34);
    EXPECT_EQ(lt5.u8[0], 0x34);
    EXPECT_EQ(lt5.u8[1], 0x12);

    big_uint16_t bg60;
    big_uint16_t bg61;
    little_uint16_t lt60;
    little_uint16_t lt61;
    if (m5::endian::little) {
        bg60.operator= <false>(0x3412);  // big to big
        bg61.operator= <true>(0x1234);   // little to big
        lt60.operator= <false>(0x3412);  // big to little
        lt61.operator= <true>(0x1234);   // little to little
    } else {
        bg60.operator= <false>(0x1234);  // big to big
        bg61.operator= <true>(0x3412);   // little to big
        lt60.operator= <false>(0x1234);  // big to little
        lt61.operator= <true>(0x3412);   // little to little
    }
    EXPECT_EQ(bg60.u8[0], 0x12);
    EXPECT_EQ(bg60.u8[1], 0x34);
    EXPECT_EQ(bg61.u8[0], 0x12);
    EXPECT_EQ(bg61.u8[1], 0x34);
    EXPECT_EQ(lt60.u8[0], 0x34);
    EXPECT_EQ(lt60.u8[1], 0x12);
    EXPECT_EQ(lt61.u8[0], 0x34);
    EXPECT_EQ(lt61.u8[1], 0x12);

    big_uint16_t bg7;
    little_uint16_t lt7;
    bg7 = std::make_pair<int, int>(0x12, 0x34);
    lt7 = std::make_pair<int, int>(0x12, 0x34);
    EXPECT_EQ(bg7.u8[0], 0x12);
    EXPECT_EQ(bg7.u8[1], 0x34);
    EXPECT_EQ(lt7.u8[0], 0x12);
    EXPECT_EQ(lt7.u8[1], 0x34);

    // Compile error (static_assert)
    // bg7 = std::make_pair<float, double>( 1.2f, 3.4 );

    // Cast
    EXPECT_FALSE((bool)bg0);
    EXPECT_FALSE((bool)lt0);
    EXPECT_TRUE((bool)bg1);
    EXPECT_TRUE((bool)lt1);

    EXPECT_EQ((uint16_t)bg0, 00U);
    EXPECT_EQ((uint16_t)lt0, 00U);
    EXPECT_EQ((uint16_t)bg1, 0x1234);
    EXPECT_EQ((uint16_t)lt1, 0x1234);

    EXPECT_EQ(*(const uint8_t*)bg0, 0U);
    EXPECT_EQ(*(const uint8_t*)lt0, 0U);
    EXPECT_EQ(*(const uint8_t*)bg1, 0x12U);
    EXPECT_EQ(*(const uint8_t*)lt1, 0x34U);

    // set() is using in operator=
    // get() is using in cast to uint16
    // data() is using in cast to const uint8_t*

    // Non-const uint8_t* cast (mutable access)
    {
        big_uint16_t bg_mut{0x12, 0x34};
        uint8_t* p = static_cast<uint8_t*>(bg_mut);
        EXPECT_EQ(p[0], 0x12);
        p[0] = 0xAA;
        EXPECT_EQ(bg_mut.u8[0], 0xAA);
    }
}

TEST(Utility, Types_U16_Accessors)
{
    // high() and low()
    {
        big_uint16_t bg{0x12, 0x34};
        little_uint16_t lt{0x12, 0x34};

        EXPECT_EQ(bg.high(), 0x12);
        EXPECT_EQ(bg.low(), 0x34);
        EXPECT_EQ(lt.high(), 0x12);
        EXPECT_EQ(lt.low(), 0x34);
    }

    // high() and low() with value constructor
    {
        big_uint16_t bg{0xABCD};
        little_uint16_t lt{0xABCD};

        EXPECT_EQ(bg.high(), 0xAB);
        EXPECT_EQ(bg.low(), 0xCD);
        // little endian stores low byte first
        EXPECT_EQ(lt.high(), 0xCD);
        EXPECT_EQ(lt.low(), 0xAB);
    }

    // size()
    {
        big_uint16_t bg;
        little_uint16_t lt;

        EXPECT_EQ(bg.size(), 2U);
        EXPECT_EQ(lt.size(), 2U);
    }

    // data() const
    {
        const big_uint16_t bg{0x12, 0x34};
        const little_uint16_t lt{0x56, 0x78};

        const uint8_t* bg_ptr = bg.data();
        const uint8_t* lt_ptr = lt.data();

        EXPECT_EQ(bg_ptr[0], 0x12);
        EXPECT_EQ(bg_ptr[1], 0x34);
        EXPECT_EQ(lt_ptr[0], 0x56);
        EXPECT_EQ(lt_ptr[1], 0x78);
    }

    // data() non-const (modifiable)
    {
        big_uint16_t bg{0x00, 0x00};
        little_uint16_t lt{0x00, 0x00};

        uint8_t* bg_ptr = bg.data();
        uint8_t* lt_ptr = lt.data();

        bg_ptr[0] = 0xAA;
        bg_ptr[1] = 0xBB;
        lt_ptr[0] = 0xCC;
        lt_ptr[1] = 0xDD;

        EXPECT_EQ(bg.u8[0], 0xAA);
        EXPECT_EQ(bg.u8[1], 0xBB);
        EXPECT_EQ(lt.u8[0], 0xCC);
        EXPECT_EQ(lt.u8[1], 0xDD);
    }
}

TEST(Utility, Types_U16_SetGet)
{
    // set<> and get<> with explicit endianness
    {
        big_uint16_t bg;
        little_uint16_t lt;

        // Set with little endian value
        bg.set<true>(0x1234);
        lt.set<true>(0x1234);

        EXPECT_EQ(bg.get<true>(), 0x1234U);
        EXPECT_EQ(lt.get<true>(), 0x1234U);
    }

    {
        big_uint16_t bg;
        little_uint16_t lt;

        // Set with big endian value
        bg.set<false>(0x1234);
        lt.set<false>(0x1234);

        EXPECT_EQ(bg.get<false>(), 0x1234U);
        EXPECT_EQ(lt.get<false>(), 0x1234U);
    }
}

TEST(Utility, Types_U16_BoundaryValues)
{
    // Zero
    {
        big_uint16_t bg{0x0000};
        little_uint16_t lt{0x0000};

        EXPECT_EQ((uint16_t)bg, 0x0000U);
        EXPECT_EQ((uint16_t)lt, 0x0000U);
        EXPECT_EQ(bg.u8[0], 0x00);
        EXPECT_EQ(bg.u8[1], 0x00);
        EXPECT_EQ(lt.u8[0], 0x00);
        EXPECT_EQ(lt.u8[1], 0x00);
    }

    // Max value
    {
        big_uint16_t bg{0xFFFF};
        little_uint16_t lt{0xFFFF};

        EXPECT_EQ((uint16_t)bg, 0xFFFFU);
        EXPECT_EQ((uint16_t)lt, 0xFFFFU);
        EXPECT_EQ(bg.u8[0], 0xFF);
        EXPECT_EQ(bg.u8[1], 0xFF);
        EXPECT_EQ(lt.u8[0], 0xFF);
        EXPECT_EQ(lt.u8[1], 0xFF);
    }

    // Single byte values
    {
        big_uint16_t bg_low{0x00FF};
        big_uint16_t bg_high{0xFF00};
        little_uint16_t lt_low{0x00FF};
        little_uint16_t lt_high{0xFF00};

        EXPECT_EQ(bg_low.u8[0], 0x00);
        EXPECT_EQ(bg_low.u8[1], 0xFF);
        EXPECT_EQ(bg_high.u8[0], 0xFF);
        EXPECT_EQ(bg_high.u8[1], 0x00);

        EXPECT_EQ(lt_low.u8[0], 0xFF);
        EXPECT_EQ(lt_low.u8[1], 0x00);
        EXPECT_EQ(lt_high.u8[0], 0x00);
        EXPECT_EQ(lt_high.u8[1], 0xFF);
    }
}

TEST(Utility, Types_U16_Compare)
{
    // Same value across endianness
    big_uint16_t bg0{0x1234};
    little_uint16_t lt0{0x1234};

    EXPECT_EQ(bg0, lt0);
    EXPECT_GE(bg0, lt0);
    EXPECT_LE(bg0, lt0);
    EXPECT_FALSE(bg0 < lt0);
    EXPECT_FALSE(bg0 > lt0);

    // Differ in low byte only
    {
        big_uint16_t bg1{0x1235};
        little_uint16_t lt1{0x1235};

        EXPECT_NE(bg0, bg1);
        EXPECT_NE(bg0, lt1);
        EXPECT_LT(bg0, bg1);
        EXPECT_LT(bg0, lt1);
        EXPECT_LE(bg0, bg1);
        EXPECT_LE(bg0, lt1);
        EXPECT_GE(bg1, bg0);
        EXPECT_GE(lt1, bg0);
        EXPECT_GT(bg1, bg0);
        EXPECT_GT(lt1, bg0);

        EXPECT_NE(lt0, bg1);
        EXPECT_NE(lt0, lt1);
        EXPECT_LT(lt0, bg1);
        EXPECT_LT(lt0, lt1);
        EXPECT_LE(lt0, bg1);
        EXPECT_LE(lt0, lt1);
        EXPECT_GE(bg1, lt0);
        EXPECT_GE(lt1, lt0);
        EXPECT_GT(bg1, lt0);
        EXPECT_GT(lt1, lt0);
    }

    // Differ in high byte only
    {
        big_uint16_t bg1{0x1334};
        little_uint16_t lt1{0x1334};

        EXPECT_NE(bg0, bg1);
        EXPECT_NE(bg0, lt1);
        EXPECT_LT(bg0, bg1);
        EXPECT_LT(bg0, lt1);
        EXPECT_LE(bg0, bg1);
        EXPECT_LE(bg0, lt1);
        EXPECT_GE(bg1, bg0);
        EXPECT_GE(lt1, bg0);
        EXPECT_GT(bg1, bg0);
        EXPECT_GT(lt1, bg0);

        EXPECT_NE(lt0, bg1);
        EXPECT_NE(lt0, lt1);
        EXPECT_LT(lt0, bg1);
        EXPECT_LT(lt0, lt1);
        EXPECT_LE(lt0, bg1);
        EXPECT_LE(lt0, lt1);
        EXPECT_GE(bg1, lt0);
        EXPECT_GE(lt1, lt0);
        EXPECT_GT(bg1, lt0);
        EXPECT_GT(lt1, lt0);
    }

    // Both bytes differ with high/low reversal (exposes byteswap ordering bug)
    // 0x0003=3 < 0x0100=256, but byteswapped: 0x0300 > 0x0001
    {
        big_uint16_t bg_lo{0x0003};
        big_uint16_t bg_hi{0x0100};
        little_uint16_t lt_lo{0x0003};
        little_uint16_t lt_hi{0x0100};

        // big vs big
        EXPECT_LT(bg_lo, bg_hi);
        EXPECT_GT(bg_hi, bg_lo);
        EXPECT_LE(bg_lo, bg_hi);
        EXPECT_GE(bg_hi, bg_lo);
        EXPECT_NE(bg_lo, bg_hi);

        // little vs little
        EXPECT_LT(lt_lo, lt_hi);
        EXPECT_GT(lt_hi, lt_lo);
        EXPECT_LE(lt_lo, lt_hi);
        EXPECT_GE(lt_hi, lt_lo);
        EXPECT_NE(lt_lo, lt_hi);

        // cross endian
        EXPECT_LT(bg_lo, lt_hi);
        EXPECT_GT(lt_hi, bg_lo);
        EXPECT_LT(lt_lo, bg_hi);
        EXPECT_GT(bg_hi, lt_lo);

        // equal value, different endianness
        EXPECT_EQ(bg_lo, lt_lo);
        EXPECT_EQ(bg_hi, lt_hi);
    }

    // Boundary: 0 vs max
    {
        big_uint16_t bg_zero{uint16_t(0)};
        big_uint16_t bg_max{0xFFFF};
        little_uint16_t lt_zero{uint16_t(0)};
        little_uint16_t lt_max{0xFFFF};

        EXPECT_LT(bg_zero, bg_max);
        EXPECT_LT(lt_zero, lt_max);
        EXPECT_LT(bg_zero, lt_max);
        EXPECT_LT(lt_zero, bg_max);
        EXPECT_GT(bg_max, bg_zero);
        EXPECT_GT(lt_max, lt_zero);
    }

    // Additional: high byte smaller but low byte larger
    // 0x01FF=511 < 0x0200=512
    {
        big_uint16_t bg_a{0x01FF};
        big_uint16_t bg_b{0x0200};
        little_uint16_t lt_a{0x01FF};
        little_uint16_t lt_b{0x0200};

        EXPECT_LT(bg_a, bg_b);
        EXPECT_LT(lt_a, lt_b);
        EXPECT_LT(bg_a, lt_b);
        EXPECT_LT(lt_a, bg_b);
        EXPECT_GT(bg_b, bg_a);
        EXPECT_GT(lt_b, lt_a);
    }
}

// ============================================================
// S16 tests
// ============================================================
TEST(Utility, Types_S16)
{
    // Constructor
    constexpr big_int16_t bg0;
    constexpr little_int16_t lt0;
    EXPECT_EQ(bg0.value, 0);
    EXPECT_EQ(lt0.value, 0);

    big_int16_t bg1{int16_t(0x1234)};
    little_int16_t lt1{int16_t(0x1234)};
    EXPECT_EQ(bg1.u8[0], 0x12);
    EXPECT_EQ(bg1.u8[1], 0x34);
    EXPECT_EQ(lt1.u8[0], 0x34);
    EXPECT_EQ(lt1.u8[1], 0x12);

    constexpr big_int16_t bg2{0x12, 0x34};
    constexpr little_int16_t lt2{0x12, 0x34};
    EXPECT_EQ(bg2.u8[0], 0x12);
    EXPECT_EQ(bg2.u8[1], 0x34);
    EXPECT_EQ(lt2.u8[0], 0x12);
    EXPECT_EQ(lt2.u8[1], 0x34);

    // Copy / Move
    big_int16_t bg3    = bg1;
    little_int16_t lt3 = lt1;
    EXPECT_EQ(bg3.u8[0], 0x12);
    EXPECT_EQ(bg3.u8[1], 0x34);
    EXPECT_EQ(lt3.u8[0], 0x34);
    EXPECT_EQ(lt3.u8[1], 0x12);

    big_int16_t bg4    = std::move(bg3);
    little_int16_t lt4 = std::move(lt3);
    EXPECT_EQ(bg4.u8[0], 0x12);
    EXPECT_EQ(bg4.u8[1], 0x34);
    EXPECT_EQ(lt4.u8[0], 0x34);
    EXPECT_EQ(lt4.u8[1], 0x12);

    // Assignment
    big_int16_t bg5;
    little_int16_t lt5;
    bg5 = bg4;
    lt5 = lt4;
    EXPECT_EQ(bg5.u8[0], 0x12);
    EXPECT_EQ(bg5.u8[1], 0x34);
    EXPECT_EQ(lt5.u8[0], 0x34);
    EXPECT_EQ(lt5.u8[1], 0x12);

    // Assignment from int16_t
    big_int16_t bg6;
    little_int16_t lt6;
    bg6 = int16_t(-1);
    lt6 = int16_t(-1);
    EXPECT_EQ((int16_t)bg6, -1);
    EXPECT_EQ((int16_t)lt6, -1);
    EXPECT_EQ(bg6.u8[0], 0xFF);
    EXPECT_EQ(bg6.u8[1], 0xFF);
    EXPECT_EQ(lt6.u8[0], 0xFF);
    EXPECT_EQ(lt6.u8[1], 0xFF);

    // Assignment from pair
    big_int16_t bg7;
    little_int16_t lt7;
    bg7 = std::make_pair<int, int>(0x12, 0x34);
    lt7 = std::make_pair<int, int>(0x12, 0x34);
    EXPECT_EQ(bg7.u8[0], 0x12);
    EXPECT_EQ(bg7.u8[1], 0x34);
    EXPECT_EQ(lt7.u8[0], 0x12);
    EXPECT_EQ(lt7.u8[1], 0x34);

    // Cast
    EXPECT_FALSE((bool)bg0);
    EXPECT_FALSE((bool)lt0);
    EXPECT_TRUE((bool)bg1);
    EXPECT_TRUE((bool)lt1);

    EXPECT_EQ((int16_t)bg0, 0);
    EXPECT_EQ((int16_t)lt0, 0);
    EXPECT_EQ((int16_t)bg1, 0x1234);
    EXPECT_EQ((int16_t)lt1, 0x1234);

    EXPECT_EQ(*(const uint8_t*)bg0, 0U);
    EXPECT_EQ(*(const uint8_t*)lt0, 0U);
    EXPECT_EQ(*(const uint8_t*)bg1, 0x12U);
    EXPECT_EQ(*(const uint8_t*)lt1, 0x34U);

    // Non-const uint8_t* cast
    {
        big_int16_t bg_mut{0x12, 0x34};
        uint8_t* p = static_cast<uint8_t*>(bg_mut);
        EXPECT_EQ(p[0], 0x12);
        p[0] = 0xAA;
        EXPECT_EQ(bg_mut.u8[0], 0xAA);
    }
}

TEST(Utility, Types_S16_Accessors)
{
    // high() and low()
    {
        big_int16_t bg{0x12, 0x34};
        little_int16_t lt{0x12, 0x34};

        EXPECT_EQ(bg.high(), 0x12);
        EXPECT_EQ(bg.low(), 0x34);
        EXPECT_EQ(lt.high(), 0x12);
        EXPECT_EQ(lt.low(), 0x34);
    }

    // size()
    {
        big_int16_t bg;
        little_int16_t lt;
        EXPECT_EQ(bg.size(), 2U);
        EXPECT_EQ(lt.size(), 2U);
    }

    // data()
    {
        const big_int16_t bg{0x12, 0x34};
        const little_int16_t lt{0x56, 0x78};

        const uint8_t* bg_ptr = bg.data();
        const uint8_t* lt_ptr = lt.data();

        EXPECT_EQ(bg_ptr[0], 0x12);
        EXPECT_EQ(bg_ptr[1], 0x34);
        EXPECT_EQ(lt_ptr[0], 0x56);
        EXPECT_EQ(lt_ptr[1], 0x78);
    }

    // data() non-const
    {
        big_int16_t bg{0x00, 0x00};
        uint8_t* p = bg.data();
        p[0]       = 0xAA;
        p[1]       = 0xBB;
        EXPECT_EQ(bg.u8[0], 0xAA);
        EXPECT_EQ(bg.u8[1], 0xBB);
    }
}

TEST(Utility, Types_S16_SetGet)
{
    // set/get with explicit endianness
    {
        big_int16_t bg;
        little_int16_t lt;

        bg.set<true>(0x1234);
        lt.set<true>(0x1234);
        EXPECT_EQ(bg.get<true>(), 0x1234);
        EXPECT_EQ(lt.get<true>(), 0x1234);
    }
    {
        big_int16_t bg;
        little_int16_t lt;

        bg.set<false>(0x1234);
        lt.set<false>(0x1234);
        EXPECT_EQ(bg.get<false>(), 0x1234);
        EXPECT_EQ(lt.get<false>(), 0x1234);
    }

    // Negative values
    {
        big_int16_t bg;
        little_int16_t lt;

        bg = int16_t(-256);
        lt = int16_t(-256);
        EXPECT_EQ((int16_t)bg, -256);
        EXPECT_EQ((int16_t)lt, -256);

        // -256 = 0xFF00
        EXPECT_EQ(bg.u8[0], 0xFF);
        EXPECT_EQ(bg.u8[1], 0x00);
        EXPECT_EQ(lt.u8[0], 0x00);
        EXPECT_EQ(lt.u8[1], 0xFF);
    }
}

TEST(Utility, Types_S16_BoundaryValues)
{
    // Zero
    {
        big_int16_t bg{int16_t(0)};
        little_int16_t lt{int16_t(0)};
        EXPECT_EQ((int16_t)bg, 0);
        EXPECT_EQ((int16_t)lt, 0);
    }

    // INT16_MAX (32767 = 0x7FFF)
    {
        big_int16_t bg{int16_t(32767)};
        little_int16_t lt{int16_t(32767)};
        EXPECT_EQ((int16_t)bg, 32767);
        EXPECT_EQ((int16_t)lt, 32767);
        EXPECT_EQ(bg.u8[0], 0x7F);
        EXPECT_EQ(bg.u8[1], 0xFF);
        EXPECT_EQ(lt.u8[0], 0xFF);
        EXPECT_EQ(lt.u8[1], 0x7F);
    }

    // INT16_MIN (-32768 = 0x8000)
    {
        big_int16_t bg{int16_t(-32768)};
        little_int16_t lt{int16_t(-32768)};
        EXPECT_EQ((int16_t)bg, -32768);
        EXPECT_EQ((int16_t)lt, -32768);
        EXPECT_EQ(bg.u8[0], 0x80);
        EXPECT_EQ(bg.u8[1], 0x00);
        EXPECT_EQ(lt.u8[0], 0x00);
        EXPECT_EQ(lt.u8[1], 0x80);
    }

    // -1 (0xFFFF)
    {
        big_int16_t bg{int16_t(-1)};
        little_int16_t lt{int16_t(-1)};
        EXPECT_EQ((int16_t)bg, -1);
        EXPECT_EQ((int16_t)lt, -1);
        EXPECT_EQ(bg.u8[0], 0xFF);
        EXPECT_EQ(bg.u8[1], 0xFF);
        EXPECT_EQ(lt.u8[0], 0xFF);
        EXPECT_EQ(lt.u8[1], 0xFF);
    }
}

TEST(Utility, Types_S16_Compare)
{
    // Same value across endianness
    big_int16_t bg0{int16_t(0x1234)};
    little_int16_t lt0{int16_t(0x1234)};

    EXPECT_EQ(bg0, lt0);
    EXPECT_GE(bg0, lt0);
    EXPECT_LE(bg0, lt0);
    EXPECT_FALSE(bg0 < lt0);
    EXPECT_FALSE(bg0 > lt0);

    // Positive ordering with high/low reversal
    // 3 < 256, byteswapped would reverse this
    {
        big_int16_t bg_lo{int16_t(3)};
        big_int16_t bg_hi{int16_t(256)};
        little_int16_t lt_lo{int16_t(3)};
        little_int16_t lt_hi{int16_t(256)};

        EXPECT_LT(bg_lo, bg_hi);
        EXPECT_GT(bg_hi, bg_lo);
        EXPECT_LE(bg_lo, bg_hi);
        EXPECT_GE(bg_hi, bg_lo);

        EXPECT_LT(lt_lo, lt_hi);
        EXPECT_GT(lt_hi, lt_lo);

        EXPECT_LT(bg_lo, lt_hi);
        EXPECT_GT(lt_hi, bg_lo);
        EXPECT_LT(lt_lo, bg_hi);
        EXPECT_GT(bg_hi, lt_lo);

        EXPECT_EQ(bg_lo, lt_lo);
        EXPECT_EQ(bg_hi, lt_hi);
    }

    // Negative vs positive: -1 < 1
    {
        big_int16_t bg_neg{int16_t(-1)};
        big_int16_t bg_pos{int16_t(1)};
        little_int16_t lt_neg{int16_t(-1)};
        little_int16_t lt_pos{int16_t(1)};

        EXPECT_LT(bg_neg, bg_pos);
        EXPECT_LT(lt_neg, lt_pos);
        EXPECT_LT(bg_neg, lt_pos);
        EXPECT_LT(lt_neg, bg_pos);
        EXPECT_GT(bg_pos, bg_neg);
        EXPECT_GT(lt_pos, lt_neg);
    }

    // Negative ordering: -32768 < -1
    {
        big_int16_t bg_min{int16_t(-32768)};
        big_int16_t bg_neg1{int16_t(-1)};
        little_int16_t lt_min{int16_t(-32768)};
        little_int16_t lt_neg1{int16_t(-1)};

        EXPECT_LT(bg_min, bg_neg1);
        EXPECT_LT(lt_min, lt_neg1);
        EXPECT_LT(bg_min, lt_neg1);
        EXPECT_LT(lt_min, bg_neg1);
        EXPECT_GT(bg_neg1, bg_min);
        EXPECT_GT(lt_neg1, lt_min);
    }

    // Boundary: INT16_MIN < 0 < INT16_MAX
    {
        big_int16_t bg_min{int16_t(-32768)};
        big_int16_t bg_zero{int16_t(0)};
        big_int16_t bg_max{int16_t(32767)};

        EXPECT_LT(bg_min, bg_zero);
        EXPECT_LT(bg_zero, bg_max);
        EXPECT_LT(bg_min, bg_max);
        EXPECT_GT(bg_max, bg_min);
    }

    // Negative values with high/low byte reversal
    // -256 (0xFF00) vs -255 (0xFF01): -256 < -255
    {
        big_int16_t bg_a{int16_t(-256)};
        big_int16_t bg_b{int16_t(-255)};
        little_int16_t lt_a{int16_t(-256)};
        little_int16_t lt_b{int16_t(-255)};

        EXPECT_LT(bg_a, bg_b);
        EXPECT_LT(lt_a, lt_b);
        EXPECT_LT(bg_a, lt_b);
        EXPECT_LT(lt_a, bg_b);
    }

    // -2 (0xFFFE) vs -257 (0xFEFF): -257 < -2
    // byteswapped on LE: 0xFEFF -> 0xFFFE, 0xFFFE -> 0xFEFF (reversed!)
    {
        big_int16_t bg_a{int16_t(-257)};
        big_int16_t bg_b{int16_t(-2)};
        little_int16_t lt_a{int16_t(-257)};
        little_int16_t lt_b{int16_t(-2)};

        EXPECT_LT(bg_a, bg_b);
        EXPECT_LT(lt_a, lt_b);
        EXPECT_LT(bg_a, lt_b);
        EXPECT_LT(lt_a, bg_b);
        EXPECT_GT(bg_b, bg_a);
        EXPECT_GT(lt_b, lt_a);
    }
}

// ============================================================
// U32 tests
// ============================================================
TEST(Utility, Types_U32)
{
    // Constructor
    constexpr big_uint32_t bg0;
    constexpr little_uint32_t lt0;
    EXPECT_EQ(bg0.value, 0U);
    EXPECT_EQ(lt0.value, 0U);

    big_uint32_t bg1{0x12345678U};
    little_uint32_t lt1{0x12345678U};
    EXPECT_EQ(bg1.u8[0], 0x12);
    EXPECT_EQ(bg1.u8[1], 0x34);
    EXPECT_EQ(bg1.u8[2], 0x56);
    EXPECT_EQ(bg1.u8[3], 0x78);
    EXPECT_EQ(lt1.u8[0], 0x78);
    EXPECT_EQ(lt1.u8[1], 0x56);
    EXPECT_EQ(lt1.u8[2], 0x34);
    EXPECT_EQ(lt1.u8[3], 0x12);

    // Byte constructor
    constexpr big_uint32_t bg2{0x12, 0x34, 0x56, 0x78};
    EXPECT_EQ(bg2.u8[0], 0x12);
    EXPECT_EQ(bg2.u8[1], 0x34);
    EXPECT_EQ(bg2.u8[2], 0x56);
    EXPECT_EQ(bg2.u8[3], 0x78);

    // Cast
    EXPECT_FALSE((bool)bg0);
    EXPECT_TRUE((bool)bg1);
    EXPECT_EQ((uint32_t)bg1, 0x12345678U);
    EXPECT_EQ((uint32_t)lt1, 0x12345678U);

    // size
    EXPECT_EQ(bg0.size(), 4U);

    // high/low
    EXPECT_EQ(bg1.high(), 0x12);
    EXPECT_EQ(bg1.low(), 0x78);
    EXPECT_EQ(lt1.high(), 0x78);
    EXPECT_EQ(lt1.low(), 0x12);
}

TEST(Utility, Types_U32_SetGet)
{
    {
        big_uint32_t bg;
        little_uint32_t lt;

        bg.set<true>(0x12345678U);
        lt.set<true>(0x12345678U);
        EXPECT_EQ(bg.get<true>(), 0x12345678U);
        EXPECT_EQ(lt.get<true>(), 0x12345678U);
    }
    {
        big_uint32_t bg;
        little_uint32_t lt;

        bg.set<false>(0x12345678U);
        lt.set<false>(0x12345678U);
        EXPECT_EQ(bg.get<false>(), 0x12345678U);
        EXPECT_EQ(lt.get<false>(), 0x12345678U);
    }
}

TEST(Utility, Types_U32_Compare)
{
    // Same value
    big_uint32_t bg0{0x12345678U};
    little_uint32_t lt0{0x12345678U};
    EXPECT_EQ(bg0, lt0);

    // Both bytes differ with reversal (0x00000003=3 < 0x01000000)
    {
        big_uint32_t bg_lo{0x00000003U};
        big_uint32_t bg_hi{0x01000000U};
        EXPECT_LT(bg_lo, bg_hi);
        EXPECT_GT(bg_hi, bg_lo);
    }

    // Cross endian
    {
        big_uint32_t bg{0x00000003U};
        little_uint32_t lt{0x01000000U};
        EXPECT_LT(bg, lt);
        EXPECT_GT(lt, bg);
    }

    // Boundary
    {
        big_uint32_t bg_zero{0U};
        big_uint32_t bg_max{0xFFFFFFFFU};
        EXPECT_LT(bg_zero, bg_max);
        EXPECT_GT(bg_max, bg_zero);
    }
}

// ============================================================
// S32 tests
// ============================================================
TEST(Utility, Types_S32)
{
    // Constructor
    constexpr big_int32_t bg0;
    EXPECT_EQ(bg0.value, 0);

    big_int32_t bg1{int32_t(0x12345678)};
    little_int32_t lt1{int32_t(0x12345678)};
    EXPECT_EQ(bg1.u8[0], 0x12);
    EXPECT_EQ(bg1.u8[1], 0x34);
    EXPECT_EQ(bg1.u8[2], 0x56);
    EXPECT_EQ(bg1.u8[3], 0x78);
    EXPECT_EQ(lt1.u8[0], 0x78);
    EXPECT_EQ(lt1.u8[1], 0x56);
    EXPECT_EQ(lt1.u8[2], 0x34);
    EXPECT_EQ(lt1.u8[3], 0x12);

    // Negative
    big_int32_t bg_neg{int32_t(-1)};
    little_int32_t lt_neg{int32_t(-1)};
    EXPECT_EQ((int32_t)bg_neg, -1);
    EXPECT_EQ((int32_t)lt_neg, -1);
    EXPECT_EQ(bg_neg.u8[0], 0xFF);
    EXPECT_EQ(bg_neg.u8[1], 0xFF);
    EXPECT_EQ(bg_neg.u8[2], 0xFF);
    EXPECT_EQ(bg_neg.u8[3], 0xFF);

    // Cast
    EXPECT_EQ((int32_t)bg1, 0x12345678);
    EXPECT_EQ((int32_t)lt1, 0x12345678);

    // size
    EXPECT_EQ(bg0.size(), 4U);
}

TEST(Utility, Types_S32_Compare)
{
    // Negative vs positive
    {
        big_int32_t bg_neg{int32_t(-1)};
        big_int32_t bg_pos{int32_t(1)};
        little_int32_t lt_neg{int32_t(-1)};
        little_int32_t lt_pos{int32_t(1)};

        EXPECT_LT(bg_neg, bg_pos);
        EXPECT_LT(lt_neg, lt_pos);
        EXPECT_LT(bg_neg, lt_pos);
        EXPECT_GT(bg_pos, bg_neg);
    }

    // Byte reversal: 3 < 0x01000000
    {
        big_int32_t bg_lo{int32_t(3)};
        big_int32_t bg_hi{int32_t(0x01000000)};
        EXPECT_LT(bg_lo, bg_hi);
        EXPECT_GT(bg_hi, bg_lo);
    }

    // INT32_MIN < 0 < INT32_MAX
    {
        big_int32_t bg_min{int32_t(-2147483648)};
        big_int32_t bg_zero{int32_t(0)};
        big_int32_t bg_max{int32_t(2147483647)};

        EXPECT_LT(bg_min, bg_zero);
        EXPECT_LT(bg_zero, bg_max);
        EXPECT_LT(bg_min, bg_max);
    }

    // Cross endian
    {
        big_int32_t bg{int32_t(-257)};
        little_int32_t lt{int32_t(-2)};
        EXPECT_LT(bg, lt);
        EXPECT_GT(lt, bg);
    }
}

// ============================================================
// 64-bit tests
// ============================================================
TEST(Utility, Types_U64)
{
    constexpr big_uint64_t bg0;
    EXPECT_EQ(bg0.value, 0U);

    big_uint64_t bg1{uint64_t(0x0102030405060708ULL)};
    little_uint64_t lt1{uint64_t(0x0102030405060708ULL)};

    EXPECT_EQ(bg1.u8[0], 0x01);
    EXPECT_EQ(bg1.u8[7], 0x08);
    EXPECT_EQ(lt1.u8[0], 0x08);
    EXPECT_EQ(lt1.u8[7], 0x01);

    EXPECT_EQ((uint64_t)bg1, 0x0102030405060708ULL);
    EXPECT_EQ((uint64_t)lt1, 0x0102030405060708ULL);
    EXPECT_EQ(bg1, lt1);
    EXPECT_EQ(bg0.size(), 8U);

    // Ordering with byte reversal
    {
        big_uint64_t a{uint64_t(3)};
        big_uint64_t b{uint64_t(0x0100000000000000ULL)};
        EXPECT_LT(a, b);
        EXPECT_GT(b, a);
    }
}

TEST(Utility, Types_S64)
{
    big_int64_t bg_neg{int64_t(-1)};
    big_int64_t bg_pos{int64_t(1)};
    little_int64_t lt_neg{int64_t(-1)};

    EXPECT_EQ((int64_t)bg_neg, -1);
    EXPECT_LT(bg_neg, bg_pos);
    EXPECT_LT(lt_neg, bg_pos);
    EXPECT_EQ(bg_neg, lt_neg);
    EXPECT_EQ(bg_neg.size(), 8U);
}

// ============================================================
// 128-bit tests
// ============================================================
#if defined(__SIZEOF_INT128__)
TEST(Utility, Types_U128)
{
    using unsigned128 = unsigned __int128;

    constexpr big_uint128_t bg0;
    EXPECT_EQ(bg0.value, unsigned128(0));
    EXPECT_EQ(bg0.size(), 16U);

    // 0x0102030405060708090A0B0C0D0E0F10
    unsigned128 val = (unsigned128(0x0102030405060708ULL) << 64) | unsigned128(0x090A0B0C0D0E0F10ULL);
    big_uint128_t bg1{val};
    little_uint128_t lt1{val};

    // Big-endian: MSB first
    EXPECT_EQ(bg1.u8[0], 0x01);
    EXPECT_EQ(bg1.u8[1], 0x02);
    EXPECT_EQ(bg1.u8[14], 0x0F);
    EXPECT_EQ(bg1.u8[15], 0x10);

    // Little-endian: LSB first
    EXPECT_EQ(lt1.u8[0], 0x10);
    EXPECT_EQ(lt1.u8[1], 0x0F);
    EXPECT_EQ(lt1.u8[14], 0x02);
    EXPECT_EQ(lt1.u8[15], 0x01);

    // Round-trip via get()
    EXPECT_TRUE(bg1.get() == val);
    EXPECT_TRUE(lt1.get() == val);

    // Cross-endian equality
    EXPECT_EQ(bg1, lt1);

    // Cast
    EXPECT_FALSE((bool)bg0);
    EXPECT_TRUE((bool)bg1);

    // high/low
    EXPECT_EQ(bg1.high(), 0x01);
    EXPECT_EQ(bg1.low(), 0x10);
    EXPECT_EQ(lt1.high(), 0x10);
    EXPECT_EQ(lt1.low(), 0x01);

    // Ordering with byte reversal
    {
        big_uint128_t a{unsigned128(3)};
        unsigned128 big_val = unsigned128(1) << 120;
        big_uint128_t b{big_val};
        EXPECT_TRUE(a < b);
        EXPECT_TRUE(b > a);
    }

    // Boundary: 0 vs max
    {
        unsigned128 max_val = ~unsigned128(0);
        big_uint128_t bg_zero{unsigned128(0)};
        big_uint128_t bg_max{max_val};
        EXPECT_TRUE(bg_zero < bg_max);
        EXPECT_TRUE(bg_max > bg_zero);
    }
}

TEST(Utility, Types_S128)
{
    using signed128   = __int128;
    using unsigned128 = unsigned __int128;

    // Constructor / cast
    big_int128_t bg_neg{signed128(-1)};
    big_int128_t bg_pos{signed128(1)};
    little_int128_t lt_neg{signed128(-1)};
    little_int128_t lt_pos{signed128(1)};

    EXPECT_TRUE(bg_neg.get() == signed128(-1));
    EXPECT_TRUE(lt_neg.get() == signed128(-1));
    EXPECT_EQ(bg_neg.size(), 16U);

    // All bytes 0xFF for -1
    for (size_t i = 0; i < 16; ++i) {
        EXPECT_EQ(bg_neg.u8[i], 0xFF);
        EXPECT_EQ(lt_neg.u8[i], 0xFF);
    }

    // Cross-endian equality
    EXPECT_EQ(bg_neg, lt_neg);
    EXPECT_EQ(bg_pos, lt_pos);

    // Negative vs positive
    EXPECT_TRUE(bg_neg < bg_pos);
    EXPECT_TRUE(lt_neg < lt_pos);
    EXPECT_TRUE(bg_neg < lt_pos);
    EXPECT_TRUE(lt_neg < bg_pos);
    EXPECT_TRUE(bg_pos > bg_neg);

    // MIN < 0 < MAX
    {
        signed128 min_val = signed128(1) << 127;     // INT128_MIN
        signed128 max_val = ~(signed128(1) << 127);  // INT128_MAX
        big_int128_t bg_min{min_val};
        big_int128_t bg_zero{signed128(0)};
        big_int128_t bg_max{max_val};

        EXPECT_TRUE(bg_min < bg_zero);
        EXPECT_TRUE(bg_zero < bg_max);
        EXPECT_TRUE(bg_min < bg_max);
    }

    // Byte reversal: 3 vs large positive
    {
        big_int128_t a{signed128(3)};
        signed128 big_val = signed128(1) << 64;
        big_int128_t b{big_val};
        EXPECT_TRUE(a < b);
        EXPECT_TRUE(b > a);
    }
}
#endif
