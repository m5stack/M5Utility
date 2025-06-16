/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file button_status.cpp
  @brief Button status management
 */

#include "button_status.hpp"

namespace m5 {
namespace utility {
namespace button {

void Status::setState(const uint32_t msec, const button_state_t state)
{
    if (_currentState == button_state_t::state_decide_click_count) {
        _clickCount = 0;
    }

    _lastMsec        = msec;
    bool flg_timeout = (msec - _lastClicked > _msecHold);
    auto new_state   = state;
    switch (state) {
        case button_state_t::state_nochange:
            if (flg_timeout && !_press && _clickCount) {
                if (_oldPress == 0 && _currentState == button_state_t::state_nochange) {
                    new_state = button_state_t::state_decide_click_count;
                } else {
                    _clickCount = 0;
                }
            }
            break;

        case button_state_t::state_clicked:
            ++_clickCount;
            _lastClicked = msec;
            break;

        default:
            break;
    }
    _currentState = new_state;
}

void Status::setRawState(const uint32_t msec, const bool press)
{
    button_state_t state = button_state_t::state_nochange;
    bool disable_db      = (msec - _lastMsec) > _msecDebounce;
    auto oldPress        = _press;
    _oldPress            = oldPress;
    if (_raw_press != press) {
        _raw_press     = press;
        _lastRawChange = msec;
    }
    if (disable_db || msec - _lastRawChange >= _msecDebounce) {
        if (press != (0 != oldPress)) {
            _lastChange = msec;
        }

        if (press) {
            uint32_t holdPeriod = msec - _lastChange;
            _lastHoldPeriod     = holdPeriod;
            if (!oldPress) {
                _press = 1;
            } else if (oldPress == 1 && (holdPeriod >= _msecHold)) {
                _press = 2;
                state  = button_state_t::state_hold;
            }
        } else {
            _press = 0;
            if (oldPress == 1) {
                state = button_state_t::state_clicked;
            }
        }
    }
    setState(msec, state);
}

}  // namespace button
}  // namespace utility
}  // namespace m5
