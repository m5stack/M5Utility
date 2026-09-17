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

namespace m5 {
namespace utility {

///@name Arduino API
///@{
/*!
  @brief Returns the number of milliseconds passed since the Arduino board began running the current program
 */
unsigned long millis();

/*!
  @brief Returns the number of microseconds since the Arduino board began running the current program
*/
unsigned long micros();

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

///@name Elapsed time
///@{
/*!
  @brief Elapsed time unit (ms)
  @note Identical to the return type of millis(), so passing a value obtained
  from millis() never narrows, neither on 32-bit targets nor on 64-bit hosted builds
 */
using elapsed_time_t = unsigned long;

/*!
  @brief Gets the elapsed time between start_at and now
  @param start_at Earlier time
  @param now Current time
  @return Elapsed time (ms)
  @note Unsigned subtraction yields the correct elapsed time even across the wrap
  of the clock, whatever the width of elapsed_time_t
  @note Overload for code that takes its current time from somewhere other than
  millis(), such as an injected clock that unit tests can drive
 */
inline elapsed_time_t elapsedSince(const elapsed_time_t start_at, const elapsed_time_t now)
{
    return now - start_at;
}

/*!
  @brief Gets the elapsed time since start_at
  @param start_at Time obtained by millis()
  @return Elapsed time (ms)
 */
inline elapsed_time_t elapsedSince(const elapsed_time_t start_at)
{
    return elapsedSince(start_at, millis());
}

/*!
  @brief Has the duration passed between start_at and now?
  @param start_at Earlier time
  @param duration Duration to wait for (ms)
  @param now Current time
  @return True if the duration has passed
  @note Compares the elapsed time rather than a precomputed deadline. A deadline
  such as (start_at + duration) overflows near the wrap of the clock and cuts the
  wait short, so compare the elapsed time instead
  @note Overload for code that takes its current time from somewhere other than
  millis(), such as an injected clock that unit tests can drive. The current time
  is the trailing argument so that the shorter form is a prefix of this one and
  cannot be called by mistake
 */
inline bool hasElapsed(const elapsed_time_t start_at, const elapsed_time_t duration, const elapsed_time_t now)
{
    return elapsedSince(start_at, now) >= duration;
}

/*!
  @brief Has the duration passed since start_at?
  @param start_at Time obtained by millis()
  @param duration Duration to wait for (ms)
  @return True if the duration has passed
 */
inline bool hasElapsed(const elapsed_time_t start_at, const elapsed_time_t duration)
{
    return hasElapsed(start_at, duration, millis());
}
///@}

}  // namespace utility
}  // namespace m5
#endif
