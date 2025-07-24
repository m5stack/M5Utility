/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file compatibility_feature.cpp
  @brief Maintain compatibility with Arduino API, etc.
*/
#include "compatibility_feature.hpp"
#include <freertos/task.h>
#include <esp_cpu.h>

namespace {

}  // namespace

namespace m5 {
namespace utility {

IRAM_ATTR unsigned long millis()
{
    return static_cast<unsigned long>(esp_timer_get_time() / 1000ULL);
}
IRAM_ATTR unsigned long micros()
{
    return static_cast<unsigned long>(esp_timer_get_time());
}

void delay(const unsigned long ms)
{
    if (ms) {
        if (xPortInIsrContext()) {
            // Using busy-wait in ISR
            const uint64_t target = esp_timer_get_time() + static_cast<uint64_t>(ms) * 1000ULL;
            while (esp_timer_get_time() < target) { /* nop */
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(ms));
        }
    }
}

void delayMicroseconds(const unsigned int us)
{
    if (us) {
        // Using esp_rom_delay if less than 1ms
        if (us < 1000 || xPortInIsrContext()) {
            esp_rom_delay_us(us);
            return;
        }

        // vTaskDelay + esp_rom_delay
        vTaskDelay(pdMS_TO_TICKS(us / 1000));
        const uint32_t us_rem = us % 1000;
        if (us_rem) {
            esp_rom_delay_us(us_rem);
        }
    }
}

}  // namespace utility
}  // namespace m5
