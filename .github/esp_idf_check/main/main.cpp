/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file main.cpp
  @brief Minimal app for the ESP-IDF native compile check (CI only)
*/
#include <M5Utility.h>

extern "C" void app_main(void)
{
    M5_LIB_LOGI("t=%ld", static_cast<long>(m5::utility::millis()));
    m5::utility::delay(10);
}
