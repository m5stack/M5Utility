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

using namespace m5::utility;

namespace {
}

using FLL = FibonacciLFSR_Left<8, 4, 1>;  // Tapbit // bit 4,7
class DruagaLFSR : public FLL {
public:
    using Base = FLL;
    using Base::Base;
    using typename Base::state_type_t;

    bool step() noexcept
    {
        const bool out = this->_state.test(7);  // MSB
        const bool fb  = !Base::taps_xor_all(this->_state);
        this->_state <<= 1;       // shift
        this->_state.set(0, fb);  // Insert into LSB
        return out;
    }

    uint64_t step(uint32_t nbits) noexcept
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

TEST(Utility, FibonacciLFSR)
{
    using LFSR16 = FibonacciLFSR_Right<16, 16, 14, 13, 11>;

    LFSR16 l(0xACE1U);
    uint16_t reg = 0xACE1;
    uint16_t bit;
    uint16_t first{};

    for (int i = 0; i < 65536; ++i) {
        // bit 0, 2,3,5
        // x^16 + x^(16-2) + x^(16-3) + x^(16-5) + 1
        // x^16 + x^14 + x^13 + x^11 + 1
        bit = (reg & 0x0001) ^ ((reg & 0x0004) >> 2) ^ ((reg & 0x0008) >> 3) ^ ((reg & 0x0020) >> 5);
        reg = (reg >> 1) | (bit << 15);

        l.step();
        EXPECT_EQ(l.value(), reg);
        //        M5_LOGI("[%3d]:%04X:%04X", i, reg, (uint16_t)l.value());

        if (i == 0) {
            first = reg;
        }
        if (i == 65535) {
            EXPECT_EQ(reg, first);
        }
    }
}

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

    {  // 217 cycle
        DruagaLFSR d(0);
        std::vector<uint8_t> a1, a2;

        for (int i = 0; i < 217; ++i) {
            d.step();
            a1.push_back((uint8_t)d.value());
            //            M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        for (int i = 0; i < 217; ++i) {
            d.step();
            a2.push_back((uint8_t)d.value());
            //            M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        //        m5::utility::log::dump(a1.data(), a1.size(), false);
        //       m5::utility::log::dump(a2.data(), a2.size(), false);
        EXPECT_EQ(a1, a2);
    }

    {  // 31 cycle
        DruagaLFSR d(6);
        std::vector<uint8_t> a1, a2;

        for (int i = 0; i < 31; ++i) {
            d.step();
            a1.push_back((uint8_t)d.value());
            //            M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        for (int i = 0; i < 31; ++i) {
            d.step();
            a2.push_back((uint8_t)d.value());
            //            M5_LOGI("[%3d]:%u", i, (uint8_t)d.value());
        }
        //        m5::utility::log::dump(a1.data(), a1.size(), false);
        //        m5::utility::log::dump(a2.data(), a2.size(), false);
        EXPECT_EQ(a1, a2);
    }

    {  // 7 cycle
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
