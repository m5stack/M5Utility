/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for TripleDES
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>
#include <M5Unified.hpp>

using m5::utility::crypto::TripleDES;

namespace {

using Mode    = TripleDES::Mode;
using Padding = TripleDES::Padding;
using Key8    = TripleDES::Key8;
using Key16   = TripleDES::Key16;
using Key24   = TripleDES::Key24;

std::vector<uint8_t> hex_to_bytes(const std::string& hex)
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

template <size_t N>
std::array<uint8_t, N> hex_to_array(const std::string& hex)
{
    auto v = hex_to_bytes(hex);
    EXPECT_EQ(v.size(), N);
    std::array<uint8_t, N> a{};
    std::copy(v.begin(), v.end(), a.begin());
    return a;
}

template <typename KeyT>
void roundtrip(Mode mode, Padding pad, const std::vector<uint8_t>& plain, const KeyT& key,
               const uint8_t iv[8] = nullptr)
{
    TripleDES des(mode, pad, iv);

    std::vector<uint8_t> enc(plain.size() + 16 /* for padding */);
    std::vector<uint8_t> dec(enc.size());

    uint32_t enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_GT(enc_len, 0U);
    EXPECT_LE(enc_len, enc.size());

    uint32_t dec_len = des.decrypt(dec.data(), enc.data(), enc_len, key);

    if (pad == Padding::Zero) {
        EXPECT_GE(dec_len, plain.size());
        EXPECT_EQ(dec_len % 8U, 0U);
        EXPECT_TRUE(std::equal(plain.begin(), plain.end(), dec.begin()));
    } else {
        EXPECT_EQ(dec_len, plain.size());
        dec.resize(dec_len);
        EXPECT_EQ(dec, plain);
    }
}

}  // namespace

// ---------------------- ECB / Padding None ----------------------
// 1-key / ECB / None
TEST(Crypto_DES, ECB_None_1)
{
    const auto key         = hex_to_array<8>("133457799BBCDFF1");
    const auto pt          = hex_to_bytes("0123456789ABCDEF");
    const auto ct_expected = hex_to_bytes("85E813540F0AB405");

    TripleDES des(Mode::ECB, Padding::None);

    uint8_t enc[8]{};
    uint8_t dec[8]{};

    uint32_t enc_len = des.encrypt(enc, pt.data(), static_cast<uint32_t>(pt.size()), key);
    EXPECT_EQ(enc_len, 8U);
    EXPECT_TRUE(std::equal(enc, enc + 8, ct_expected.begin()));

    uint32_t dec_len = des.decrypt(dec, enc, enc_len, key);
    EXPECT_EQ(dec_len, 8U);
    EXPECT_TRUE(std::equal(dec, dec + 8, pt.begin()));
}

// 2-key / ECB / None
TEST(Crypto_DES, ECB_None_2)
{
    const auto key             = hex_to_array<16>("0123456789ABCDEF23456789ABCDEF01");
    std::vector<uint8_t> plain = hex_to_bytes("00112233445566778899AABBCCDDEEFF");

    roundtrip(Mode::ECB, Padding::None, plain, key);
}

// 3-key / ECB / None
TEST(Crypto_DES, ECB_None_3)
{
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");

    const auto plain = hex_to_bytes(
        "5468652071756663"  // "The quic" typo HEX
        "6B2062726F776E20"  // "k brown "
        "666F78206A756D70"  // "fox jump"
    );
    const auto ct_expected = hex_to_bytes(
        "A826FD8CE53B855F"
        "CCE21C8112256FE6"
        "68D5C05DD9B6B900");

    TripleDES des(Mode::ECB, Padding::None);

    std::vector<uint8_t> enc(plain.size());
    uint32_t enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_EQ(enc_len, plain.size());
    EXPECT_EQ(enc, ct_expected);

    std::vector<uint8_t> dec(enc.size());
    uint32_t dec_len = des.decrypt(dec.data(), enc.data(), enc_len, key);
    EXPECT_EQ(dec_len, plain.size());
    dec.resize(dec_len);
    EXPECT_EQ(dec, plain);
}

#if 1

// ---------------------- ECB / Padding Zero ----------------------
TEST(Crypto_DES, ECB_Zero_1)
{
    const auto key             = hex_to_array<8>("133457799BBCDFF1");
    std::vector<uint8_t> plain = {'D', 'E', 'S', '-', 'Z', 'E', 'R'};

    roundtrip(Mode::ECB, Padding::Zero, plain, key);
}

TEST(Crypto_DES, ECB_Zero_2)
{
    const auto key             = hex_to_array<16>("00112233445566778899AABBCCDDEEFF");
    std::vector<uint8_t> plain = hex_to_bytes("0102030405060708090A0B0C");

    roundtrip(Mode::ECB, Padding::Zero, plain, key);
}

TEST(Crypto_DES, ECB_Zero_3)
{
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "1111111111111111"
        "2222222222222222");
    std::vector<uint8_t> plain = {'3', 'D', 'E', 'S', ' ', 'Z', 'e', 'r', 'o'};

    roundtrip(Mode::ECB, Padding::Zero, plain, key);
}

// ---------------------- ECB / Padding PKCS7 ----------------------

TEST(Crypto_DES, ECB_PKCS7_1)
{
    const auto key             = hex_to_array<8>("133457799BBCDFF1");
    std::vector<uint8_t> plain = {'P', 'K', 'C', 'S', '7', ' ', 't', 'e', 's', 't'};

    roundtrip(Mode::ECB, Padding::PKCS7, plain, key);
}

TEST(Crypto_DES, ECB_PKCS7_2)
{
    const auto key             = hex_to_array<16>("00112233445566778899AABBCCDDEEFF");
    std::vector<uint8_t> plain = hex_to_bytes("000102030405060708");

    roundtrip(Mode::ECB, Padding::PKCS7, plain, key);
}

TEST(Crypto_DES, ECB_PKCS7_3)
{
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");
    std::vector<uint8_t> plain(17);
    for (size_t i = 0; i < plain.size(); ++i) plain[i] = static_cast<uint8_t>(i);

    roundtrip(Mode::ECB, Padding::PKCS7, plain, key);
}

// ---------------------- CBC / Padding None ----------------------

TEST(Crypto_DES, CBC_None_1)
{
    const auto key      = hex_to_array<8>("133457799BBCDFF1");
    const uint8_t iv[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

    // 2ブロック分
    std::vector<uint8_t> plain = hex_to_bytes("0123456789ABCDEF0123456789ABCDEF");

    roundtrip(Mode::CBC, Padding::None, plain, key, iv);
}

TEST(Crypto_DES, CBC_None_2)
{
    const auto key      = hex_to_array<16>("00112233445566778899AABBCCDDEEFF");
    const uint8_t iv[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    std::vector<uint8_t> plain = hex_to_bytes("FFEEDDCCBBAA99887766554433221100");

    roundtrip(Mode::CBC, Padding::None, plain, key, iv);
}

TEST(Crypto_DES, CBC_None_3)
{
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");
    const uint8_t iv[8] = {9, 8, 7, 6, 5, 4, 3, 2};

    std::vector<uint8_t> plain = hex_to_bytes("000102030405060708090A0B0C0D0E0F");

    roundtrip(Mode::CBC, Padding::None, plain, key, iv);
}

// ---------------------- CBC / Padding Zero ----------------------

TEST(Crypto_DES, CBC_Zero_1)
{
    const auto key      = hex_to_array<8>("133457799BBCDFF1");
    const uint8_t iv[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    std::vector<uint8_t> plain = {'C', 'B', 'C', ' ', 'Z', 'e', 'r', 'o'};

    roundtrip(Mode::CBC, Padding::Zero, plain, key, iv);
}

TEST(Crypto_DES, CBC_Zero_2)
{
    const auto key      = hex_to_array<16>("00112233445566778899AABBCCDDEEFF");
    const uint8_t iv[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11};

    std::vector<uint8_t> plain = hex_to_bytes("AA55AA55AA55AA55AA");

    roundtrip(Mode::CBC, Padding::Zero, plain, key, iv);
}

TEST(Crypto_DES, CBC_Zero_3)
{
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");
    const uint8_t iv[8] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE};

    std::vector<uint8_t> plain = {'3', 'D', 'E', 'S', '-', 'C', 'B', 'C', '-', 'Z'};

    roundtrip(Mode::CBC, Padding::Zero, plain, key, iv);
}

// ---------------------- CBC / Padding PKCS7 ----------------------

TEST(Crypto_DES, CBC_PKCS7_1)
{
    const auto key      = hex_to_array<8>("133457799BBCDFF1");
    const uint8_t iv[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    std::vector<uint8_t> plain = {'P', 'K', 'C', 'S', '7', '-', 'C', 'B', 'C'};

    roundtrip(Mode::CBC, Padding::PKCS7, plain, key, iv);
}

TEST(Crypto_DES, CBC_PKCS7_2)
{
    const auto key      = hex_to_array<16>("00112233445566778899AABBCCDDEEFF");
    const uint8_t iv[8] = {2, 3, 5, 7, 11, 13, 17, 19};

    std::vector<uint8_t> plain = hex_to_bytes("00010203040506070809");

    roundtrip(Mode::CBC, Padding::PKCS7, plain, key, iv);
}

TEST(Crypto_DES, CBC_PKCS7_3)
{
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");
    const uint8_t iv[8] = {0, 0, 0, 0, 0, 0, 0, 1};

    std::vector<uint8_t> plain(15);
    for (size_t i = 0; i < plain.size(); ++i) plain[i] = static_cast<uint8_t>(i + 1);

    roundtrip(Mode::CBC, Padding::PKCS7, plain, key, iv);
}
#endif
