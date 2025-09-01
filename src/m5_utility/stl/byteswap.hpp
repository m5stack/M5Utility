/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file byteswap.hpp
  @brief std::byteswap for less than C++23
*/
#ifndef M5_UTILITY_STL_BYTESWAP_HPP
#define M5_UTILITY_STL_BYTESWAP_HPP

#include <stdint.h>
#include <type_traits>
#include <cstring>  // memcpy

namespace m5 {
namespace stl {

///@cond
template <typename U,
          typename std::enable_if<sizeof(U) == 2 && std::is_unsigned<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return static_cast<U>((static_cast<uint16_t>(x >> 8) & 0x00FFu) | (static_cast<uint16_t>(x << 8) & 0xFF00u));
}

template <typename U,
          typename std::enable_if<sizeof(U) == 4 && std::is_unsigned<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return static_cast<U>(((static_cast<uint32_t>(x) >> 24)) | ((static_cast<uint32_t>(x) >> 8) & 0x0000FF00u) |
                          ((static_cast<uint32_t>(x) << 8) & 0x00FF0000u) | ((static_cast<uint32_t>(x) << 24)));
}

template <typename U,
          typename std::enable_if<sizeof(U) == 8 && std::is_unsigned<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return static_cast<U>(
        ((static_cast<uint64_t>(x) >> 56)) | ((static_cast<uint64_t>(x) >> 40) & 0x000000000000FF00ull) |
        ((static_cast<uint64_t>(x) >> 24) & 0x0000000000FF0000ull) |
        ((static_cast<uint64_t>(x) >> 8) & 0x00000000FF000000ull) |
        ((static_cast<uint64_t>(x) << 8) & 0x000000FF00000000ull) |
        ((static_cast<uint64_t>(x) << 24) & 0x0000FF0000000000ull) |
        ((static_cast<uint64_t>(x) << 40) & 0x00FF000000000000ull) | ((static_cast<uint64_t>(x) << 56)));
}

#if defined(__SIZEOF_INT128__)
template <typename U,
          typename std::enable_if<sizeof(U) == 16 && std::is_unsigned<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return ((static_cast<U>(bswap_fixed<uint64_t>(static_cast<uint64_t>(x))) << 64) |
            static_cast<U>(bswap_fixed<uint64_t>(static_cast<uint64_t>(x >> 64))));
}
#endif

template <typename U, typename std::enable_if<sizeof(U) == 1, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return x;
}

template <typename T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value),
                                              std::nullptr_t>::type = nullptr>
inline constexpr T byteswap_constexpr(T v) noexcept
{
    using U = typename std::make_unsigned<T>::type;
    return static_cast<T>(bswap_fixed<U>(static_cast<U>(v)));
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, std::nullptr_t>::type = nullptr>
T byteswap_any(T v)
{
    if (sizeof(T) == 1) {
        return v;
    }
    if (sizeof(T) == 2) {
        uint16_t u;
        ::memcpy(&u, &v, 2);
        u = byteswap_constexpr(u);
        ::memcpy(&v, &u, 2);
        return v;
    }
    if (sizeof(T) == 4) {
        uint32_t u;
        ::memcpy(&u, &v, 4);
        u = byteswap_constexpr(u);
        ::memcpy(&v, &u, 4);
        return v;
    }
    if (sizeof(T) == 8) {
        uint64_t u;
        ::memcpy(&u, &v, 8);
        u = byteswap_constexpr(u);
        ::memcpy(&v, &u, 8);
        return v;
    }
#if defined(__SIZEOF_INT128__)
    if (sizeof(T) == 16) {
        unsigned __int128 u = 0;
        ::memcpy(&u, &v, 16);
        u = bswap_fixed(u);
        ::memcpy(&v, &u, 16);
        return v;
    }
#endif
    return v;
}
///@endcond

//! @brief byteswap for integral type
template <typename T, typename std::enable_if<(std::is_integral<T>::value || std::is_enum<T>::value),
                                              std::nullptr_t>::type = nullptr>
inline constexpr T byteswap(T v) noexcept
{
    return byteswap_constexpr(v);
}

//! @brief byteswap for floating point type
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, std::nullptr_t>::type = nullptr>
inline T byteswap(T v)
{
    return byteswap_any(v);
}

// If the type is not a number, it is prohibited
template <typename T, typename std::enable_if<!(std::is_floating_point<T>::value) &&
                                                  !(std::is_integral<T>::value || std::is_enum<T>::value),
                                              std::nullptr_t>::type = nullptr>
T byteswap(T) = delete;

}  // namespace stl
}  // namespace m5

#endif
