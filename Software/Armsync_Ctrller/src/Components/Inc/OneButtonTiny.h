// -----
// OneButtonTiny.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button.
// Renesas RA FSP adaptation.
// -----

#pragma once

#include <cstdint>

extern "C" {
    typedef void (*callbackFunction)(void);
}

class OneButtonTiny {
public:
    /**
     * @param readPin 回调函数：读引脚电平，返回 true = 按下
     * @param getTick 回调函数：返回系统毫秒数
     */
    OneButtonTiny(bool (*readPin)(), uint32_t (*getTick)());

    void setDebounceMs(uint16_t ms)    { _debounce_ms = ms; }
    void setClickMs(uint16_t ms)       { _click_ms = ms; }
    void setPressMs(uint16_t ms)       { _press_ms = ms; }
    void setMultiClickMs(uint16_t ms)  { _multiClick_ms = ms; }

    void attachClick(callbackFunction fn)           { _clickFunc = fn; }
    void attachDoubleClick(callbackFunction fn)     { _doubleClickFunc = fn; }
    void attachMultiClick(callbackFunction fn)      { _multiClickFunc = fn; }
    void attachLongPressStart(callbackFunction fn)  { _longPressStartFunc = fn; }

    void tick();
    void tick(bool level);

    void reset();
    bool isIdle() const { return _state == OCS_INIT; }

private:
    bool (*_readPin)() = nullptr;
    uint32_t (*_getTick)() = nullptr;

    uint16_t _debounce_ms = 50;
    uint16_t _click_ms = 400;
    uint16_t _press_ms = 800;
    uint16_t _multiClick_ms = 600;

    bool _buttonPressed = true;  // true = 低电平按下

    callbackFunction _clickFunc = nullptr;
    callbackFunction _doubleClickFunc = nullptr;
    callbackFunction _multiClickFunc = nullptr;
    callbackFunction _longPressStartFunc = nullptr;

    enum stateMachine_t : int {
        OCS_INIT = 0,
        OCS_DOWN = 1,
        OCS_UP = 2,
        OCS_COUNT = 3,
        OCS_PRESS = 6,
        OCS_PRESSEND = 7,
    };

    void _fsm(bool activeLevel);
    void _newState(stateMachine_t nextState);

    stateMachine_t _state = OCS_INIT;

    int debouncedPinLevel = -1;
    int _lastDebouncePinLevel = -1;
    uint32_t _lastDebounceTime = 0;
    uint32_t _startTime = 0;
    int _nClicks = 0;

    int debounce(int value);
    bool readPinNow();
    uint32_t getTickNow();

public:
    stateMachine_t state() const { return _state; }
    int debouncedValue() const { return debouncedPinLevel; }
};
