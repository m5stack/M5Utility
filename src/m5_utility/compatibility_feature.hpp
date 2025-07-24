/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file compatibility_feature.hpp
  @brief Maintain compatibility with Arduino API, etc.
*/
#ifndef M5_UTILITY_COMPATIBILITY_FEATURE_HPP
#define M5_UTILITY_COMPATIBILITY_FEATURE_HPP

#include <freertos/FreeRTOS.h>
#include <esp_timer.h>

namespace m5 {
namespace utility {

///@name Arduino API
///@{
/*!
  @brief Returns the number of milliseconds passed since the Arduino board began running the current program
 */
IRAM_ATTR unsigned long millis();

/*!
  @brief Returns the number of microseconds since the Arduino board began running the current program
*/
IRAM_ATTR unsigned long micros();

/*!
  @brief Pauses the program for the amount of time (in milliseconds) specified as parameter
  @param ms delay time (ms)
  @warning Accuracy varies depending on the environment.
*/
void delay(const unsigned long ms);

/*!
  @brief Pauses the program for the amount of time (in microseconds) specified by the parameter
  @param us delay time (us)
  @warning Accuracy varies depending on the environment.
*/
void delayMicroseconds(const unsigned int us);
///@}

}  // namespace utility
}  // namespace m5
#endif
