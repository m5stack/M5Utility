/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for compatibility_feature (Arduino API compatibility)
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>

using m5::utility::delay;
using m5::utility::delayMicroseconds;
using m5::utility::micros;
using m5::utility::millis;

TEST(CompatibilityFeature, MillisIncreasing)
{
    // millis() should return increasing values
    unsigned long t1 = millis();
    delay(10);
    unsigned long t2 = millis();

    EXPECT_GT(t2, t1) << "millis() should increase over time";
}

TEST(CompatibilityFeature, MicrosIncreasing)
{
    // micros() should return increasing values
    unsigned long t1 = micros();
    delayMicroseconds(100);
    unsigned long t2 = micros();

    EXPECT_GT(t2, t1) << "micros() should increase over time";
}

TEST(CompatibilityFeature, MicrosGreaterThanMillis)
{
    // micros() should be approximately millis() * 1000
    unsigned long ms = millis();
    unsigned long us = micros();

    // Allow some tolerance for execution time between calls
    // micros should be at least millis * 1000 (minus small tolerance)
    EXPECT_GE(us + 1000, ms * 1000) << "micros() should be >= millis() * 1000";
}

TEST(CompatibilityFeature, DelayApproximateTime)
{
    // delay() should pause for approximately the specified time
    const unsigned long delay_ms = 50;
    const unsigned long tolerance_ms = 20;  // Allow 20ms tolerance

    unsigned long start = millis();
    delay(delay_ms);
    unsigned long elapsed = millis() - start;

    EXPECT_GE(elapsed, delay_ms - tolerance_ms)
        << "delay() should pause for at least " << (delay_ms - tolerance_ms) << "ms";
    EXPECT_LE(elapsed, delay_ms + tolerance_ms)
        << "delay() should not pause for more than " << (delay_ms + tolerance_ms) << "ms";
}

TEST(CompatibilityFeature, DelayMicrosecondsApproximateTime)
{
    // delayMicroseconds() should pause for approximately the specified time
    const unsigned int delay_us   = 5000;   // 5ms in microseconds
    const unsigned long tolerance_us = 3000;  // Allow 3ms tolerance

    unsigned long start = micros();
    delayMicroseconds(delay_us);
    unsigned long elapsed = micros() - start;

    EXPECT_GE(elapsed, delay_us - tolerance_us)
        << "delayMicroseconds() should pause for at least " << (delay_us - tolerance_us) << "us";
    EXPECT_LE(elapsed, delay_us + tolerance_us)
        << "delayMicroseconds() should not pause for more than " << (delay_us + tolerance_us) << "us";
}

TEST(CompatibilityFeature, DelayZero)
{
    // delay(0) should return quickly
    unsigned long start = millis();
    delay(0);
    unsigned long elapsed = millis() - start;

    EXPECT_LE(elapsed, 5U) << "delay(0) should return quickly";
}

TEST(CompatibilityFeature, DelayMicrosecondsZero)
{
    // delayMicroseconds(0) should return quickly
    unsigned long start = micros();
    delayMicroseconds(0);
    unsigned long elapsed = micros() - start;

    EXPECT_LE(elapsed, 1000U) << "delayMicroseconds(0) should return quickly";
}

TEST(CompatibilityFeature, MultipleDelays)
{
    // Multiple delays should accumulate
    const unsigned long delay_ms = 20;
    const int count = 3;
    const unsigned long tolerance_ms = 30;

    unsigned long start = millis();
    for (int i = 0; i < count; ++i) {
        delay(delay_ms);
    }
    unsigned long elapsed = millis() - start;

    unsigned long expected = delay_ms * count;
    EXPECT_GE(elapsed, expected - tolerance_ms);
    EXPECT_LE(elapsed, expected + tolerance_ms);
}

TEST(CompatibilityFeature, ConsistencyBetweenMillisAndMicros)
{
    // The difference between consecutive millis/micros calls should be consistent
    unsigned long ms1 = millis();
    unsigned long us1 = micros();

    delay(100);

    unsigned long ms2 = millis();
    unsigned long us2 = micros();

    unsigned long ms_diff = ms2 - ms1;
    unsigned long us_diff = us2 - us1;

    // us_diff should be approximately ms_diff * 1000 (with tolerance)
    unsigned long expected_us = ms_diff * 1000;
    unsigned long tolerance = 10000;  // 10ms tolerance

    EXPECT_GE(us_diff + tolerance, expected_us);
    EXPECT_LE(us_diff, expected_us + tolerance);
}
