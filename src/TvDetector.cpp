#include "TvDetector.h"

#include <WiFi.h>

namespace
{
    constexpr uint16_t TV_PORTS[] =
    {
        8001,
        8002
    };
}

void TvDetector::begin(
    const IPAddress& tvIp,
    unsigned long checkIntervalMs
)
{
    _tvIp = tvIp;
    _checkIntervalMs = checkIntervalMs;

    Serial.print("TV detector started for ");
    Serial.println(_tvIp);
}

void TvDetector::update()
{
    const unsigned long now =
        millis();

    if (
        now - _lastCheckTime <
        _checkIntervalMs
    )
    {
        return;
    }

    _lastCheckTime = now;

    // Do not interpret Wi-Fi loss as TV off.
    if (
        WiFi.status() !=
        WL_CONNECTED
    )
    {
        return;
    }

    const bool reachable =
        respondsOverNetwork();

    // -------------------------------------------------
    // TV responded
    // -------------------------------------------------

    if (reachable)
    {
        _failedChecks = 0;

        if (
            _successfulChecks <
            REQUIRED_ON_CHECKS
        )
        {
            _successfulChecks++;
        }

        if (
            _successfulChecks >=
            REQUIRED_ON_CHECKS
        )
        {
            if (
                !_statusKnown ||
                !_isOn
            )
            {
                _statusKnown = true;
                _isOn = true;

                Serial.println(
                    "TV status changed to: ON"
                );
            }
        }
    }

    // -------------------------------------------------
    // TV did not respond
    // -------------------------------------------------

    else
    {
        _successfulChecks = 0;

        if (
            _failedChecks <
            REQUIRED_OFF_CHECKS
        )
        {
            _failedChecks++;
        }

        if (
            _failedChecks >=
            REQUIRED_OFF_CHECKS
        )
        {
            if (
                !_statusKnown ||
                _isOn
            )
            {
                _statusKnown = true;
                _isOn = false;

                Serial.println(
                    "TV status changed to: OFF"
                );
            }
        }
    }
}

bool TvDetector::respondsOnPort(
    uint16_t port
)
{
    WiFiClient client;

    Serial.print("Testing TV port ");
    Serial.print(port);
    Serial.print("... ");

    const bool connected =
        client.connect(
            _tvIp,
            port,
            CONNECT_TIMEOUT_MS
        );

    if (connected)
    {
        Serial.println("responding");

        client.stop();

        return true;
    }

    Serial.println("no response");

    client.stop();

    return false;
}

bool TvDetector::respondsOverNetwork()
{
    for (
        const uint16_t port :
        TV_PORTS
    )
    {
        if (respondsOnPort(port))
        {
            return true;
        }
    }

    return false;
}

void TvDetector::setTvIp(
    const IPAddress& tvIp
)
{
    _tvIp = tvIp;

    // Reset state because we are now
    // monitoring a different address.
    _statusKnown = false;
    _successfulChecks = 0;
    _failedChecks = 0;

    Serial.print("TV IP changed to: ");
    Serial.println(_tvIp);
}

void TvDetector::setCheckInterval(
    unsigned long intervalMs
)
{
    _checkIntervalMs =
        intervalMs;

    Serial.print(
        "TV check interval changed to: "
    );

    Serial.print(
        _checkIntervalMs
    );

    Serial.println(" ms");
}

bool TvDetector::isOn() const
{
    return _isOn;
}

bool TvDetector::isStatusKnown() const
{
    return _statusKnown;
}