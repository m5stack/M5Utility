/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file string.cpp
  @brief Utilities for string
*/
#include "string.hpp"
#include <cstdarg>
#include <algorithm>

namespace m5 {
namespace utility {

std::string formatString(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t sz = vsnprintf(nullptr, 0U, fmt, args);  // calculate length
    va_end(args);

    std::string result(sz, '\0');
    va_start(args, fmt);
    vsnprintf(&result[0], sz + 1, fmt, args);
    va_end(args);
    return result;
}

std::string& trimRight(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](std::string::value_type& ch) { return !std::isspace(ch); }).base(),
            s.end());
    return s;
}

std::string& trimLeft(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](std::string::value_type& ch) { return !std::isspace(ch); }));
    return s;
}

std::string& trim(std::string& s)
{
    return trimRight(trimLeft(s));
}

}  // namespace utility
}  // namespace m5
