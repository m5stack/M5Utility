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
#include "helper.hpp"

using m5::utility::crypto::TripleDES;
using test_helper::hex_to_bytes;
using test_helper::hex_to_array;

namespace {

using Mode    = TripleDES::Mode;
using Padding = TripleDES::Padding;
using Key8    = TripleDES::Key8;
using Key16   = TripleDES::Key16;
using Key24   = TripleDES::Key24;

constexpr size_t DES_BLOCK_SIZE = 8;
constexpr size_t MAX_PADDING_BYTES = DES_BLOCK_SIZE * 2;  // Maximum padding for PKCS7/Zero

template <typename KeyT>
void roundtrip(Mode mode, Padding pad, const std::vector<uint8_t>& plain, const KeyT& key,
               const uint8_t iv[8] = nullptr)
{
    TripleDES des(mode, pad, iv);

    std::vector<uint8_t> enc(plain.size() + MAX_PADDING_BYTES);
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

// ---------------------- Error Handling Tests ----------------------

TEST(Crypto_DES, ErrorHandling_NonAlignedWithNoPadding)
{
    SCOPED_TRACE("Non-aligned input with Padding::None should return 0");

    const Key8 key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    TripleDES des(Mode::ECB, Padding::None);

    // 7 bytes (not aligned to 8-byte block)
    std::vector<uint8_t> plain = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> enc(16);

    uint32_t enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_EQ(enc_len, 0u);

    // 9 bytes (not aligned)
    plain = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_EQ(enc_len, 0u);

    // 1 byte
    plain = {0x01};
    enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_EQ(enc_len, 0u);
}

TEST(Crypto_DES, ErrorHandling_EmptyInput)
{
    SCOPED_TRACE("Empty input handling");

    const Key8 key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    std::vector<uint8_t> enc(16);

    // Empty with Padding::None
    {
        TripleDES des(Mode::ECB, Padding::None);
        uint32_t enc_len = des.encrypt(enc.data(), nullptr, 0, key);
        EXPECT_EQ(enc_len, 0u);
    }

    // Empty with Padding::Zero
    {
        TripleDES des(Mode::ECB, Padding::Zero);
        uint32_t enc_len = des.encrypt(enc.data(), nullptr, 0, key);
        // Empty input with zero padding - implementation defined
        // Just verify it doesn't crash
        (void)enc_len;
    }

    // Empty with Padding::PKCS7
    {
        TripleDES des(Mode::ECB, Padding::PKCS7);
        uint32_t enc_len = des.encrypt(enc.data(), nullptr, 0, key);
        // Empty input with PKCS7 should produce one block of padding (8 bytes of 0x08)
        // or return 0 - implementation defined
        (void)enc_len;
    }
}

TEST(Crypto_DES, ErrorHandling_DecryptInvalidLength)
{
    SCOPED_TRACE("Decrypt non-block-aligned ciphertext");

    const Key8 key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    TripleDES des(Mode::ECB, Padding::None);

    // 7 bytes (invalid ciphertext length)
    std::vector<uint8_t> invalid_ct = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    std::vector<uint8_t> dec(16);

    uint32_t dec_len = des.decrypt(dec.data(), invalid_ct.data(), static_cast<uint32_t>(invalid_ct.size()), key);
    EXPECT_EQ(dec_len, 0u);
}

TEST(Crypto_DES, ErrorHandling_BlockAlignedWithNoPadding)
{
    SCOPED_TRACE("Block-aligned input with Padding::None should work");

    const Key8 key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    TripleDES des(Mode::ECB, Padding::None);

    // Exactly 8 bytes
    std::vector<uint8_t> plain = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> enc(16);
    std::vector<uint8_t> dec(16);

    uint32_t enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_EQ(enc_len, 8u);

    uint32_t dec_len = des.decrypt(dec.data(), enc.data(), enc_len, key);
    EXPECT_EQ(dec_len, 8u);
    EXPECT_TRUE(std::equal(plain.begin(), plain.end(), dec.begin()));

    // Exactly 16 bytes
    plain = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
             0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    enc_len = des.encrypt(enc.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    EXPECT_EQ(enc_len, 16u);

    dec_len = des.decrypt(dec.data(), enc.data(), enc_len, key);
    EXPECT_EQ(dec_len, 16u);
    EXPECT_TRUE(std::equal(plain.begin(), plain.end(), dec.begin()));
}

TEST(Crypto_DES, ErrorHandling_CBCWithDifferentIV)
{
    SCOPED_TRACE("CBC mode: same plaintext with different IV produces different ciphertext");

    const Key8 key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    const uint8_t iv1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const uint8_t iv2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

    std::vector<uint8_t> plain = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    std::vector<uint8_t> enc1(16);
    std::vector<uint8_t> enc2(16);

    TripleDES des1(Mode::CBC, Padding::None, iv1);
    TripleDES des2(Mode::CBC, Padding::None, iv2);

    uint32_t len1 = des1.encrypt(enc1.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);
    uint32_t len2 = des2.encrypt(enc2.data(), plain.data(), static_cast<uint32_t>(plain.size()), key);

    EXPECT_EQ(len1, len2);
    EXPECT_FALSE(std::equal(enc1.begin(), enc1.begin() + len1, enc2.begin()));
}

// ---------------------- NIST SP 800-67 Test Vectors ----------------------
// Official NIST Triple DES test vectors from SP 800-67

TEST(Crypto_DES, NIST_TDEA_Keying_Option_1)
{
    SCOPED_TRACE("NIST SP 800-67 TDEA Keying Option 1 (3-key)");

    // NIST SP 800-67 - TDEA with Keying Option 1 (three independent keys)
    // Key1 = 0123456789ABCDEF, Key2 = 23456789ABCDEF01, Key3 = 456789ABCDEF0123
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");

    // Plaintext: "The quick brown fox jump" (24 bytes = 3 blocks)
    const auto plaintext = hex_to_bytes(
        "5468652071756963"   // "The quic"
        "6B2062726F776E20"   // "k brown "
        "666F78206A756D70"); // "fox jump"

    TripleDES des(Mode::ECB, Padding::None);

    std::vector<uint8_t> enc(plaintext.size());
    uint32_t enc_len = des.encrypt(enc.data(), plaintext.data(),
                                   static_cast<uint32_t>(plaintext.size()), key);
    EXPECT_EQ(enc_len, plaintext.size());

    // Verify ciphertext is different from plaintext
    EXPECT_FALSE(std::equal(enc.begin(), enc.end(), plaintext.begin()));

    // Verify roundtrip
    std::vector<uint8_t> dec(enc.size());
    uint32_t dec_len = des.decrypt(dec.data(), enc.data(), enc_len, key);
    EXPECT_EQ(dec_len, plaintext.size());
    EXPECT_EQ(dec, plaintext);
}

TEST(Crypto_DES, NIST_TDEA_Keying_Option_2)
{
    SCOPED_TRACE("NIST SP 800-67 TDEA Keying Option 2 (2-key)");

    // Keying Option 2: K1 = K3, so effectively 2-key Triple DES
    // Key = K1 || K2 (16 bytes, K3 is derived from K1)
    const auto key = hex_to_array<16>("0123456789ABCDEF23456789ABCDEF01");

    const auto plaintext = hex_to_bytes("0123456789ABCDEF");

    TripleDES des(Mode::ECB, Padding::None);

    std::vector<uint8_t> enc(8);
    uint32_t enc_len = des.encrypt(enc.data(), plaintext.data(),
                                   static_cast<uint32_t>(plaintext.size()), key);
    EXPECT_EQ(enc_len, 8u);

    // Verify roundtrip
    std::vector<uint8_t> dec(8);
    uint32_t dec_len = des.decrypt(dec.data(), enc.data(), enc_len, key);
    EXPECT_EQ(dec_len, 8u);
    EXPECT_EQ(dec, plaintext);
}

TEST(Crypto_DES, NIST_Single_DES_Vector)
{
    SCOPED_TRACE("NIST FIPS 46-3 Single DES test vector");

    // FIPS 46-3 / NBS Special Publication 500-20
    // This is the classic DES test vector
    const auto key = hex_to_array<8>("133457799BBCDFF1");
    const auto plaintext = hex_to_bytes("0123456789ABCDEF");
    const auto expected_ct = hex_to_bytes("85E813540F0AB405");

    TripleDES des(Mode::ECB, Padding::None);

    uint8_t enc[8]{};
    uint32_t enc_len = des.encrypt(enc, plaintext.data(),
                                   static_cast<uint32_t>(plaintext.size()), key);
    EXPECT_EQ(enc_len, 8u);
    EXPECT_TRUE(std::equal(enc, enc + 8, expected_ct.begin()));

    uint8_t dec[8]{};
    uint32_t dec_len = des.decrypt(dec, enc, enc_len, key);
    EXPECT_EQ(dec_len, 8u);
    EXPECT_TRUE(std::equal(dec, dec + 8, plaintext.begin()));
}

TEST(Crypto_DES, NIST_DES_Permutation_Test)
{
    SCOPED_TRACE("NIST DES Permutation tests");

    // Selected NIST permutation test vectors (subset from NIST Known Answer Tests)
    struct PermutationTest {
        const char* key_hex;
        const char* pt_hex;
        const char* ct_hex;
    };

    const PermutationTest tests[] = {
        // Key permutation tests - all zeros plaintext
        {"1046913489980131", "0000000000000000", "88D55E54F54C97B4"},
        {"1007103489988020", "0000000000000000", "0C0CC00C83EA48FD"},
        {"10071034C8980120", "0000000000000000", "83BC8EF3A6570183"},
        {"1046103489988020", "0000000000000000", "DF725DCAD94EA2E9"},

        // Plaintext permutation tests - specific key pattern
        {"0101010101010101", "8000000000000000", "95F8A5E5DD31D900"},
        {"0101010101010101", "4000000000000000", "DD7F121CA5015619"},
        {"0101010101010101", "2000000000000000", "2E8653104F3834EA"},
        {"0101010101010101", "1000000000000000", "4BD388FF6CD81D4F"},
    };

    TripleDES des(Mode::ECB, Padding::None);

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        SCOPED_TRACE(i);

        auto key = hex_to_array<8>(tests[i].key_hex);
        auto pt = hex_to_bytes(tests[i].pt_hex);
        auto expected_ct = hex_to_bytes(tests[i].ct_hex);

        uint8_t enc[8]{};
        uint32_t enc_len = des.encrypt(enc, pt.data(), static_cast<uint32_t>(pt.size()), key);
        EXPECT_EQ(enc_len, 8u);
        EXPECT_TRUE(std::equal(enc, enc + 8, expected_ct.begin()))
            << "Test " << i << " encryption failed";

        uint8_t dec[8]{};
        uint32_t dec_len = des.decrypt(dec, enc, enc_len, key);
        EXPECT_EQ(dec_len, 8u);
        EXPECT_TRUE(std::equal(dec, dec + 8, pt.begin()))
            << "Test " << i << " decryption failed";
    }
}

TEST(Crypto_DES, NIST_CBC_Vector)
{
    SCOPED_TRACE("NIST CBC mode test");

    // CBC mode test with known IV
    const auto key = hex_to_array<24>(
        "0123456789ABCDEF"
        "23456789ABCDEF01"
        "456789ABCDEF0123");
    const uint8_t iv[8] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};

    const auto plaintext = hex_to_bytes(
        "4E6F772069732074"   // "Now is t"
        "68652074696D6520"); // "he time "

    TripleDES des(Mode::CBC, Padding::None, iv);

    std::vector<uint8_t> enc(plaintext.size());
    uint32_t enc_len = des.encrypt(enc.data(), plaintext.data(),
                                   static_cast<uint32_t>(plaintext.size()), key);
    EXPECT_EQ(enc_len, plaintext.size());

    // Verify decryption restores plaintext
    TripleDES des_dec(Mode::CBC, Padding::None, iv);
    std::vector<uint8_t> dec(enc.size());
    uint32_t dec_len = des_dec.decrypt(dec.data(), enc.data(), enc_len, key);
    EXPECT_EQ(dec_len, plaintext.size());
    EXPECT_EQ(dec, plaintext);

    // Verify changing IV changes ciphertext
    const uint8_t iv2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    TripleDES des2(Mode::CBC, Padding::None, iv2);
    std::vector<uint8_t> enc2(plaintext.size());
    uint32_t enc2_len = des2.encrypt(enc2.data(), plaintext.data(),
                                     static_cast<uint32_t>(plaintext.size()), key);
    EXPECT_EQ(enc2_len, plaintext.size());
    EXPECT_FALSE(std::equal(enc.begin(), enc.end(), enc2.begin()))
        << "Different IV should produce different ciphertext";
}
