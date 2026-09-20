/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file misc.hpp
  @brief Miscellaneous features
*/
#ifndef M5_UTILITY_MISC_HPP
#define M5_UTILITY_MISC_HPP

#include <cstdint>
#include <type_traits>

namespace m5 {
namespace utility {

//!  @brief Valid I2C address?
inline bool isValidI2CAddress(const uint16_t addr)
{
    if (addr <= 0x7F) {  // 7 bit
        return (addr >= 0x08 && addr <= 0x77);
    }
    return addr <= 0x3FF;  // 10 bit
}

///@cond
namespace detail {
/*!
  @brief Reversing the bit order without a compiler builtin
  @note Always compiled, so that it can be unit-tested on a toolchain whose
  reverseBitOrder() takes the builtin path
 */
inline uint8_t reverseBitOrderPortable(const uint8_t u8)
{
    uint8_t v{u8};
    v = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4);
    v = ((v & 0xCC) >> 2) | ((v & 0x33) << 2);
    v = ((v & 0xAA) >> 1) | ((v & 0x55) << 1);
    return v;
}

//! @brief Reversing the bit order without a compiler builtin
inline uint16_t reverseBitOrderPortable(const uint16_t u16)
{
    uint16_t v{u16};
    v = ((v & 0xFF00) >> 8) | ((v & 0x00FF) << 8);
    v = ((v & 0xF0F0) >> 4) | ((v & 0x0F0F) << 4);
    v = ((v & 0xCCCC) >> 2) | ((v & 0x3333) << 2);
    v = ((v & 0xAAAA) >> 1) | ((v & 0x5555) << 1);
    return v;
}
/*!
  @brief Parity without a compiler builtin
  @note Always compiled, so that it can be unit-tested on a toolchain whose
  parity() takes the builtin path
 */
inline bool parityPortable(const uint8_t u8)
{
    uint8_t v{u8};
    v ^= static_cast<uint8_t>(v >> 4);
    v ^= static_cast<uint8_t>(v >> 2);
    v ^= static_cast<uint8_t>(v >> 1);
    return (v & 0x01) != 0;
}
}  // namespace detail
///@endcond

//! @brief Reversing the bit order
inline uint8_t reverseBitOrder(const uint8_t u8)
{
#if defined(__clang__)
    return __builtin_bitreverse8(u8);
#else
    return detail::reverseBitOrderPortable(u8);
#endif
}

//! @brief Reversing the bit order
inline uint16_t reverseBitOrder(const uint16_t u16)
{
#if defined(__clang__)
    return __builtin_bitreverse16(u16);
#else
    return detail::reverseBitOrderPortable(u16);
#endif
}

/*!
  @brief Is the number of set bits odd?
  @param u8 Input byte
  @return True if the byte has an odd number of set bits
 */
inline bool parity(const uint8_t u8)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_parity(u8) != 0;
#else
    return detail::parityPortable(u8);
#endif
}

/*!
  @brief Gets the bit that makes the byte odd parity
  @param u8 Input byte
  @return 1 if the byte has an even number of set bits, 0 otherwise
  @note The bit to append in protocols that specify odd parity, such as NFC-A
 */
inline uint8_t oddParityBit(const uint8_t u8)
{
    return parity(u8) ? 0 : 1;
}

/*!
  @class uint_least_for_bits
  @brief Gets the smallest unsigned integer type that can store N bit
  @tparam N Number of bits
  @note  using foo_t = uint_least_for_bits<52>::type; // foo_t == uint64_t
 */
template <uint32_t N>
struct uint_least_for_bits {
    static_assert(N >= 1, "N must be >= 1");
#if defined(__SIZEOF_INT128__)
    static_assert(N <= 128, "N must be <= 128");
    using type =                                //
        typename std::conditional<              //
            (N <= 8), uint8_t,                  //
            typename std::conditional<          //
                (N <= 16), uint16_t,            //
                typename std::conditional<      //
                    (N <= 32), uint32_t,        //
                    typename std::conditional<  //
                        (N <= 64), uint64_t,    //
                        unsigned __int128       //
                        >::type                 //
                    >::type                     //
                >::type                         //
            >::type;
#else
    static_assert(N <= 64, "N must be <= 64");
    using type =                            //
        typename std::conditional<          //
            (N <= 8), uint8_t,              //
            typename std::conditional<      //
                (N <= 16), uint16_t,        //
                typename std::conditional<  //
                    (N <= 32), uint32_t,    //
                    uint64_t                //
                    >::type                 //
                >::type                     //
            >::type;
#endif
};

}  // namespace utility
}  // namespace m5
#endif
