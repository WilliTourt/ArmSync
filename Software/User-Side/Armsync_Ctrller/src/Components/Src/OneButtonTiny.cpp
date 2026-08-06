#include "OneButtonTiny.h"

// ---- OneButtonTiny ----

OneButtonTiny::OneButtonTiny(bool (*readPin)(), uint32_t (*getTick)())
    : _readPin(readPin), _getTick(getTick)
{
}

void OneButtonTiny::tick()
{
    tick(readPinNow());
}

void OneButtonTiny::tick(bool activeLevel)
{
    uint32_t now = getTickNow();

    // debounce
    if (activeLevel != (_lastDebouncePinLevel != 0))
    {
        _lastDebounceTime = now;
        _lastDebouncePinLevel = activeLevel ? 1 : 0;
    }

    if (now - _lastDebounceTime >= _debounce_ms)
    {
        debouncedPinLevel = _lastDebouncePinLevel;
    }

    _fsm(debouncedPinLevel != 0);
}

void OneButtonTiny::reset()
{
    _state = OCS_INIT;
    _nClicks = 0;
    _startTime = 0;
}

bool OneButtonTiny::readPinNow()
{
    return _readPin ? _readPin() : false;
}

uint32_t OneButtonTiny::getTickNow()
{
    return _getTick ? _getTick() : 0;
}

// ---- FSM ----

void OneButtonTiny::_newState(stateMachine_t nextState)
{
    _state = nextState;
}

void OneButtonTiny::_fsm(bool activeLevel)
{
    uint32_t waitTime = (getTickNow() - _startTime);

    // Implementation of the state machine
    switch (_state)
    {
    case OCS_INIT:
        if (activeLevel)
        {
            _newState(OCS_DOWN);
            _startTime = getTickNow();
            _nClicks = 0;
        }
        break;

    case OCS_DOWN:
        if (!activeLevel)
        {
            _newState(OCS_UP);
            _startTime = getTickNow();
        }
        else if (activeLevel && getTickNow() - _startTime > _press_ms)
        {
            if (_longPressStartFunc) _longPressStartFunc();
            _nClicks = 0;
            _newState(OCS_PRESS);
            _startTime = getTickNow();
        }
        break;

    case OCS_UP:
        if (activeLevel)
        {
            _nClicks++;
            _newState(OCS_DOWN);
            _startTime = getTickNow();
        }
        else if (!activeLevel && getTickNow() - _startTime > _click_ms)
        {
            if (_nClicks == 1)
            {
                if (_clickFunc) _clickFunc();
            }
            else if (_nClicks == 2)
            {
                if (_doubleClickFunc) _doubleClickFunc();
            }
            else if (_nClicks > 2)
            {
                if (_multiClickFunc) _multiClickFunc();
            }
            _newState(OCS_INIT);
        }
        break;

    case OCS_PRESS:
        if (!activeLevel)
        {
            _newState(OCS_PRESSEND);
            _startTime = getTickNow();
        }
        break;

    case OCS_PRESSEND:
        if (!activeLevel && getTickNow() - _startTime > _click_ms)
        {
            _newState(OCS_INIT);
        }
        break;

    default:
        _newState(OCS_INIT);
        break;
    }
}
