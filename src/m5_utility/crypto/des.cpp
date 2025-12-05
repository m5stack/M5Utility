/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file des.hpp
  @brief Data Encryption Standar (DES)
*/
#include "des.hpp"
#include <cstring>

namespace {
constexpr uint32_t BS{8};  // block size

// Initial Permutation
static constexpr uint8_t IP[64] = {58, 50, 42, 34, 26, 18, 10, 2,  //
                                   60, 52, 44, 36, 28, 20, 12, 4,  //
                                   62, 54, 46, 38, 30, 22, 14, 6,  //
                                   64, 56, 48, 40, 32, 24, 16, 8,  //
                                   57, 49, 41, 33, 25, 17, 9,  1,  //
                                   59, 51, 43, 35, 27, 19, 11, 3,  //
                                   61, 53, 45, 37, 29, 21, 13, 5,  //
                                   63, 55, 47, 39, 31, 23, 15, 7};

// Final Permutation
static constexpr uint8_t FP[64] = {40, 8, 48, 16, 56, 24, 64, 32,  //
                                   39, 7, 47, 15, 55, 23, 63, 31,  //
                                   38, 6, 46, 14, 54, 22, 62, 30,  //
                                   37, 5, 45, 13, 53, 21, 61, 29,  //
                                   36, 4, 44, 12, 52, 20, 60, 28,  //
                                   35, 3, 43, 11, 51, 19, 59, 27,  //
                                   34, 2, 42, 10, 50, 18, 58, 26,  //
                                   33, 1, 41, 9,  49, 17, 57, 25};

// Expansion Permutation
static constexpr uint8_t E[48] = {32, 1,  2,  3,  4,  5,   //
                                  4,  5,  6,  7,  8,  9,   //
                                  8,  9,  10, 11, 12, 13,  //
                                  12, 13, 14, 15, 16, 17,  //
                                  16, 17, 18, 19, 20, 21,  //
                                  20, 21, 22, 23, 24, 25,  //
                                  24, 25, 26, 27, 28, 29,  //
                                  28, 29, 30, 31, 32, 1};

// Permutation
static constexpr uint8_t P[32] = {
    16, 7,  20, 21,  //
    29, 12, 28, 17,  //
    1,  15, 23, 26,  //
    5,  18, 31, 10,  //
    2,  8,  24, 14,  //
    32, 27, 3,  9,   //
    19, 13, 30, 6,   //
    22, 11, 4,  25   //
};

// Permuted Choice 1: 64bit key -> 56bit
static constexpr uint8_t PC1[56] = {57, 49, 41, 33, 25, 17, 9,   //
                                    1,  58, 50, 42, 34, 26, 18,  //
                                    10, 2,  59, 51, 43, 35, 27,  //
                                    19, 11, 3,  60, 52, 44, 36,  //
                                    63, 55, 47, 39, 31, 23, 15,  //
                                    7,  62, 54, 46, 38, 30, 22,  //
                                    14, 6,  61, 53, 45, 37, 29,  //
                                    21, 13, 5,  28, 20, 12, 4};

// PC-2 – Permuted Choice 2: 56bit -> 48bit subkey
static constexpr uint8_t PC2[48] = {
    14, 17, 11, 24, 1,  5,   //
    3,  28, 15, 6,  21, 10,  //
    23, 19, 12, 4,  26, 8,   //
    16, 7,  27, 20, 13, 2,   //
    41, 52, 31, 37, 47, 55,  //
    30, 40, 51, 45, 33, 48,  //
    44, 49, 39, 56, 34, 53,  //
    46, 42, 50, 36, 29, 32   //
};

// Shift table
static constexpr uint8_t SHIFTS[16] = {1, 1, 2, 2,  //
                                       2, 2, 2, 2,  //
                                       1, 2, 2, 2,  //
                                       2, 2, 2, 1};

// Substitution Box
static constexpr uint8_t SBOX[8][64] = {
    // S1
    {14, 4,  13, 1, 2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0, 7,  //
     0,  15, 7,  4, 14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3, 8,  //
     4,  1,  14, 8, 13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5, 0,  //
     15, 12, 8,  2, 4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6, 13},
    // S2
    {15, 1,  8,  14, 6,  11, 3,  4,  9,  7, 2,  13, 12, 0, 5,  10,  //
     3,  13, 4,  7,  15, 2,  8,  14, 12, 0, 1,  10, 6,  9, 11, 5,   //
     0,  14, 7,  11, 10, 4,  13, 1,  5,  8, 12, 6,  9,  3, 2,  15,  //
     13, 8,  10, 1,  3,  15, 4,  2,  11, 6, 7,  12, 0,  5, 14, 9},
    // S3
    {10, 0,  9,  14, 6, 3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8,  //
     13, 7,  0,  9,  3, 4,  6,  10, 2,  8,  5,  14, 12, 11, 15, 1,  //
     13, 6,  4,  9,  8, 15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7,  //
     1,  10, 13, 0,  6, 9,  8,  7,  4,  15, 14, 3,  11, 5,  2,  12},
    // S4
    {7,  13, 14, 3, 0,  6,  9,  10, 1,  2, 8, 5,  11, 12, 4,  15,  //
     13, 8,  11, 5, 6,  15, 0,  3,  4,  7, 2, 12, 1,  10, 14, 9,   //
     10, 6,  9,  0, 12, 11, 7,  13, 15, 1, 3, 14, 5,  2,  8,  4,   //
     3,  15, 0,  6, 10, 1,  13, 8,  9,  4, 5, 11, 12, 7,  2,  14},
    // S5
    {2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0, 14, 9,   //
     14, 11, 2,  12, 4,  7,  13, 1,  5,  0,  15, 10, 3,  9, 8,  6,   //
     4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3, 0,  14,  //
     11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3},
    // S6
    {12, 1,  10, 15, 9, 2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11,  //
     10, 15, 4,  2,  7, 12, 9,  5,  6,  1,  13, 14, 0,  11, 3,  8,   //
     9,  14, 15, 5,  2, 8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6,   //
     4,  3,  2,  12, 9, 5,  15, 10, 11, 14, 1,  7,  6,  0,  8,  13},
    // S7
    {4,  11, 2,  14, 15, 0, 8,  13, 3,  12, 9, 7,  5,  10, 6, 1,  //
     13, 0,  11, 7,  4,  9, 1,  10, 14, 3,  5, 12, 2,  15, 8, 6,  //
     1,  4,  11, 13, 12, 3, 7,  14, 10, 15, 6, 8,  0,  5,  9, 2,  //
     6,  11, 13, 8,  1,  4, 10, 7,  9,  5,  0, 15, 14, 2,  3, 12},
    // S8
    {13, 2,  8,  4, 6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7,  //
     1,  15, 13, 8, 10, 3,  7,  4,  12, 5,  6,  11, 0,  14, 9,  2,  //
     7,  11, 4,  1, 9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8,  //
     2,  1,  14, 7, 4,  10, 8,  13, 15, 12, 9,  0,  3,  5,  6,  11}};

}  // namespace

namespace m5 {
namespace utility {
namespace crypto {

TripleDES::TripleDES(const Mode mode, const Padding pad, const uint8_t iv[8]) : _mode{mode}, _pad{pad}
{
    if (iv) {
        std::memcpy(_iv, iv, 8);
    }
}

uint32_t TripleDES::encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key16& key)
{
    Key k1{}, k2{};
    std::memcpy(k1.data(), key.data(), 8);
    std::memcpy(k2.data(), key.data() + 8, 8);
    return encrypt_3key(out, in, in_len, k1, k2, k1);
}

uint32_t TripleDES::encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key24& key)
{
    Key k1{}, k2{}, k3{};
    std::memcpy(k1.data(), key.data(), 8);
    std::memcpy(k2.data(), key.data() + 8, 8);
    std::memcpy(k3.data(), key.data() + 16, 8);
    return encrypt_3key(out, in, in_len, k1, k2, k3);
}

uint32_t TripleDES::decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key16& key)
{
    Key k1{}, k2{};
    std::memcpy(k1.data(), key.data(), 8);
    std::memcpy(k2.data(), key.data() + 8, 8);
    return decrypt_3key(out, in, in_len, k1, k2, k1);
}

uint32_t TripleDES::decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key24& key)
{
    Key k1{}, k2{}, k3{};
    std::memcpy(k1.data(), key.data(), 8);
    std::memcpy(k2.data(), key.data() + 8, 8);
    std::memcpy(k3.data(), key.data() + 16, 8);
    return decrypt_3key(out, in, in_len, k1, k2, k3);
}

void TripleDES::encrypt_block(uint8_t out[8], const uint8_t in[8], const Key& key)
{
    process_block(out, in, key, true);
}

void TripleDES::decrypt_block(uint8_t out[8], const uint8_t in[8], const Key& key)
{
    process_block(out, in, key, false);
}

void TripleDES::key_schedule(uint64_t subkeys[16], const Key& key)
{
    // 8byte -> 64bit
    uint64_t k64{};
    for (uint_fast8_t i = 0; i < 8; ++i) {
        k64 = (k64 << 8) | key[i];
    }

    // PC1: 64bit -> 56bit (C||D)
    uint64_t k56 = permute_bits(k64, PC1, 56, 64);
    uint32_t c   = static_cast<uint32_t>((k56 >> 28) & 0x0FFFFFFF);
    uint32_t d   = static_cast<uint32_t>(k56 & 0x0FFFFFFF);

    for (uint_fast8_t round = 0; round < 16; ++round) {
        uint8_t shift = SHIFTS[round];
        // left rotate 28bit
        c = ((c << shift) | (c >> (28 - shift))) & 0x0FFFFFFF;
        d = ((d << shift) | (d >> (28 - shift))) & 0x0FFFFFFF;

        uint64_t cd = (static_cast<uint64_t>(c) << 28) | d;
        // PC2: 56bit -> 48bit
        subkeys[round] = permute_bits(cd, PC2, 48, 56);
    }
}

uint32_t TripleDES::feistel(const uint32_t r, const uint64_t subkey)
{
    // 1) E  (32 -> 48)
    uint64_t r48 = permute_bits(r, E, 48, 32);

    // 2) XOR subkey
    r48 ^= subkey;

    // 3) S-box
    uint32_t out32{};
    for (uint_fast8_t i = 0; i < 8; ++i) {
        uint8_t six  = (r48 >> (42 - 6 * i)) & 0x3F;        // 6bits
        uint8_t row  = ((six & 0x20) >> 4) | (six & 0x01);  // bit6 + bit1
        uint8_t col  = (six >> 1) & 0x0F;                   // 4bit
        uint8_t sval = SBOX[i][row * 16 + col];
        out32 |= static_cast<uint32_t>(sval) << (28 - 4 * i);
    }
    // 4) P (32 -> 32)
    return static_cast<uint32_t>(permute_bits(out32, P, 32, 32));
}

uint64_t TripleDES::permute_bits(const uint64_t in, const uint8_t* table, const uint32_t table_len,
                                 const uint32_t in_bits)
{
    uint64_t out{};
    for (uint32_t i = 0; i < table_len; ++i) {
        uint32_t src_pos = in_bits - table[i];  // 0 origin
        uint64_t bit     = (in >> src_pos) & 0x1ULL;
        out |= bit << (table_len - 1 - i);
    }
    return out;
}

void TripleDES::process_block(uint8_t out[8], const uint8_t in[8], const Key& key, const bool encrypt)
{
    uint64_t subkeys[16]{};
    key_schedule(subkeys, key);

    // 8byte -> 64bit
    uint64_t block{};
    for (uint_fast8_t i = 0; i < 8; ++i) {
        block = (block << 8) | in[i];
    }

    // IP
    block = permute_bits(block, IP, 64, 64);

    uint32_t L = static_cast<uint32_t>(block >> 32);
    uint32_t R = static_cast<uint32_t>(block & 0xFFFFFFFFu);

    for (uint_fast8_t round = 0; round < 16; ++round) {
        uint32_t prevL = L;
        uint32_t prevR = R;

        uint8_t idx   = encrypt ? round : (15 - round);
        uint32_t fval = feistel(prevR, subkeys[idx]);
        L             = prevR;
        R             = prevL ^ fval;
    }

    // Finally, swap L and R and combine.
    uint64_t preoutput = (static_cast<uint64_t>(R) << 32) | static_cast<uint64_t>(L);

    // FP
    preoutput = permute_bits(preoutput, FP, 64, 64);

    // 64bit -> 8byte
    for (int_fast8_t i = 7; i >= 0; --i) {
        out[i] = static_cast<uint8_t>(preoutput & 0xFFu);
        preoutput >>= 8;
    }
}

uint32_t TripleDES::encrypt_3key(uint8_t* out, const uint8_t* in, uint32_t in_len, const Key& k1, const Key& k2,
                                 const Key& k3)
{
    uint32_t full = in_len / BS;
    uint32_t rem  = in_len % BS;

    if (_pad == Padding::None && rem != 0) {
        return 0;
    }

    uint32_t out_len{};

    uint8_t prev[BS]{};
    std::memcpy(prev, _iv, BS);

    for (uint32_t i = 0; i < full; ++i) {
        uint8_t block[BS]{};
        std::memcpy(block, in + i * BS, BS);

        if (_mode == Mode::CBC) {
            for (uint32_t j = 0; j < BS; ++j) {
                block[j] ^= prev[j];
            }
        }

        encrypt_block_3key(out + out_len, block, k1, k2, k3);

        if (_mode == Mode::CBC) {
            std::memcpy(prev, out + out_len, BS);
        }

        out_len += BS;
    }

    // Padding
    uint8_t last[BS]{};
    if (_pad == Padding::None) {
        return out_len;
    }

    if (_pad == Padding::Zero) {
        if (rem > 0) {
            std::memcpy(last, in + full * BS, rem);
            std::memset(last + rem, 0, BS - rem);

            if (_mode == Mode::CBC) {
                for (uint32_t j = 0; j < BS; ++j) {
                    last[j] ^= prev[j];
                }
            }

            encrypt_block_3key(out + out_len, last, k1, k2, k3);
            out_len += BS;
        }
    } else if (_pad == Padding::PKCS7) {
        uint32_t pad_len = BS - rem;
        if (pad_len == 0) {
            pad_len = BS;
        }
        if (rem > 0) {
            std::memcpy(last, in + full * BS, rem);
        }
        std::memset(last + rem, pad_len, BS - rem);

        if (_mode == Mode::CBC) {
            for (uint32_t j = 0; j < BS; ++j) {
                last[j] ^= prev[j];
            }
        }

        encrypt_block_3key(out + out_len, last, k1, k2, k3);
        out_len += BS;
    }

    return out_len;
}

void TripleDES::encrypt_block_3key(uint8_t out[8], const uint8_t in[8], const Key8& k1, const Key8& k2, const Key8& k3)
{
    std::uint8_t buf1[8];
    std::uint8_t buf2[8];

    encrypt_block(buf1, in, k1);
    decrypt_block(buf2, buf1, k2);
    encrypt_block(out, buf2, k3);
}

uint32_t TripleDES::decrypt_3key(uint8_t* out, const uint8_t* in, uint32_t in_len, const Key& k1, const Key& k2,
                                 const Key& k3)
{
    if (in_len == 0) {
        return 0;
    }

    if ((in_len % BS) != 0) {
        return 0;
    }

    uint32_t blocks  = in_len / BS;
    uint32_t out_len = in_len;

    uint8_t prev[BS]{};
    std::memcpy(prev, _iv, BS);

    for (uint32_t i = 0; i < blocks; ++i) {
        const uint8_t* cin = in + i * BS;

        uint8_t tmp[BS]{};
        decrypt_block_3key(tmp, cin, k1, k2, k3);

        if (_mode == Mode::CBC) {
            for (uint32_t j = 0; j < BS; ++j) {
                tmp[j] ^= prev[j];
            }
            std::memcpy(prev, cin, BS);
        }
        std::memcpy(out + i * BS, tmp, BS);
    }

    if (_pad == Padding::None) {
        return out_len;
    }

    if (_pad == Padding::Zero) {
        return out_len;
    }

    if (_pad == Padding::PKCS7) {
        if (out_len < BS) {
            return 0;
        }

        uint8_t pad_len = out[out_len - 1];
        if (pad_len == 0 || pad_len > BS) {
            return 0;
        }

        for (uint32_t i = 0; i < pad_len; ++i) {
            if (out[out_len - 1 - i] != pad_len) {
                return 0;
            }
        }

        out_len -= pad_len;
        return out_len;
    }

    return out_len;
}

void TripleDES::decrypt_block_3key(uint8_t out[8], const uint8_t in[8], const Key8& k1, const Key8& k2, const Key8& k3)
{
    std::uint8_t buf1[8];
    std::uint8_t buf2[8];

    decrypt_block(buf1, in, k3);
    encrypt_block(buf2, buf1, k2);
    decrypt_block(out, buf2, k1);
}

}  // namespace crypto
}  // namespace utility
}  // namespace m5
