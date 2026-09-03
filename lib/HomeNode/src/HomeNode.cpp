#include "HomeNode.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

HomeNode::HomeNode(
    const char* hostname,
    const char* displayName
)
    : _hostname(hostname),
      _displayName(
          displayName != nullptr
              ? displayName
              : hostname
      ),
      _server(80),
      _web(
          _server,
          _settings
      )
{
    _web.setTitle(
        _displayName.c_str()
    );
}

void HomeNode::begin(
    const char* wifiSsid,
    const char* wifiPassword,
    const char* otaPassword
)
{
    _wifiSsid = wifiSsid;
    _wifiPassword = wifiPassword;
    _otaPassword = otaPassword;

    // Persistent ESP32 NVS storage.
    _settings.begin(
        "homenode"
    );

    _web.setOtaPassword(
        otaPassword
    );

    // Generic status fields shown on every HomeNode.
    _web.addStatus(
        "IP address",
        [this]() -> String
        {
            return ipAddress();
        }
    );

    _web.addStatus(
        "Wi-Fi",
        [this]() -> String
        {
            if (!isConnected())
            {
                return "Disconnected";
            }

            String status = String(rssi());
            status += " dBm";

            return status;
        }
    );

    // Register HTTP routes.
    // The actual server is started after Wi-Fi connects.
    _web.begin();

    // Start Wi-Fi before any network services.
    WiFi.mode(
        WIFI_STA
    );

    WiFi.setHostname(
        _hostname.c_str()
    );

    Serial.print(
        "Connecting to Wi-Fi: "
    );

    Serial.println(
        _wifiSsid
    );

    WiFi.begin(
        _wifiSsid.c_str(),
        _wifiPassword.c_str()
    );
}

void HomeNode::update()
{
    handleWifi();

    // Handle requests to the local web interface.
    _server.handleClient();

    // Handle OTA uploads from development tools.
    if (
        _arduinoOtaStarted &&
        WiFi.status() == WL_CONNECTED
    )
    {
        ArduinoOTA.handle();
    }
}

// =====================================================
// Wi-Fi
// =====================================================

void HomeNode::handleWifi()
{
    const bool connected =
        WiFi.status() ==
        WL_CONNECTED;

    // -------------------------------------------------
    // Newly connected
    // -------------------------------------------------

    if (
    connected &&
    !_previousWifiState
    )
    {
        Serial.println("Wi-Fi connected");

        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        Serial.print("Wi-Fi hostname: ");
        Serial.println(WiFi.getHostname());

        // Start HTTP server only after the
        // network stack is available.
        if (!_serverStarted)
        {
            _server.begin();
            _serverStarted = true;

            Serial.println(
                "Web server started"
            );
        }

        startMdns();
        startArduinoOta();
    }

    // -------------------------------------------------
    // Connection lost
    // -------------------------------------------------

    if (
        !connected &&
        _previousWifiState
    )
    {
        Serial.println("Wi-Fi connection lost");

        if (_mdnsStarted)
        {
            MDNS.end();

            _mdnsStarted =
                false;
        }
    }

    _previousWifiState =
        connected;

    // -------------------------------------------------
    // Reconnect
    // -------------------------------------------------

    if (!connected)
    {
        const unsigned long now =
            millis();

        if (
            now -
            _lastReconnectAttempt >=
            WIFI_RECONNECT_INTERVAL_MS
        )
        {
            _lastReconnectAttempt =
                now;

            Serial.println(
                "Attempting Wi-Fi reconnect..."
            );

            WiFi.disconnect();

            WiFi.begin(
                _wifiSsid.c_str(),
                _wifiPassword.c_str()
            );
        }
    }
}

// =====================================================
// mDNS
// =====================================================

void HomeNode::startMdns()
{
    if (_mdnsStarted)
    {
        return;
    }

    if (
        MDNS.begin(
            _hostname.c_str()
        )
    )
    {
        // Normal HTTP service.
        MDNS.addService(
            "http",
            "tcp",
            80
        );

        // HomeNode discovery service.
        MDNS.addService(
            "homenode",
            "tcp",
            80
        );

        // Information used by automatic discovery.
        MDNS.addServiceTxt(
            "homenode",
            "tcp",
            "name",
            _displayName.c_str()
        );

        MDNS.addServiceTxt(
            "homenode",
            "tcp",
            "hostname",
            _hostname.c_str()
        );

        MDNS.addServiceTxt(
            "homenode",
            "tcp",
            "path",
            "/"
        );

        _mdnsStarted = true;

        Serial.print(
            "Web interface: http://"
        );

        Serial.print(
            _hostname
        );

        Serial.println(
            ".local"
        );

        Serial.print(
            "HomeNode advertised as: "
        );

        Serial.println(
            _displayName
        );
    }
    else
    {
        Serial.println(
            "Failed to start mDNS"
        );
    }
}

// =====================================================
// Arduino / PlatformIO OTA
// =====================================================

void HomeNode::startArduinoOta()
{
    // ArduinoOTA only needs to be initialized once.
    if (_arduinoOtaStarted)
    {
        return;
    }

    ArduinoOTA.setHostname(
        _hostname.c_str()
    );

    if (
        _otaPassword.length() > 0
    )
    {
        ArduinoOTA.setPassword(
            _otaPassword.c_str()
        );
    }

    ArduinoOTA.onStart(
        []()
        {
            Serial.println(
                "OTA update started"
            );
        }
    );

    ArduinoOTA.onEnd(
        []()
        {
            Serial.println(
                "OTA update completed"
            );
        }
    );

    ArduinoOTA.onError(
        [](ota_error_t error)
        {
            Serial.print(
                "OTA error: "
            );

            Serial.println(
                error
            );
        }
    );

    ArduinoOTA.begin();

    _arduinoOtaStarted =
        true;

    Serial.println(
        "OTA service started"
    );
}

// =====================================================
// Public accessors
// =====================================================

bool HomeNode::isConnected() const
{
    return WiFi.status() ==
        WL_CONNECTED;
}

String HomeNode::ipAddress() const
{
    if (!isConnected())
    {
        return "Disconnected";
    }

    return WiFi.localIP().toString();
}

int32_t HomeNode::rssi() const
{
    if (!isConnected())
    {
        return 0;
    }

    return WiFi.RSSI();
}

const String& HomeNode::hostname() const
{
    return _hostname;
}

Settings& HomeNode::settings()
{
    return _settings;
}

WebPortal& HomeNode::web()
{
    return _web;
}