/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file base64.cpp
  @brief base64 encoding
*/
#include "base64.hpp"
#include <M5Utility.h>

namespace {
constexpr char b64_table_std[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr char b64_table_url[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

inline bool is_ignored_b64_char(uint8_t ch)
{
    return (ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t');
}

inline int8_t b64_value(const uint8_t ch, const bool urlEncode)
{
    if ('A' <= ch && ch <= 'Z') return (int8_t)(ch - 'A');
    if ('a' <= ch && ch <= 'z') return (int8_t)(26 + (ch - 'a'));
    if ('0' <= ch && ch <= '9') return (int8_t)(52 + (ch - '0'));
    if (urlEncode) {
        if (ch == '-') return 62;
        if (ch == '_') return 63;
    } else {
        if (ch == '+') return 62;
        if (ch == '/') return 63;
    }
    return -1;
}

}  // namespace

namespace m5 {
namespace utility {

uint32_t encode_base64(char* out, const uint32_t olen, const uint8_t* buf, const uint32_t blen, const uint8_t line_len,
                       const bool urlEncode, const bool padding)
{
    const char* b64_table = urlEncode ? b64_table_url : b64_table_std;

    if (!out || !olen || !buf || !blen) return 0;

    uint32_t written  = 0;
    uint32_t line_pos = 0;

    for (uint32_t i = 0; i < blen; i += 3) {
        uint32_t val    = (buf[i] << 16);
        bool has_second = (i + 1 < blen);
        bool has_third  = (i + 2 < blen);
        if (has_second) {
            val |= (buf[i + 1] << 8);
        }
        if (has_third) {
            val |= buf[i + 2];
        }

        auto emit = [&](char c) {
            if (written + 1 >= olen) {
                return false;
            }
            out[written++] = c;
            if (line_len && ++line_pos == line_len) {
                if (written + 1 >= olen) {
                    return false;
                }
                out[written++] = '\n';
                line_pos       = 0;
            }
            return true;
        };

        if (!emit(b64_table[(val >> 18) & 0x3F]) || !emit(b64_table[(val >> 12) & 0x3F])) {
            return 0;
        }
        if (has_second) {
            if (!emit(b64_table[(val >> 6) & 0x3F])) {
                return 0;
            }
        } else if (padding) {
            if (!emit('=')) {
                return 0;
            }
        }
        if (has_third) {
            if (!emit(b64_table[val & 0x3F])) {
                return 0;
            }
        } else if (padding) {
            if (!emit('=')) {
                return 0;
            }
        }
    }

    if (written > 0 && out[written - 1] == '\n') {
        --written;
    }
    if (written < olen) {
        out[written] = '\0';
    }

    return written;
}

uint32_t decode_base64(uint8_t* out, const uint32_t olen, const char* buf, const uint32_t blen, const bool urlEncode,
                       const bool padding)
{
    if (!out || !olen || !buf || !blen) return 0;

    uint32_t written = 0;

    uint8_t q[4];
    uint32_t qlen{};

    auto flush_quad = [&](const uint8_t quad[4]) -> bool {
        int pad = 0;
        if (quad[2] == '=') pad++;
        if (quad[3] == '=') pad++;

        int8_t v0 = b64_value(quad[0], urlEncode);
        int8_t v1 = b64_value(quad[1], urlEncode);
        if (v0 < 0 || v1 < 0) return false;

        int8_t v2 = (quad[2] == '=') ? 0 : b64_value(quad[2], urlEncode);
        int8_t v3 = (quad[3] == '=') ? 0 : b64_value(quad[3], urlEncode);
        if (v2 < 0 || v3 < 0) return false;

        uint32_t val = (static_cast<uint32_t>(v0) << 18) | (static_cast<uint32_t>(v1) << 12) |
                       (static_cast<uint32_t>(v2) << 6) | static_cast<uint32_t>(v3);

        if (written + 1 > olen) return false;
        out[written++] = static_cast<uint8_t>((val >> 16) & 0xFF);

        if (pad < 2) {
            if (written + 1 > olen) return false;
            out[written++] = static_cast<uint8_t>((val >> 8) & 0xFF);
        }
        if (pad < 1) {
            if (written + 1 > olen) return false;
            out[written++] = static_cast<uint8_t>(val & 0xFF);
        }
        return true;
    };

    for (uint32_t i = 0; i < blen; ++i) {
        uint8_t c = static_cast<uint8_t>(buf[i]);

        if (is_ignored_b64_char(c)) continue;

        if (c == '=') {
            if (!padding) return 0;
        } else {
            if (b64_value(c, urlEncode) < 0) return 0;
        }

        q[qlen++] = c;

        if (qlen == 4) {
            if (!flush_quad(q)) return 0;
            qlen = 0;
        }
    }

    if (qlen != 0) {
        if (padding) {
            return 0;
        }
        if (qlen == 1) return 0;
        for (; qlen < 4; ++qlen) q[qlen] = '=';
        if (!flush_quad(q)) return 0;
    }

    return written;
}

}  // namespace utility
}  // namespace m5
