/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file des.hpp
  @brief Data Encryption Standar (DES)
*/
#ifndef M5_UTILITY_DES_HPP
#define M5_UTILITY_DES_HPP
#include <cstdint>
#include <array>

namespace m5 {
namespace utility {
namespace crypto {

/*!
  @class TrippleDES
  @brief Triple Data Encryption Standar(3DES)
*/
class TripleDES {
public:
    using Key   = std::array<uint8_t, 8>;
    using Key8  = Key;
    using Key16 = std::array<uint8_t, 16>;
    using Key24 = std::array<uint8_t, 24>;

    /*!
      @enum Mode
      @brief Operation mode
     */
    enum class Mode : uint8_t {
        ECB,  //!< Electronic CodeBook
        CBC,  //!< Cipher Block Chaining
    };
    /*!
      @enum Padding
      @brief Padding mode
     */
    enum class Padding : uint8_t {
        None,
        Zero,
        PKCS7,
    };

    /*!
      @brief Constructor
      @param mode Mode
      @param pad  Padding
      @param iv Initialization vector for CBC mode
      @note IV is 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 if iv is nullptr
     */
    TripleDES(const Mode mode, const Padding pad, const uint8_t iv[8] = nullptr);

    ///@name Encrypto
    ///@{
    /*!
      @brief Encrypto 1-key 3DES
      @param[out] out Output buffer
      @param in Input
      @param in_len Input length
      @param key Key
      @return Output length
      @note Same as single DES
      @note out at leaset in_len if Padding::None
      @note out at leaset (in_len + 7)/8+8 if Padding::Zero, PKCS7
     */
    inline uint32_t encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key& key)
    {
        return encrypt_3key(out, in, in_len, key, key, key);
    }
    /*!
      @brief Encrypto 2-key 3DES
      @param[out] out Output buffer
      @param in Input
      @param in_len Input length
      @param key1 Key1
      @param key2 Key2
      @return Output length
      @note out at leaset in_len if Padding::None
      @note out at leaset (in_len + 7)/8+8 if Padding::Zero, PKCS7
     */
    inline uint32_t encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key& key1, const Key& key2)
    {
        return encrypt_3key(out, in, in_len, key1, key2, key1);
    }
    /*!
      @brief Encrypto 2-key 3DES
      @param[out] out Output buffer
      @param in Input
      @param in_len Input length
      @param key Key
      @return Output length
      @note out at leaset in_len if Padding::None
      @note out at leaset (in_len + 7)/8+8 if Padding::Zero, PKCS7
     */
    uint32_t encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key16& key);
    /*!
      @brief Encrypto 3-key 3DES
      @param[out] out Output buffer
      @param in Input
      @param in_len Input length
      @param key1 Key1
      @param key2 Key2
      @param key3 Key3
      @return Output length
      @note out at leaset in_len if Padding::None
      @note out at leaset (in_len + 7)/8+8 if Padding::Zero, PKCS7
     */
    inline uint32_t encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key& key1, const Key& key2,
                            const Key& key3)
    {
        return encrypt_3key(out, in, in_len, key1, key2, key3);
    }
    /*!
      @brief Encrypto 3-key 3DES
      @param[out] out Output buffer
      @param in Input
      @param in_len Input length
      @param key Key
      @return Output length
      @note out at leaset in_len if Padding::None
      @note out at leaset (in_len + 7)/8+8 if Padding::Zero, PKCS7
     */
    uint32_t encrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key24& key);
    ///@}

    ///@name Decrypto
    ///@{
    inline uint32_t decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key& key)
    {
        return decrypt_3key(out, in, in_len, key, key, key);
    }

    inline uint32_t decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key& key1, const Key& key2)
    {
        return decrypt_3key(out, in, in_len, key1, key2, key1);
    }

    inline uint32_t decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key& key1, const Key& key2,
                            const Key& key3)
    {
        return decrypt_3key(out, in, in_len, key1, key2, key3);
    }

    uint32_t decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key16& key);
    uint32_t decrypt(uint8_t* out, const uint8_t* in, const uint32_t in_len, const Key24& key);
    ///@}

    static void encrypt_block(uint8_t out[8], const uint8_t in[8], const Key& key);
    static void decrypt_block(uint8_t out[8], const uint8_t in[8], const Key& key);

protected:
    static void key_schedule(uint64_t subkeys[16], const Key& key);
    static uint32_t feistel(const uint32_t r, const uint64_t subkey);
    static uint64_t permute_bits(const uint64_t in, const uint8_t* table, const uint32_t table_len,
                                 const uint32_t in_bits);
    static void process_block(uint8_t out[8], const uint8_t in[8], const Key& key, bool encrypt);

    static void encrypt_block_3key(uint8_t out[8], const uint8_t in[8], const Key8& k1, const Key8& k2, const Key8& k3);
    uint32_t encrypt_3key(uint8_t* out, const uint8_t* in, uint32_t in_len, const Key& k1, const Key& k2,
                          const Key& k3);
    static void decrypt_block_3key(uint8_t out[8], const uint8_t in[8], const Key8& k1, const Key8& k2, const Key8& k3);
    uint32_t decrypt_3key(uint8_t* out, const uint8_t* in, uint32_t in_len, const Key& k1, const Key& k2,
                          const Key& k3);

private:
    Mode _mode{};
    Padding _pad{};
    uint8_t _iv[8]{};
};

}  // namespace crypto
}  // namespace utility
}  // namespace m5

#endif
