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

namespace detail {

template <typename T>
struct is_int128 : std::false_type {  //
};

#if defined(__SIZEOF_INT128__)
template <>
struct is_int128<__int128> : std::true_type {  //
};

template <>
struct is_int128<unsigned __int128> : std::true_type {  //
};
#endif

template <typename T>
struct is_integer_or_enum : std::integral_constant<bool, std::is_integral<typename std::remove_cv<T>::type>::value ||
                                                             std::is_enum<typename std::remove_cv<T>::type>::value ||
                                                             is_int128<typename std::remove_cv<T>::type>::value> {  //
};

template <typename T>
struct is_unsigned_ex : std::is_unsigned<typename std::remove_cv<T>::type> {};

#if defined(__SIZEOF_INT128__)
template <>
struct is_unsigned_ex<unsigned __int128> : std::true_type {};
#endif

template <typename T>
struct make_unsigned_impl {
    using type = typename std::make_unsigned<T>::type;
};

template <typename T>
struct make_unsigned_ex {
    using type = typename make_unsigned_impl<typename std::remove_cv<T>::type>::type;
};

#if defined(__SIZEOF_INT128__)
template <>
struct make_unsigned_impl<__int128> {
    using type = unsigned __int128;
};

template <>
struct make_unsigned_impl<unsigned __int128> {
    using type = unsigned __int128;
};
#endif

}  // namespace detail

///@cond
// Portable bit manipulation (always available for testing)
template <typename U,
          typename std::enable_if<sizeof(U) == 2 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_portable(U x) noexcept
{
    return static_cast<U>((static_cast<uint16_t>(x >> 8) & 0x00FFu) | (static_cast<uint16_t>(x << 8) & 0xFF00u));
}

template <typename U,
          typename std::enable_if<sizeof(U) == 4 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_portable(U x) noexcept
{
    return static_cast<U>(((static_cast<uint32_t>(x) >> 24)) | ((static_cast<uint32_t>(x) >> 8) & 0x0000FF00u) |
                          ((static_cast<uint32_t>(x) << 8) & 0x00FF0000u) | ((static_cast<uint32_t>(x) << 24)));
}

template <typename U,
          typename std::enable_if<sizeof(U) == 8 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_portable(U x) noexcept
{
    return static_cast<U>(
        ((static_cast<uint64_t>(x) >> 56)) | ((static_cast<uint64_t>(x) >> 40) & 0x000000000000FF00ull) |
        ((static_cast<uint64_t>(x) >> 24) & 0x0000000000FF0000ull) |
        ((static_cast<uint64_t>(x) >> 8) & 0x00000000FF000000ull) |
        ((static_cast<uint64_t>(x) << 8) & 0x000000FF00000000ull) |
        ((static_cast<uint64_t>(x) << 24) & 0x0000FF0000000000ull) |
        ((static_cast<uint64_t>(x) << 40) & 0x00FF000000000000ull) | ((static_cast<uint64_t>(x) << 56)));
}

#if defined(__GNUC__) || defined(__clang__)
// GCC/Clang: use __builtin_bswap for optimal single-instruction output
template <typename U,
          typename std::enable_if<sizeof(U) == 2 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return static_cast<U>(__builtin_bswap16(static_cast<uint16_t>(x)));
}

template <typename U,
          typename std::enable_if<sizeof(U) == 4 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return static_cast<U>(__builtin_bswap32(static_cast<uint32_t>(x)));
}

template <typename U,
          typename std::enable_if<sizeof(U) == 8 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return static_cast<U>(__builtin_bswap64(static_cast<uint64_t>(x)));
}
#else
// Non-GCC/Clang: delegate to portable
template <typename U, typename std::enable_if<(sizeof(U) >= 2 && sizeof(U) <= 8) && detail::is_unsigned_ex<U>::value,
                                              std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return bswap_portable(x);
}
#endif

#if defined(__SIZEOF_INT128__)
template <typename U,
          typename std::enable_if<sizeof(U) == 16 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_portable(U x) noexcept
{
    return ((static_cast<U>(bswap_portable<uint64_t>(static_cast<uint64_t>(x))) << 64) |
            static_cast<U>(bswap_portable<uint64_t>(static_cast<uint64_t>(x >> 64))));
}

// Detect __builtin_bswap128 availability
#if defined(__has_builtin)
#if __has_builtin(__builtin_bswap128)
#define M5_HAS_BUILTIN_BSWAP128 1
#endif
#elif defined(__GNUC__) && (__GNUC__ >= 13)
#define M5_HAS_BUILTIN_BSWAP128 1
#endif

template <typename U,
          typename std::enable_if<sizeof(U) == 16 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
#if defined(M5_HAS_BUILTIN_BSWAP128)
    return static_cast<U>(__builtin_bswap128(static_cast<unsigned __int128>(x)));
#else
    return ((static_cast<U>(bswap_fixed<uint64_t>(static_cast<uint64_t>(x))) << 64) |
            static_cast<U>(bswap_fixed<uint64_t>(static_cast<uint64_t>(x >> 64))));
#endif
}
#endif

template <typename U,
          typename std::enable_if<sizeof(U) == 1 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_portable(U x) noexcept
{
    return x;
}

template <typename U,
          typename std::enable_if<sizeof(U) == 1 && detail::is_unsigned_ex<U>::value, std::nullptr_t>::type = nullptr>
inline constexpr U bswap_fixed(U x) noexcept
{
    return x;
}

template <typename T, typename std::enable_if<detail::is_integer_or_enum<T>::value, std::nullptr_t>::type = nullptr>
inline constexpr T byteswap_constexpr(T v) noexcept
{
    using U = typename detail::make_unsigned_ex<T>::type;
    return static_cast<T>(bswap_fixed<U>(static_cast<U>(v)));
}
///@endcond

//! @brief byteswap for integral type
template <typename T, typename std::enable_if<detail::is_integer_or_enum<T>::value, std::nullptr_t>::type = nullptr>
inline constexpr T byteswap(T v) noexcept
{
    return byteswap_constexpr(v);
}

//! @brief byteswap using portable bit manipulation (always available, for testing both paths)
template <typename T, typename std::enable_if<detail::is_integer_or_enum<T>::value, std::nullptr_t>::type = nullptr>
inline constexpr T byteswap_with_portable(T v) noexcept
{
    using U = typename detail::make_unsigned_ex<T>::type;
    return static_cast<T>(bswap_portable<U>(static_cast<U>(v)));
}

// If the type is not integral , it is prohibited
template <typename T, typename std::enable_if<!detail::is_integer_or_enum<T>::value, std::nullptr_t>::type = nullptr>
T byteswap(T) = delete;

}  // namespace stl
}  // namespace m5

#endif
