#pragma once

#include <Arduino.h>
#include <IPAddress.h>

class TvDetector
{
public:
    void begin(
        const IPAddress& tvIp,
        unsigned long checkIntervalMs
    );

    void update();

    void setTvIp(
        const IPAddress& tvIp
    );

    void setCheckInterval(
        unsigned long intervalMs
    );

    bool isOn() const;

    bool isStatusKnown() const;

private:
    bool respondsOverNetwork();
    bool respondsOnPort(uint16_t port);

    IPAddress _tvIp;

    unsigned long _checkIntervalMs = 1000;
    unsigned long _lastCheckTime = 0;

    uint8_t _successfulChecks = 0;
    uint8_t _failedChecks = 0;

    bool _statusKnown = false;
    bool _isOn = false;

    static constexpr int CONNECT_TIMEOUT_MS = 700;

    static constexpr uint8_t REQUIRED_ON_CHECKS = 2;
    static constexpr uint8_t REQUIRED_OFF_CHECKS = 3;
};