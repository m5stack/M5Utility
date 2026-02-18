/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for M5Utility
*/
#ifndef TEST_HELPER_HPP
#define TEST_HELPER_HPP

#include <vector>
#include <array>
#include <string>
#include <cstdint>

// catch2 STATIC_REQUIRE
#define STATIC_EXPECT_TRUE(constexpr_cond)                                       \
    do {                                                                         \
        static_assert((constexpr_cond), "Occurrence of compile-time assertion"); \
    } while (0)

namespace test_helper {

//! @brief Convert hex string to byte vector
inline std::vector<uint8_t> hex_to_bytes(const std::string& hex)
{
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        auto h1  = hex[i];
        auto h2  = hex[i + 1];
        auto cvt = [](char c) -> int {
            if ('0' <= c && c <= '9') return c - '0';
            if ('a' <= c && c <= 'f') return 10 + (c - 'a');
            if ('A' <= c && c <= 'F') return 10 + (c - 'A');
            return 0;
        };
        out.push_back(static_cast<uint8_t>((cvt(h1) << 4) | cvt(h2)));
    }
    return out;
}

//! @brief Convert hex string to fixed-size array
template <size_t N>
inline std::array<uint8_t, N> hex_to_array(const std::string& hex)
{
    auto v = hex_to_bytes(hex);
    std::array<uint8_t, N> a{};
    if (v.size() >= N) {
        std::copy(v.begin(), v.begin() + N, a.begin());
    } else {
        std::copy(v.begin(), v.end(), a.begin());
    }
    return a;
}

//! @brief Convert bytes to hex string for debug output
inline std::string bytes_to_hex(const uint8_t* data, size_t len)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    std::string result;
    result.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        result.push_back(hex_chars[(data[i] >> 4) & 0x0F]);
        result.push_back(hex_chars[data[i] & 0x0F]);
    }
    return result;
}

}  // namespace test_helper

#endif
