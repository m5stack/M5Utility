/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file button_status.hpp
  @brief Button status management
 */
#ifndef M5_UTILITY_BUTTON_STATUS_HPP
#define M5_UTILITY_BUTTON_STATUS_HPP

#include <cstdint>

namespace m5 {
namespace utility {
namespace button {

/*!
  @class m5::utility::button::Status
  @brief Button status management
  @note Class compatible with Button_Class in M5Unified
 */
class Status {
public:
    /*!
      @enum button_state_t
      @brief Button status
     */
    enum class button_state_t : uint8_t { state_nochange, state_clicked, state_hold, state_decide_click_count };

    /*!
      @brief Constructor
      @param hold_ms Time to be considered hold(ms)
      @param debounce_ms Debounce time(ms)
     */
    Status(const uint16_t hold_ms = 500, const uint16_t debounce_ms = 10)
        : _msecHold{hold_ms}, _msecDebounce{debounce_ms}
    {
    }

    ///@name Settings
    ///@{
    //! @brief Set debounce time(ms)
    inline void setDebounceThreshold(const uint32_t msec)
    {
        _msecDebounce = msec;
    }
    //! @brief Set time to be considered hold(ms)
    inline void setHoldThreshold(const uint32_t msec)
    {
        _msecHold = msec;
    }
    //! @brief Gets the debounce time(ms)
    inline uint32_t getDebounceThreshold(void) const
    {
        return _msecDebounce;
    }
    //! @brief Gets the time to be considered hold(ms)
    inline uint32_t getHoldThreshold(void) const
    {
        return _msecHold;
    }
    ///@}

    ///@name Button status
    ///@{
    //! @brief Is pressed?
    bool isPressed(void) const
    {
        return _press;
    }
    //! @brief Is released?
    bool isReleased(void) const
    {
        return !_press;
    }
    //! @brief Returns true if the button is currently held pressed
    bool isHolding(void) const
    {
        return _press == 2;
    }
    //! @brief Returns true if button was pressed
    bool wasPressed(void) const
    {
        return !_oldPress && _press;
    }
    //! @brief Returns true if button was released
    bool wasReleased(void) const
    {
        return _oldPress && !_press;
    }
    //! @brief Returns true when the button is pressed briefly and released
    bool wasClicked(void) const
    {
        return _currentState == button_state_t::state_clicked;
    }
    //! @brief Returns true when the button has been held pressed for a while
    bool wasHold(void) const
    {
        return _currentState == button_state_t::state_hold;
    }
    //! @brief Returns true when some time has passed since the button was single clicked
    bool wasSingleClicked(void) const
    {
        return _currentState == button_state_t::state_decide_click_count && _clickCount == 1;
    }
    //! @brief Returns true when some time has passed since the button was double clicked
    bool wasDoubleClicked(void) const
    {
        return _currentState == button_state_t::state_decide_click_count && _clickCount == 2;
    }
    //! @brief Returns true when some time has passed since the button was multiple clicked
    bool wasDecideClickCount(void) const
    {
        return _currentState == button_state_t::state_decide_click_count;
    }
    //! @brief Gets the number of consecutive button clicks
    uint8_t getClickCount(void) const
    {
        return _clickCount;
    }
    //! @brief Has the button press state changed?
    bool wasChangePressed(void) const
    {
        return ((bool)_press) != ((bool)_oldPress);
    }
    //! @brief Pressed and released a button for more than the set hold time?
    bool wasReleasedAfterHold(void) const
    {
        return !_press && _oldPress == 2;
    }
    //! @brief Was it pressed for more than the specified time?
    bool wasReleaseFor(const uint32_t ms) const
    {
        return _oldPress && !_press && _lastHoldPeriod >= ms;
    }
    //! @brief Is pressed for more than the specified time?
    bool pressedFor(const uint32_t ms) const
    {
        return (_press && _lastMsec - _lastChange >= ms);
    }
    //! @brief Is released for more than the specified time?
    bool releasedFor(const uint32_t ms) const
    {
        return (!_press && _lastMsec - _lastChange >= ms);
    }
    ///@}

    ///@name Status
    ///@{
    void setRawState(const uint32_t msec, const bool press);
    void setState(const uint32_t msec, const button_state_t state);
    inline button_state_t getState(void) const
    {
        return _currentState;
    }
    inline uint32_t lastChange(void) const
    {
        return _lastChange;
    }
    inline uint32_t getUpdateMsec(void) const
    {
        return _lastMsec;
    }
    ///@}

private:
    uint16_t _msecHold{500}, _msecDebounce{10};
    uint32_t _lastMsec{}, _lastChange{}, _lastRawChange{}, _lastClicked{};
    uint16_t _lastHoldPeriod{};
    button_state_t _currentState{button_state_t::state_nochange};
    bool _raw_press{};
    // 0:release  1:click  2:holding
    uint8_t _press{}, _oldPress{}, _clickCount{};
};

}  // namespace button
}  // namespace utility
}  // namespace m5

#endif
