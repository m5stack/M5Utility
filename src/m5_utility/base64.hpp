/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file base64.hpp
  @brief base64 encoding/decoding
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
inline uint32_t encodeBase64(char* out, const uint32_t olen, const uint8_t* buf, const uint32_t blen)
{
    return encode_base64(out, olen, buf, blen, 64, false, true);
}
//! @brief Encode Base64URL
inline uint32_t encodeBase64URL(char* out, const uint32_t olen, const uint8_t* buf, const uint32_t blen)
{
    return encode_base64(out, olen, buf, blen, 0, true, false);
}

/*!
  @brief decode Base64 / Base64URL
  @param[out] out Output buffer (decoded bytes)
  @param olen Output buffer length
  @param buf Input buffer (base64 text; may include CR/LF/spaces)
  @param blen Input buffer length
  @param urlEncode base64url decoding if true
  @param padding Expect/allow padding if true
  @return Decoded length
  @note padding  true: Input must contain ‘=’ padding (RFC 4648 standard)
  @note padding false: Input must not contain ‘=’. Incomplete groups are auto-completed
*/
uint32_t decode_base64(uint8_t* out, const uint32_t olen, const char* buf, const uint32_t blen, const bool urlEncode,
                       const bool padding);

//! @brief Decode Base64(PEM)
inline uint32_t decodeBase64(uint8_t* out, const uint32_t olen, const char* buf, const uint32_t blen)
{
    return decode_base64(out, olen, buf, blen, false, true);
}

//! @brief Decode Base64URL
inline uint32_t decodeBase64URL(uint8_t* out, const uint32_t olen, const char* buf, const uint32_t blen)
{
    return decode_base64(out, olen, buf, blen, true, false);
}

}  // namespace utility
}  // namespace m5
#endif
