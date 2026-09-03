#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "Settings.h"
#include "WebPortal.h"

// Generic foundation for small ESP32 devices.
//
// Provides:
// - Wi-Fi
// - automatic reconnect
// - web server
// - persistent settings
// - mDNS
// - browser firmware updates
// - Arduino / PlatformIO OTA
class HomeNode
{
public:
    HomeNode(
        const char* hostname,
        const char* displayName = nullptr
    );

    void begin(
        const char* wifiSsid,
        const char* wifiPassword,
        const char* otaPassword
    );

    void update();

    bool isConnected() const;

    String ipAddress() const;

    int32_t rssi() const;

    const String& hostname() const;

    Settings& settings();

    WebPortal& web();

private:
    void handleWifi();

    void startMdns();

    void startArduinoOta();

    String _hostname;
    String _displayName;

    String _wifiSsid;
    String _wifiPassword;
    String _otaPassword;

    Settings _settings;

    WebServer _server;

    WebPortal _web;

    bool _previousWifiState = false;

    bool _serverStarted = false;
    bool _mdnsStarted = false;
    bool _arduinoOtaStarted = false;

    unsigned long _lastReconnectAttempt = 0;

    static constexpr unsigned long
        WIFI_RECONNECT_INTERVAL_MS = 10000;
};