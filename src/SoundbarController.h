#pragma once

#include <Arduino.h>

class SoundbarController
{
public:
    SoundbarController(
        uint8_t powerButtonPin,
        uint8_t statusPin
    );

    void begin();

    // Call with the current TV state.
    void update(
        bool tvStatusKnown,
        bool tvIsOn
    );

    bool isOn() const;

    bool isManagedByTv() const;

private:
    void pressPowerButton(
        const char* reason
    );

    uint8_t _powerButtonPin;
    uint8_t _statusPin;

    bool _managedByTv = false;
    bool _automaticStartPending = false;

    unsigned long _lastPowerPressTime = 0;
    unsigned long _automaticStartAttemptTime = 0;

    static constexpr unsigned long BUTTON_PRESS_MS = 200;

    static constexpr unsigned long POWER_RETRY_MS = 15000;

    static constexpr unsigned long START_CONFIRM_TIMEOUT_MS = 10000;
};