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

}  // namespace utility
}  // namespace m5
