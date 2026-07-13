/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file platform_detect.hpp
  @brief Build-environment detection shared by the sources that need a time source or <chrono>

  Detection policy, kept in one place so supporting a new environment takes one
  change here (or a build-flag override) instead of edits scattered across sources:
  - Header existence is probed with __has_include (e.g. <sdkconfig.h> in library_log.hpp).
  - Language / standard-library features are probed with feature-test macros (__cplusplus etc.).
  - Environment traits (bare-metal vs hosted, which time API is authoritative) cannot be
    probed; they are derived from ecosystem-guaranteed macros (ESP_PLATFORM, ARDUINO).
    For an environment the auto-detection does not know, integrators can predefine
    M5UTILITY_TIME_SOURCE and/or M5UTILITY_HAS_USABLE_CHRONO to override it.
*/
#ifndef M5_UTILITY_PLATFORM_DETECT_HPP
#define M5_UTILITY_PLATFORM_DETECT_HPP

#define M5UTILITY_TIME_SOURCE_ESP     1  //!< esp_timer + FreeRTOS (ESP-IDF and Arduino-ESP32)
#define M5UTILITY_TIME_SOURCE_ARDUINO 2  //!< Arduino API millis()/micros()/delay()
#define M5UTILITY_TIME_SOURCE_CHRONO  3  //!< hosted C++ <chrono> + <thread>

/*!
  @def M5UTILITY_TIME_SOURCE
  @brief Which API backs m5::utility::millis()/micros()/delay()/delayMicroseconds() and the
  no-<chrono> log timestamp. Predefine to one of the values above to override the auto-detection.
*/
#if !defined(M5UTILITY_TIME_SOURCE)
#if defined(ESP_PLATFORM)
// Defined by the ESP-IDF build system and by every Arduino-ESP32 core, for all
// chips — no per-target CONFIG_IDF_TARGET_* list to maintain.
#define M5UTILITY_TIME_SOURCE M5UTILITY_TIME_SOURCE_ESP
#elif defined(ARDUINO)
// Any other Arduino core: millis()/micros()/delay() are guaranteed by the Arduino API
// contract on every core (RP2040, SAMD, ...), while <thread> generally is not
// (bare-metal, no RTOS by default).
#define M5UTILITY_TIME_SOURCE M5UTILITY_TIME_SOURCE_ARDUINO
#else
// Hosted build (native tests etc.)
#define M5UTILITY_TIME_SOURCE M5UTILITY_TIME_SOURCE_CHRONO
#endif
#endif

#if (M5UTILITY_TIME_SOURCE < M5UTILITY_TIME_SOURCE_ESP) || (M5UTILITY_TIME_SOURCE > M5UTILITY_TIME_SOURCE_CHRONO)
#error "M5UTILITY_TIME_SOURCE must be one of M5UTILITY_TIME_SOURCE_{ESP,ARDUINO,CHRONO}"
#endif

/*!
  @def M5UTILITY_HAS_USABLE_CHRONO
  @brief Whether the logging path may use <chrono> (elapsed_time_t and the log
  timestamp clock in log/library_log.*, the only consumers). Predefine (0/1)
  to override.

  Scope note: this flag alone does not make the whole library <chrono>-free —
  with M5UTILITY_TIME_SOURCE_CHRONO the time source itself still requires
  <chrono>/<thread> (compatibility_feature.cpp). An environment whose <chrono>
  is unusable needs a non-chrono M5UTILITY_TIME_SOURCE as well; the
  auto-detection already pairs the two (non-ESP Arduino cores get
  TIME_SOURCE_ARDUINO + HAS_USABLE_CHRONO=0).

  Why not __has_include(<chrono>): on some non-ESP32 Arduino cores the header
  exists but Arduino.h's abs()/round() function-like macros corrupt <chrono>'s
  own internal use of those names (empirically confirmed on the
  officially-registered PlatformIO atmelsam / raspberrypi(mbed) platforms,
  GCC 9.x libstdc++).
*/
#if !defined(M5UTILITY_HAS_USABLE_CHRONO)
#if defined(ARDUINO) && !defined(ESP_PLATFORM)
#define M5UTILITY_HAS_USABLE_CHRONO 0
#else
#define M5UTILITY_HAS_USABLE_CHRONO 1
#endif
#endif

#endif  // M5_UTILITY_PLATFORM_DETECT_HPP
