#include "SoundbarController.h"

SoundbarController::SoundbarController(
    uint8_t powerButtonPin,
    uint8_t statusPin
)
    : _powerButtonPin(powerButtonPin),
      _statusPin(statusPin)
{
}

void SoundbarController::begin()
{
    // Optocoupler controlling the physical power button.
    pinMode(
        _powerButtonPin,
        OUTPUT
    );

    digitalWrite(
        _powerButtonPin,
        LOW
    );

    // The external voltage divider handles
    // pull-down for the status signal.
    pinMode(
        _statusPin,
        INPUT
    );

    // Allow immediate button press after boot.
    _lastPowerPressTime =
        millis() - POWER_RETRY_MS;

    Serial.println(
        "Soundbar controller started"
    );
}

bool SoundbarController::isOn() const
{
    return digitalRead(
        _statusPin
    ) == HIGH;
}

bool SoundbarController::isManagedByTv() const
{
    return _managedByTv;
}

void SoundbarController::pressPowerButton(
    const char* reason
)
{
    Serial.print(
        "Pressing soundbar power button: "
    );

    Serial.println(reason);

    digitalWrite(
        _powerButtonPin,
        HIGH
    );

    delay(BUTTON_PRESS_MS);

    digitalWrite(
        _powerButtonPin,
        LOW
    );

    _lastPowerPressTime =
        millis();
}

void SoundbarController::update(
    bool tvStatusKnown,
    bool tvIsOn
)
{
    // Never make decisions until the TV detector
    // has established a reliable state.
    if (!tvStatusKnown)
    {
        return;
    }

    const unsigned long now =
        millis();

    const bool soundbarOn =
        isOn();

    // =================================================
    // Confirm automatic startup
    // =================================================

    if (_automaticStartPending)
    {
        if (soundbarOn)
        {
            _automaticStartPending =
                false;

            Serial.println(
                "Automatic soundbar start confirmed"
            );
        }

        else if (
            now -
            _automaticStartAttemptTime >=
            START_CONFIRM_TIMEOUT_MS
        )
        {
            _automaticStartPending =
                false;

            _managedByTv =
                false;

            Serial.println(
                "Automatic soundbar start failed"
            );
        }
    }

    // =================================================
    // TV ON
    // =================================================

    if (tvIsOn)
    {
        // Start the soundbar only when it is off.
        if (
            !soundbarOn &&
            !_automaticStartPending &&
            now - _lastPowerPressTime >=
            POWER_RETRY_MS
        )
        {
            // Because the ESP started this session,
            // it may also turn it off later.
            _managedByTv = true;

            _automaticStartPending =
                true;

            _automaticStartAttemptTime =
                now;

            pressPowerButton(
                "TV is on and soundbar is off"
            );
        }

        return;
    }

    // =================================================
    // TV OFF
    // =================================================

    // A manually started soundbar must remain untouched.
    if (!_managedByTv)
    {
        return;
    }

    // If this soundbar session was started by the ESP,
    // turn it off when the TV turns off.
    if (
        soundbarOn &&
        now - _lastPowerPressTime >=
        POWER_RETRY_MS
    )
    {
        _automaticStartPending =
            false;

        pressPowerButton(
            "TV is off and soundbar is TV-managed"
        );

        return;
    }

    // GPIO confirms that the soundbar is now off.
    // Release ownership of this session.
    if (
        !soundbarOn &&
        !_automaticStartPending
    )
    {
        _managedByTv = false;

        Serial.println(
            "Automatic soundbar shutdown confirmed"
        );
    }
}