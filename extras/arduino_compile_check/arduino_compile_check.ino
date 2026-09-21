/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Minimal sketch for the Arduino CI compile check (CI only)
*/
#include <M5Utility.h>

void setup()
{
    m5::utility::delay(1);
}

void loop()
{
    M5_LIB_LOGI("t=%lu", m5::utility::millis());
    m5::utility::delay(1000);
}
