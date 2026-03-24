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

#if defined(ESP_PLATFORM) || defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) ||                 \
    defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP32C3) ||  \
    defined(CONFIG_IDF_TARGET_ESP32C5) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C61) || \
    defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32P4)
#define USING_ESP_PLATFORM
#pragma message("Using ESP Platform")
#else
#pragma message("NOT Using ESP Platform")
#endif

#if defined(USING_ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_cpu.h>
#include <esp_timer.h>
#else
#include <ctime>
#include <chrono>
#include <thread>
#define IRAM_ATTR /* nop */
#endif

namespace {

#if !defined(USING_ESP_PLATFORM)
using clock                      = std::chrono::high_resolution_clock;
const clock::time_point start_at = clock::now();
#endif

}  // namespace

namespace m5 {
namespace utility {

IRAM_ATTR unsigned long millis()
{
#if defined(USING_ESP_PLATFORM)
    return static_cast<unsigned long>(esp_timer_get_time() / 1000ULL);
#else
    return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - ::start_at).count();
#endif
}

IRAM_ATTR unsigned long micros()
{
#if defined(USING_ESP_PLATFORM)
    return static_cast<unsigned long>(esp_timer_get_time());
#else
    return std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - ::start_at).count();
#endif
}

void delay(const unsigned long ms)
{
#if defined(USING_ESP_PLATFORM)
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
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

void delayMicroseconds(const unsigned int us)
{
#if defined(USING_ESP_PLATFORM)
    if (us) {
        esp_rom_delay_us(us);
    }
#else
    std::this_thread::sleep_for(std::chrono::microseconds(us));
#endif
}

}  // namespace utility
}  // namespace m5
