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
#include <random>

using namespace m5::utility;

namespace {
auto rng = std::default_random_engine{};

// x^16 + x^14 + x^13 + x^11 + 1
uint32_t prng_successor32(uint32_t x, uint32_t n)
{
    while (n--) {
        //  0            16(32-16)   14(32-18)   13(32-19)   11(32-11)
        x = (x >> 1) | (((x >> 16) ^ (x >> 18) ^ (x >> 19) ^ (x >> 21)) << 31);
    }
    return x;
}

uint16_t prng_successor16(uint16_t x, uint32_t n)
{
    while (n--) {
        //                0(16-16)  14(16-2)   13(16-3)   11(16-5)
        x = (x >> 1) | (((x >> 0) ^ (x >> 2) ^ (x >> 3) ^ (x >> 5)) << 15);
    }
    return x;
}

}  // namespace

TEST(Utility, FibonacciLFSR16)
{
    {
        using LFSR16 = FibonacciLFSR_Right<16, 16, 14, 13, 11>;

        constexpr uint16_t seed{0xACE1U};
        LFSR16 lfsr{seed};
        //        uint16_t reg = seed;
        //        uint16_t bit;
        uint16_t p16 = seed;
        for (int i = 0; i < 65536; ++i) {
            p16 = prng_successor16(p16, 1);
#if 0
            bool out = (reg >> 1);
            bit      = (reg & 0x0001) ^ ((reg & 0x0004) >> 2) ^ ((reg & 0x0008) >> 3) ^ ((reg & 0x0020) >> 5);
            reg      = (reg >> 1) | (bit << 15);
#endif
            lfsr.step();
            EXPECT_EQ(lfsr.value(), p16) << i;
        }
    }
}

TEST(Utility, FibonacciLFSR32)
{
    {
        using LFSR32 = FibonacciLFSR_Right<32, 16, 14, 13, 11>;

        constexpr uint32_t seed{0xACE1U};
        LFSR32 lfsr{seed};
        uint32_t p32 = seed;
        for (int i = 0; i < 65536; ++i) {
            p32 = prng_successor32(p32, 1);
            lfsr.step();
            EXPECT_EQ(lfsr.value(), p32) << i;
        }
    }
}

using FLL = FibonacciLFSR_Left<8, 8, 5>;
class DruagaLFSR : public FLL {
public:
    using Base = FLL;
    using Base::Base;
    using typename Base::state_type_t;

    bool step() noexcept
    {
        const bool fb = !Base::taps_xor_all(this->_state);  // NOT tap
        this->_state <<= 1;                                 // shift
        this->_state.set(0, fb);                            // Insert into LSB
        return fb;
    };

    uint64_t step(const uint32_t nbits) noexcept
    {
        uint64_t v{};
        for (uint32_t i = 0; i < nbits; ++i) v |= (uint64_t)step() << i;
        return v;
    }

    uint8_t next2() noexcept
    {
        return (uint8_t)step(2);
    }
    uint8_t next4() noexcept
    {
        return (uint8_t)step(4);
    }
    uint8_t next8() noexcept
    {
        return (uint8_t)step(8);
    }
    uint16_t next16() noexcept
    {
        return (uint16_t)step(16);
    }
    uint32_t next32() noexcept
    {
        return (uint32_t)step(32);
    }
    uint64_t next64() noexcept
    {
        return step(64);
    }
};

TEST(Utility, DruagaLFSR)
{
    {  // 1 cycle
        DruagaLFSR d(255);
        EXPECT_EQ(d.next2(), 0x03);
        EXPECT_EQ(d.next4(), 0x0F);
        EXPECT_EQ(d.next8(), 0xFF);
        EXPECT_EQ(d.next16(), 0xFFFF);
        EXPECT_EQ(d.next32(), 0xFFFFFFFF);
        EXPECT_EQ(d.next64(), 0xFFFFFFFFFFFFFFFFull);
    }

    if (1) {  // 217 cycle
        DruagaLFSR d(0);
        std::vector<uint8_t> a1, a2;

        for (int i = 0; i < 217; ++i) {
            d.step();
            a1.push_back((uint8_t)d.value());
            // M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        for (int i = 0; i < 217; ++i) {
            d.step();
            a2.push_back((uint8_t)d.value());
            // M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        //        m5::utility::log::dump(a1.data(), a1.size(), false);
        //       m5::utility::log::dump(a2.data(), a2.size(), false);
        EXPECT_EQ(a1, a2);
    }

    if (1) {  // 31 cycle
        DruagaLFSR d(6);
        std::vector<uint8_t> a1, a2;

        for (int i = 0; i < 31; ++i) {
            d.step();
            a1.push_back((uint8_t)d.value());
            // M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        for (int i = 0; i < 31; ++i) {
            d.step();
            a2.push_back((uint8_t)d.value());
            // M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        //        m5::utility::log::dump(a1.data(), a1.size(), false);
        //        m5::utility::log::dump(a2.data(), a2.size(), false);
        EXPECT_EQ(a1, a2);
    }

    if (1) {  // 7 cycle
        DruagaLFSR d(26);
        std::vector<uint8_t> a1, a2;

        for (int i = 0; i < 7; ++i) {
            d.step();
            a1.push_back((uint8_t)d.value());
            //            M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        for (int i = 0; i < 7; ++i) {
            d.step();
            a2.push_back((uint8_t)d.value());
            //            M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        //        m5::utility::log::dump(a1.data(), a1.size(), false);
        //        m5::utility::log::dump(a2.data(), a2.size(), false);
        EXPECT_EQ(a1, a2);
    }
}
