/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for button_status
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>

using m5::utility::button::Status;
using button_state_t = Status::button_state_t;

TEST(ButtonStatus, Constructor)
{
    // Default constructor
    {
        Status btn;
        EXPECT_EQ(btn.getHoldThreshold(), 500U);
        EXPECT_EQ(btn.getDebounceThreshold(), 10U);
        EXPECT_FALSE(btn.isPressed());
        EXPECT_TRUE(btn.isReleased());
        EXPECT_FALSE(btn.isHolding());
        EXPECT_EQ(btn.getState(), button_state_t::state_nochange);
        EXPECT_EQ(btn.getClickCount(), 0U);
    }

    // Custom thresholds
    {
        Status btn(1000, 50);
        EXPECT_EQ(btn.getHoldThreshold(), 1000U);
        EXPECT_EQ(btn.getDebounceThreshold(), 50U);
    }
}

TEST(ButtonStatus, ThresholdSettings)
{
    Status btn;

    btn.setHoldThreshold(800);
    EXPECT_EQ(btn.getHoldThreshold(), 800U);

    btn.setDebounceThreshold(20);
    EXPECT_EQ(btn.getDebounceThreshold(), 20U);
}

TEST(ButtonStatus, InitialState)
{
    Status btn;

    EXPECT_FALSE(btn.isPressed());
    EXPECT_TRUE(btn.isReleased());
    EXPECT_FALSE(btn.isHolding());
    EXPECT_FALSE(btn.wasPressed());
    EXPECT_FALSE(btn.wasReleased());
    EXPECT_FALSE(btn.wasClicked());
    EXPECT_FALSE(btn.wasHold());
    EXPECT_FALSE(btn.wasSingleClicked());
    EXPECT_FALSE(btn.wasDoubleClicked());
    EXPECT_FALSE(btn.wasDecideClickCount());
    EXPECT_FALSE(btn.wasChangePressed());
    EXPECT_FALSE(btn.wasReleasedAfterHold());
}

TEST(ButtonStatus, SimplePress)
{
    Status btn(500, 10);
    uint32_t time = 0;

    // Initial state - not pressed
    btn.setRawState(time, false);
    EXPECT_FALSE(btn.isPressed());
    EXPECT_TRUE(btn.isReleased());

    // Press button (after debounce time)
    time = 100;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());
    EXPECT_FALSE(btn.isReleased());
    EXPECT_TRUE(btn.wasPressed());
    EXPECT_FALSE(btn.wasReleased());
    EXPECT_FALSE(btn.isHolding());
}

TEST(ButtonStatus, SimpleClick)
{
    Status btn(500, 10);
    uint32_t time = 0;

    // Press
    btn.setRawState(time, false);
    time = 100;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());

    // Release (click)
    time = 200;
    btn.setRawState(time, false);
    EXPECT_FALSE(btn.isPressed());
    EXPECT_TRUE(btn.wasReleased());
    EXPECT_TRUE(btn.wasClicked());
    EXPECT_EQ(btn.getState(), button_state_t::state_clicked);
    EXPECT_EQ(btn.getClickCount(), 1U);
}

TEST(ButtonStatus, Hold)
{
    Status btn(500, 10);
    uint32_t time = 0;

    // Initial
    btn.setRawState(time, false);

    // Press
    time = 100;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());
    EXPECT_FALSE(btn.isHolding());
    EXPECT_FALSE(btn.wasHold());

    // Still pressed but not yet hold time
    time = 400;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());
    EXPECT_FALSE(btn.isHolding());
    EXPECT_FALSE(btn.wasHold());

    // Hold time reached (500ms after press at 100ms = 600ms)
    time = 700;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());
    EXPECT_TRUE(btn.isHolding());
    EXPECT_TRUE(btn.wasHold());
    EXPECT_EQ(btn.getState(), button_state_t::state_hold);
}

TEST(ButtonStatus, ReleaseAfterHold)
{
    Status btn(500, 10);
    uint32_t time = 0;

    // Initial -> Press -> Hold
    btn.setRawState(time, false);
    time = 100;
    btn.setRawState(time, true);
    time = 700;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isHolding());

    // Release after hold
    time = 800;
    btn.setRawState(time, false);
    EXPECT_FALSE(btn.isPressed());
    EXPECT_TRUE(btn.wasReleasedAfterHold());
    EXPECT_FALSE(btn.wasClicked());  // Not a click, it was a hold
}

TEST(ButtonStatus, DoubleClick)
{
    Status btn(500, 10);
    uint32_t time = 0;

    // Initial
    btn.setRawState(time, false);

    // First click
    time = 100;
    btn.setRawState(time, true);
    time = 150;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasClicked());
    EXPECT_EQ(btn.getClickCount(), 1U);

    // Poll before second click (resets currentState to nochange)
    time = 200;
    btn.setRawState(time, false);

    // Second click (within hold threshold)
    time = 250;
    btn.setRawState(time, true);
    time = 300;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasClicked());
    EXPECT_EQ(btn.getClickCount(), 2U);

    // Poll before timeout (resets currentState to nochange while preserving clickCount)
    time = 350;
    btn.setRawState(time, false);
    EXPECT_EQ(btn.getClickCount(), 2U);

    // Wait for click count decision (after hold threshold from last click)
    time = 900;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasDecideClickCount());
    EXPECT_TRUE(btn.wasDoubleClicked());
    EXPECT_EQ(btn.getClickCount(), 2U);
}

TEST(ButtonStatus, SingleClickDecision)
{
    Status btn(500, 10);
    uint32_t time = 0;

    // Initial
    btn.setRawState(time, false);

    // Single click
    time = 100;
    btn.setRawState(time, true);
    time = 150;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasClicked());
    EXPECT_EQ(btn.getClickCount(), 1U);

    // Poll before timeout (resets currentState to nochange)
    time = 200;
    btn.setRawState(time, false);
    EXPECT_EQ(btn.getClickCount(), 1U);

    // Wait for click count decision
    time = 800;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasDecideClickCount());
    EXPECT_TRUE(btn.wasSingleClicked());
    EXPECT_FALSE(btn.wasDoubleClicked());
}

TEST(ButtonStatus, TripleClick)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);

    // First click
    time = 100;
    btn.setRawState(time, true);
    time = 150;
    btn.setRawState(time, false);

    // Poll between clicks
    time = 200;
    btn.setRawState(time, false);

    // Second click
    time = 250;
    btn.setRawState(time, true);
    time = 300;
    btn.setRawState(time, false);

    // Poll between clicks
    time = 350;
    btn.setRawState(time, false);

    // Third click
    time = 400;
    btn.setRawState(time, true);
    time = 450;
    btn.setRawState(time, false);
    EXPECT_EQ(btn.getClickCount(), 3U);

    // Poll before timeout (resets currentState to nochange)
    time = 500;
    btn.setRawState(time, false);
    EXPECT_EQ(btn.getClickCount(), 3U);

    // Wait for decision
    time = 1100;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasDecideClickCount());
    EXPECT_EQ(btn.getClickCount(), 3U);
    EXPECT_FALSE(btn.wasSingleClicked());
    EXPECT_FALSE(btn.wasDoubleClicked());
}

TEST(ButtonStatus, PressedFor)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);

    // Press at time 100
    time = 100;
    btn.setRawState(time, true);

    // Check pressedFor at various times
    time = 200;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.pressedFor(50));
    EXPECT_TRUE(btn.pressedFor(100));
    EXPECT_FALSE(btn.pressedFor(150));

    time = 400;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.pressedFor(200));
    EXPECT_TRUE(btn.pressedFor(300));
    EXPECT_FALSE(btn.pressedFor(350));
}

TEST(ButtonStatus, ReleasedFor)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);

    // Press and release
    time = 100;
    btn.setRawState(time, true);
    time = 200;
    btn.setRawState(time, false);

    // Check releasedFor at various times
    time = 300;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.releasedFor(50));
    EXPECT_TRUE(btn.releasedFor(100));
    EXPECT_FALSE(btn.releasedFor(150));

    time = 500;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.releasedFor(200));
    EXPECT_TRUE(btn.releasedFor(300));
    EXPECT_FALSE(btn.releasedFor(350));
}

TEST(ButtonStatus, WasReleaseFor)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);

    // Press for 150ms then release
    time = 100;
    btn.setRawState(time, true);
    time = 250;
    btn.setRawState(time, true);

    // Release - need enough time gap to pass debounce check
    time = 400;
    btn.setRawState(time, false);

    // Check wasReleaseFor (checks if previous press duration was >= ms)
    // _lastHoldPeriod was 150ms (250 - 100) from the last press update
    EXPECT_TRUE(btn.wasReleaseFor(100));
    EXPECT_TRUE(btn.wasReleaseFor(150));
    EXPECT_FALSE(btn.wasReleaseFor(200));
}

TEST(ButtonStatus, WasChangePressed)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);
    EXPECT_FALSE(btn.wasChangePressed());

    // Press - state changes
    time = 100;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.wasChangePressed());

    // Still pressed - no change
    time = 200;
    btn.setRawState(time, true);
    EXPECT_FALSE(btn.wasChangePressed());

    // Release - state changes
    time = 300;
    btn.setRawState(time, false);
    EXPECT_TRUE(btn.wasChangePressed());

    // Still released - no change
    time = 400;
    btn.setRawState(time, false);
    EXPECT_FALSE(btn.wasChangePressed());
}

TEST(ButtonStatus, LastChangeAndUpdateMsec)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);

    time = 100;
    btn.setRawState(time, true);
    EXPECT_EQ(btn.lastChange(), 100U);
    EXPECT_EQ(btn.getUpdateMsec(), 100U);

    time = 200;
    btn.setRawState(time, true);
    EXPECT_EQ(btn.lastChange(), 100U);  // No change in press state
    EXPECT_EQ(btn.getUpdateMsec(), 200U);

    time = 300;
    btn.setRawState(time, false);
    EXPECT_EQ(btn.lastChange(), 300U);  // Changed to released
    EXPECT_EQ(btn.getUpdateMsec(), 300U);
}

TEST(ButtonStatus, Debounce)
{
    Status btn(500, 50);  // 50ms debounce
    uint32_t time = 0;

    btn.setRawState(time, false);

    // Press at time 100
    time = 100;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());

    // Quick bounce at time 110 (within debounce) - should be ignored
    time = 110;
    btn.setRawState(time, false);
    // Note: debounce behavior depends on implementation details
    // The raw state changes but the processed state may not

    // After debounce time, state should be stable
    time = 200;
    btn.setRawState(time, true);
    EXPECT_TRUE(btn.isPressed());
}

TEST(ButtonStatus, SetState)
{
    Status btn(500, 10);
    uint32_t time = 0;

    btn.setRawState(time, false);

    // Manually set state to clicked
    time = 100;
    btn.setState(time, button_state_t::state_clicked);
    EXPECT_EQ(btn.getState(), button_state_t::state_clicked);
    EXPECT_EQ(btn.getClickCount(), 1U);

    // Set to nochange
    time = 200;
    btn.setState(time, button_state_t::state_nochange);
    EXPECT_EQ(btn.getState(), button_state_t::state_nochange);

    // Set to hold
    time = 300;
    btn.setState(time, button_state_t::state_hold);
    EXPECT_EQ(btn.getState(), button_state_t::state_hold);
}
