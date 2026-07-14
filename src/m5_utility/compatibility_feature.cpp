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
#include "platform_detect.hpp"

#if M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ESP
#pragma message("Using ESP Platform")
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#elif M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ARDUINO
#pragma message("Using non-ESP Arduino Platform")
#include <Arduino.h>
#ifndef IRAM_ATTR
#define IRAM_ATTR /* nop */
#endif
#else
#pragma message("Using hosted C++ Platform")
#include <ctime>
#include <chrono>
#include <thread>
#ifndef IRAM_ATTR
#define IRAM_ATTR /* nop */
#endif
#endif

namespace {

#if M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_CHRONO
using clock                      = std::chrono::high_resolution_clock;
const clock::time_point start_at = clock::now();
#endif

}  // namespace

namespace m5 {
namespace utility {

IRAM_ATTR unsigned long millis()
{
#if M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ESP
    return static_cast<unsigned long>(esp_timer_get_time() / 1000ULL);
#elif M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ARDUINO
    return static_cast<unsigned long>(::millis());
#else
    return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - ::start_at).count();
#endif
}

IRAM_ATTR unsigned long micros()
{
#if M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ESP
    return static_cast<unsigned long>(esp_timer_get_time());
#elif M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ARDUINO
    return static_cast<unsigned long>(::micros());
#else
    return std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - ::start_at).count();
#endif
}

void delay(const unsigned long ms)
{
#if M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ESP
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
#elif M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ARDUINO
    ::delay(ms);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

void delayMicroseconds(const unsigned int us)
{
#if M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ESP
    if (us) {
        esp_rom_delay_us(us);
    }
#elif M5UTILITY_TIME_SOURCE == M5UTILITY_TIME_SOURCE_ARDUINO
    ::delayMicroseconds(us);
#else
    std::this_thread::sleep_for(std::chrono::microseconds(us));
#endif
}

}  // namespace utility
}  // namespace m5
