/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file sha1.hpp
  @brief Secure Hash Algorithm (SHA-1) class
*/
#ifndef M5_UTILITY_SHA1_HPP
#define M5_UTILITY_SHA1_HPP

#include <cstdint>
#include <cstring>

namespace m5 {
namespace utility {

/*!
  @class m5::utility::SHA1
  @brief Calculate SHA-1 (RFC3174)
  @warning Only byte-by-byte input is accepted
 */
class SHA1 {
public:
    SHA1()
    {
    }

    void init()
    {
        _state[0]  = 0x67452301;
        _state[1]  = 0xEFCDAB89;
        _state[2]  = 0x98BADCFE;
        _state[3]  = 0x10325476;
        _state[4]  = 0xC3D2E1F0;
        _count     = 0;
        _bufferLen = 0;
        memset(_buffer, 0, sizeof(_buffer));
    }

    void update(const uint8_t* data, size_t len)
    {
        _count += static_cast<uint64_t>(len) << 3;
        size_t fill{64 - _bufferLen};

        if (len >= fill) {
            memcpy(_buffer + _bufferLen, data, fill);
            transform(_state, _buffer);
            data += fill;
            len -= fill;
            _bufferLen = 0;

            while (len >= 64) {
                transform(_state, data);
                data += 64;
                len -= 64;
            }
        }

        memcpy(_buffer + _bufferLen, data, len);
        _bufferLen += len;
    }

    void finalize(uint8_t digest[20])
    {
        uint8_t padding[64]{0x80};
        uint8_t length[8]{};
        uint64_t count_be = __builtin_bswap64(_count);
        memcpy(length, &count_be, 8);

        size_t padLen = (_bufferLen < 56) ? (56 - _bufferLen) : (120 - _bufferLen);
        update(padding, padLen);
        update(length, 8);

        for (int i = 0; i < 5; ++i) {
            uint32_t be = __builtin_bswap32(_state[i]);
            memcpy(digest + i * 4, &be, 4);
        }
    }

    static void sha1(uint8_t digest[20], const uint8_t* data, size_t len)
    {
        SHA1 ctx;
        ctx.update(data, len);
        ctx.finalize(digest);
    }

private:
    void transform(uint32_t state[5], const uint8_t buffer[64])
    {
        uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
        uint32_t block[80];

        for (int i = 0; i < 16; ++i) {
            block[i] = (uint32_t(buffer[i * 4 + 0]) << 24) | (uint32_t(buffer[i * 4 + 1]) << 16) |
                       (uint32_t(buffer[i * 4 + 2]) << 8) | (uint32_t(buffer[i * 4 + 3]));
        }
        for (int i = 16; i < 80; ++i) {
            block[i] = rol(block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16], 1);
        }

        for (int i = 0; i < 80; ++i) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = rol(a, 5) + f + e + k + block[i];
            e             = d;
            d             = c;
            c             = rol(b, 30);
            b             = a;
            a             = temp;
        }

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
    }

    inline uint32_t rol(uint32_t value, uint32_t bits)
    {
        return (value << bits) | (value >> (32 - bits));
    }

    uint32_t _state[5]{0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    uint64_t _count{};
    uint8_t _buffer[64]{};
    size_t _bufferLen{};
};

}  // namespace utility
}  // namespace m5
#endif
