/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for M5Utility
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>

TEST(Utility, String_Trim)
{
    // Basic trim
    std::string s = "\t\r\n\v STRING \v\n\r\t";
    s             = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "STRING");

    // Empty string
    s = "";
    s = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "");

    // Whitespace only
    s = "   \t\n\r\v  ";
    s = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "");

    // No whitespace
    s = "NoWhitespace";
    s = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "NoWhitespace");

    // Leading whitespace only
    s = "  \t leading";
    s = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "leading");

    // Trailing whitespace only
    s = "trailing \t  ";
    s = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "trailing");

    // Whitespace in middle preserved
    s = "  hello world  ";
    s = m5::utility::trim(s);
    EXPECT_STREQ(s.c_str(), "hello world");
}

TEST(Utility, String_TrimLeft)
{
    std::string s = "  \tTest  \t";
    s             = m5::utility::trimLeft(s);
    EXPECT_STREQ(s.c_str(), "Test  \t");

    s = "NoSpace";
    s = m5::utility::trimLeft(s);
    EXPECT_STREQ(s.c_str(), "NoSpace");

    // Empty string
    s = "";
    s = m5::utility::trimLeft(s);
    EXPECT_STREQ(s.c_str(), "");

    // Whitespace only
    s = "   \t\n";
    s = m5::utility::trimLeft(s);
    EXPECT_STREQ(s.c_str(), "");

    // Trailing whitespace only — should NOT be removed
    s = "trailing   ";
    s = m5::utility::trimLeft(s);
    EXPECT_STREQ(s.c_str(), "trailing   ");
}

TEST(Utility, String_TrimRight)
{
    std::string s = "  \tTest  \t";
    s             = m5::utility::trimRight(s);
    EXPECT_STREQ(s.c_str(), "  \tTest");

    s = "NoSpace";
    s = m5::utility::trimRight(s);
    EXPECT_STREQ(s.c_str(), "NoSpace");

    // Empty string
    s = "";
    s = m5::utility::trimRight(s);
    EXPECT_STREQ(s.c_str(), "");

    // Whitespace only
    s = "   \t\n";
    s = m5::utility::trimRight(s);
    EXPECT_STREQ(s.c_str(), "");

    // Leading whitespace only — should NOT be removed
    s = "   leading";
    s = m5::utility::trimRight(s);
    EXPECT_STREQ(s.c_str(), "   leading");
}

TEST(Utility, String_FormatString)
{
    // Basic string formatting
    auto s = m5::utility::formatString("Hello %s", "World");
    EXPECT_STREQ(s.c_str(), "Hello World");

    // Integer formatting
    s = m5::utility::formatString("%d + %d = %d", 1, 2, 3);
    EXPECT_STREQ(s.c_str(), "1 + 2 = 3");

    // Hex formatting
    s = m5::utility::formatString("0x%04X", 0x1234);
    EXPECT_STREQ(s.c_str(), "0x1234");

    // Float formatting
    s = m5::utility::formatString("%.2f", 3.14159);
    EXPECT_STREQ(s.c_str(), "3.14");

    // Multiple format specifiers
    s = m5::utility::formatString("%s: %d (0x%02X)", "Value", 255, 255);
    EXPECT_STREQ(s.c_str(), "Value: 255 (0xFF)");

    // Empty format string
    s = m5::utility::formatString("");
    EXPECT_STREQ(s.c_str(), "");

    // No format specifiers
    s = m5::utility::formatString("Plain text");
    EXPECT_STREQ(s.c_str(), "Plain text");

    // Long string
    s = m5::utility::formatString("%s%s%s", "This is ", "a longer ", "string test");
    EXPECT_STREQ(s.c_str(), "This is a longer string test");

    // Large string (> 256 bytes)
    {
        std::string large(512, 'X');
        auto r = m5::utility::formatString("%s", large.c_str());
        EXPECT_EQ(r.size(), 512U);
        EXPECT_EQ(r, large);
    }
}

TEST(Utility, HexString)
{
    {
        std::pair<uint8_t, char> table_upper[] = {
            {0, '0'}, {1, '1'},  {2, '2'},  {3, '3'},  {4, '4'},  {5, '5'},  {6, '6'},  {7, '7'},   {8, '8'},
            {9, '9'}, {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'}, {100, '4'},
        };
        std::pair<uint8_t, char> table_lower[] = {
            {0, '0'}, {1, '1'},  {2, '2'},  {3, '3'},  {4, '4'},  {5, '5'},  {6, '6'},  {7, '7'},   {8, '8'},
            {9, '9'}, {10, 'a'}, {11, 'b'}, {12, 'c'}, {13, 'd'}, {14, 'e'}, {15, 'f'}, {100, '4'},
        };

        for (auto&& e : table_upper) {
            EXPECT_EQ(m5::utility::uintToHexChar<true>(e.first), e.second);
        }
        for (auto&& e : table_lower) {
            EXPECT_EQ(m5::utility::uintToHexChar<false>(e.first), e.second);
        }
    }

    std::string s;
    {
        uint8_t zero = 0;
        uint8_t v    = 0xA2;
        s            = m5::utility::unsignedToHexString(zero);
        EXPECT_STREQ(s.c_str(), "00");
        s = m5::utility::unsignedToHexString(v);
        EXPECT_STREQ(s.c_str(), "A2");
    }
    {
        uint16_t zero = 0;
        uint16_t v    = 0x0D51;
        s             = m5::utility::unsignedToHexString(zero);
        EXPECT_STREQ(s.c_str(), "0000");
        s = m5::utility::unsignedToHexString(v);
        EXPECT_STREQ(s.c_str(), "0D51");
    }
    {
        uint32_t zero = 0;
        uint32_t v    = 0xBEAF1234;
        s             = m5::utility::unsignedToHexString(zero);
        EXPECT_STREQ(s.c_str(), "00000000");
        s = m5::utility::unsignedToHexString(v);
        EXPECT_STREQ(s.c_str(), "BEAF1234");
    }
    {
        uint64_t zero = 0;
        uint64_t v    = 0x5252DEADBEAF0303;
        s             = m5::utility::unsignedToHexString(zero);
        EXPECT_STREQ(s.c_str(), "0000000000000000");
        s = m5::utility::unsignedToHexString(v);
        EXPECT_STREQ(s.c_str(), "5252DEADBEAF0303");
    }
}

TEST(Utility, HexString_LowerCase)
{
    std::string s;

    // uint8_t lowercase
    s = m5::utility::unsignedToHexString<uint8_t, false>(0xAB);
    EXPECT_STREQ(s.c_str(), "ab");

    // uint16_t lowercase
    s = m5::utility::unsignedToHexString<uint16_t, false>(0xCDEF);
    EXPECT_STREQ(s.c_str(), "cdef");

    // uint32_t lowercase
    s = m5::utility::unsignedToHexString<uint32_t, false>(0xDEADBEEF);
    EXPECT_STREQ(s.c_str(), "deadbeef");

    // uint64_t lowercase
    s = m5::utility::unsignedToHexString<uint64_t, false>(0xCAFEBABE12345678);
    EXPECT_STREQ(s.c_str(), "cafebabe12345678");
}
