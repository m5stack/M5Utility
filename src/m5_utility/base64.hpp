/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file base64.hpp
  @brief base64 encoding
*/
#ifndef M5_UNIT_CRYPTO_UTILITY_BASE64_HPP
#define M5_UNIT_CRYPTO_UTILITY_BASE64_HPP

#include <cstdint>

namespace m5 {
namespace utility {

/*!
  @brief encode Base64
  @param[out] out Output buffer
  @param olen Output buffer length
  @param buf Input buffer
  @param blen Input buffer length
  @param line_len ==0 No line breaks ! =0: Line break at that number of characters
  @param urlEncode base64url encoding if true
  @param padding Enable padding if true
  @return Encoded length
 */
uint32_t encode_base64(char* out, const uint32_t olen, const uint8_t* buf, const uint32_t blen, const uint8_t line_len,
                       const bool urlEncode, const bool padding);

//! @brief Encode Base64(PEM)
inline bool encodeBase64(char* out, const uint32_t olen, const uint8_t* buf, const uint32_t blen)
{
    return encode_base64(out, olen, buf, blen, 64, false, true);
}
//! @brief Encode Base64URL
inline bool encodeBase64URL(char* out, const uint32_t olen, const uint8_t* buf, const uint32_t blen)
{
    return encode_base64(out, olen, buf, blen, 0, true, false);
}

}  // namespace utility
}  // namespace m5
#endif
