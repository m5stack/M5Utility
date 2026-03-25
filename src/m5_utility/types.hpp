/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file types.hpp
  @brief Endian-aware integer types for safe register access

  @par Motivation
  Casting raw byte buffers to integer pointers causes undefined behavior:
  @code
  // UB: alignment violation + wrong endianness
  uint16_t val = *(uint16_t*)buf;
  @endcode

  @par Safe usage with endian-aware types
  @code
  // I2C sensor register (big-endian)
  big_uint16_t reg;
  memcpy(reg.data(), buf, 2);
  uint16_t val = reg.get();  // correct endian conversion
  @endcode
*/
#ifndef M5_UTILITY_TYPES_HPP
#define M5_UTILITY_TYPES_HPP

#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>
#include "stl/endianness.hpp"
#include "stl/byteswap.hpp"

namespace m5 {
namespace types {

/*!
  @struct EndianInt
  @tparam T Underlying integer type (uint16_t, int16_t, uint32_t, int32_t, etc.)
  @tparam DataIsLittle Data endianness<br> true: Little false: Big
  @brief Endian-compliant integer type
*/
template <typename T, bool DataIsLittle>
union EndianInt {
    static_assert(m5::stl::detail::is_integer_or_enum<T>::value, "T must be an integral type");

    /// @name Constructor
    ///@{
    /*! @brief default constructor */
    constexpr EndianInt() : value{0}
    {
    }
    //! @brief from T
    template <bool HostIsLittle = m5::endian::little>
    explicit EndianInt(const T v)
    {
        set<HostIsLittle>(v);
    }
    //! @brief Stored in order of bytes (2-byte types)
    template <typename U = T, typename std::enable_if<sizeof(U) == 2, int>::type = 0>
    constexpr EndianInt(const uint8_t b0, const uint8_t b1) : u8{b0, b1}
    {
    }
    //! @brief Stored in order of bytes (4-byte types)
    template <typename U = T, typename std::enable_if<sizeof(U) == 4, int>::type = 0>
    constexpr EndianInt(const uint8_t b0, const uint8_t b1, const uint8_t b2, const uint8_t b3) : u8{b0, b1, b2, b3}
    {
    }
    //! @brief Stored in order of bytes (8-byte types)
    template <typename U = T, typename std::enable_if<sizeof(U) == 8, int>::type = 0>
    constexpr EndianInt(const uint8_t b0, const uint8_t b1, const uint8_t b2, const uint8_t b3, const uint8_t b4,
                        const uint8_t b5, const uint8_t b6, const uint8_t b7)
        : u8{b0, b1, b2, b3, b4, b5, b6, b7}
    {
    }

    constexpr EndianInt(const EndianInt&) = default;

    constexpr EndianInt(EndianInt&& o) noexcept = default;
    ///@}

    ///@name Assignment
    ///@{
    EndianInt& operator=(const EndianInt&) = default;

    EndianInt& operator=(EndianInt&&) noexcept = default;

    template <bool HostIsLittle = m5::endian::little>
    EndianInt& operator=(const T v)
    {
        set<HostIsLittle>(v);
        return *this;
    }
    template <typename H, typename L, typename U = T, typename std::enable_if<sizeof(U) == 2, int>::type = 0>
    EndianInt& operator=(const std::pair<H, L>& o)
    {
        static_assert(std::is_integral<H>::value && std::is_integral<L>::value, "HIGH & LOW Must be integral");
        u8[0] = static_cast<uint8_t>(o.first);
        u8[1] = static_cast<uint8_t>(o.second);
        return *this;
    }
    ///@}

    ///@name Cast
    ///@{
    /*! @brief To boolean */
    inline explicit operator bool() const
    {
        return value != 0;
    }
    /*! @brief To const uint8_t* */
    inline explicit operator const uint8_t*() const
    {
        return data();
    }
    /*! @brief To uint8_t* */
    inline explicit operator uint8_t*()
    {
        return data();
    }
    //! @brief To T on processor endianness
    inline explicit operator T() const
    {
        return get();
    }
    ///@}

    /*!
      @brief Set value with specified endianness
      @tparam HostIsLittle Host endianness (default as processor endianness)
    */
    template <bool HostIsLittle = m5::endian::little>
    inline void set(const T v)
    {
        if (DataIsLittle == HostIsLittle) {
            value = v;
        } else {
            value = m5::stl::byteswap(v);
        }
    }
    /*!
      @brief Gets value with specified endianness
      @tparam HostIsLittle Host endianness (default as processor endianness)
     */
    template <bool HostIsLittle = m5::endian::little>
    inline T get() const
    {
        if (DataIsLittle == HostIsLittle) {
            return value;
        }
        return m5::stl::byteswap(value);
    }
    //! @brief Gets the high byte (u8[0])
    inline uint8_t high() const
    {
        return u8[0];
    }
    //! @brief Gets the low byte (u8[sizeof(T)-1])
    inline uint8_t low() const
    {
        return u8[sizeof(T) - 1];
    }
    //! @brief Gets the const pointer
    inline const uint8_t* data() const
    {
        return u8;
    }
    //! @brief Gets the pointer
    inline uint8_t* data()
    {
        return u8;
    }
    //! @brief Gets size in uint8_t units.
    inline size_t size() const
    {
        return sizeof(T);
    }

    T value{};              //!< @brief Raw value
    uint8_t u8[sizeof(T)];  //!< @brief Raw value according to uint8_t
};

// 16-bit type aliases
using big_uint16_t    = EndianInt<uint16_t, false>;
using little_uint16_t = EndianInt<uint16_t, true>;
using big_int16_t     = EndianInt<int16_t, false>;
using little_int16_t  = EndianInt<int16_t, true>;

// 32-bit type aliases
using big_uint32_t    = EndianInt<uint32_t, false>;
using little_uint32_t = EndianInt<uint32_t, true>;
using big_int32_t     = EndianInt<int32_t, false>;
using little_int32_t  = EndianInt<int32_t, true>;

// 64-bit type aliases
using big_uint64_t    = EndianInt<uint64_t, false>;
using little_uint64_t = EndianInt<uint64_t, true>;
using big_int64_t     = EndianInt<int64_t, false>;
using little_int64_t  = EndianInt<int64_t, true>;

#if defined(__SIZEOF_INT128__)
// 128-bit type aliases (available when __int128 is supported)
using big_uint128_t    = EndianInt<unsigned __int128, false>;
using little_uint128_t = EndianInt<unsigned __int128, true>;
using big_int128_t     = EndianInt<__int128, false>;
using little_int128_t  = EndianInt<__int128, true>;
#endif

///@name Compare
/// @related m5::types::EndianInt
///@{
template <typename T, bool DL1, bool DL2>
inline bool operator==(const EndianInt<T, DL1>& a, const EndianInt<T, DL2>& b)
{
    return a.get() == b.get();
}
template <typename T, bool DL1, bool DL2>
inline bool operator!=(const EndianInt<T, DL1>& a, const EndianInt<T, DL2>& b)
{
    return !(a == b);
}
template <typename T, bool DL1, bool DL2>
inline bool operator<(const EndianInt<T, DL1>& a, const EndianInt<T, DL2>& b)
{
    return a.get() < b.get();
}
template <typename T, bool DL1, bool DL2>
inline bool operator>(const EndianInt<T, DL1>& a, const EndianInt<T, DL2>& b)
{
    return b < a;
}
template <typename T, bool DL1, bool DL2>
inline bool operator<=(const EndianInt<T, DL1>& a, const EndianInt<T, DL2>& b)
{
    return !(a > b);
}
template <typename T, bool DL1, bool DL2>
inline bool operator>=(const EndianInt<T, DL1>& a, const EndianInt<T, DL2>& b)
{
    return !(a < b);
}
///@}

}  // namespace types
}  // namespace m5
#endif
