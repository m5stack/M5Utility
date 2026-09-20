/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file conversion.hpp
  @brief Numeric conversion
*/
#ifndef M5_UTILITY_CONVERSION_HPP
#define M5_UTILITY_CONVERSION_HPP

#include <type_traits>
#include <limits>
#include <cstdint>
#include <cstdint>
#include <cstddef>

namespace m5 {
namespace utility {

///@cond
namespace detail {

//! @brief Mask of the low `bits` bits, without shifting by the full width
constexpr uint64_t low_bits_mask(const size_t bits)
{
    return (bits >= 64) ? ~uint64_t{0} : ((uint64_t{1} << bits) - 1);
}

/*!
  @brief Sign-extends the low `bits` bits of v
  @note 2^bits is expressed as (mask + 1) so that bits == 64 does not shift by the
  full width, which would be undefined
 */
template <typename T>
constexpr typename std::make_signed<T>::type to_signed_bits(const T v, const size_t bits)
{
    using S = typename std::make_signed<T>::type;
    return (bits == 0)
               ? S{0}
               : ((static_cast<uint64_t>(v) & (uint64_t{1} << (bits - 1)))
                      ? static_cast<S>((static_cast<uint64_t>(v) & low_bits_mask(bits)) - (low_bits_mask(bits) + 1))
                      : static_cast<S>(static_cast<uint64_t>(v) & low_bits_mask(bits)));
}

}  // namespace detail
///@endcond

/*!
  @brief Convert an unsigned integer of any maximum number of bits to a signed
  integer
  @tparam Bits Number of bits assumed by value
  @code {.cpp}
  uint32_t u24{0x00FFFFFF};
  // 24 bit unsigned int to int32_t
  uint32_t s32 = unsigned_to_signed<24>(u24);
  // s32 is -1 (Not 16777215)
  @endcode
 */
template <size_t Bits, typename T>
constexpr auto unsigned_to_signed(const T v) -> typename std::make_signed<T>::type
{
    static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value, "T must be an unsigned integer");
    static_assert(Bits <= sizeof(T) * 8, "Bits must be less than or equal to the number of bits in T");

    return detail::to_signed_bits(v, Bits);
}

/*!
  @brief Convert an unsigned integer to a signed integer, with the number of bits
  given at run time
  @tparam T Unsigned integer type of the value
  @param v Value to convert
  @param bits Number of bits assumed by value (1 to the width of T; 0 yields 0)
  @code {.cpp}
  // A table-driven field whose width is only known at run time
  int32_t s = unsigned_to_signed(raw, signal.bit_length);
  @endcode
  @note Use the unsigned_to_signed<Bits>() overload whenever the width is a constant:
  it can check the width against T at compile time, which this one cannot
 */
template <typename T>
constexpr auto unsigned_to_signed(const T v, const size_t bits) -> typename std::make_signed<T>::type
{
    static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value, "T must be an unsigned integer");

    return detail::to_signed_bits(v, bits);
}

/*!
  @brief Narrows an unsigned integer, saturating at the maximum of the destination
  @tparam To Destination unsigned integer type
  @tparam From Source unsigned integer type
  @param v Value to convert
  @return v, or the maximum of To when v exceeds it
  @code {.cpp}
  // 0x1FFFF does not fit in uint16_t
  uint16_t u16 = saturate_cast<uint16_t>(size_t{0x1FFFF});
  // u16 is 0xFFFF (Not 0xFFFF & 0x1FFFF == 0xFFFF... it saturates, it does not wrap)
  @endcode
  @note Only unsigned to unsigned for now. The comparison goes through uintmax_t,
  so a From narrower than To is handled as well
 */
template <typename To, typename From>
constexpr To saturate_cast(const From v)
{
    static_assert(std::is_integral<To>::value && std::is_unsigned<To>::value, "To must be an unsigned integer");
    static_assert(std::is_integral<From>::value && std::is_unsigned<From>::value, "From must be an unsigned integer");

    return static_cast<To>(static_cast<uintmax_t>(v) > static_cast<uintmax_t>(std::numeric_limits<To>::max())
                               ? std::numeric_limits<To>::max()
                               : v);
}

}  // namespace utility
}  // namespace m5
#endif
