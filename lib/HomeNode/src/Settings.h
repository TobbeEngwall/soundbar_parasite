#pragma once

#include <Arduino.h>
#include <Preferences.h>

// Simple persistent key/value storage using ESP32 NVS.
//
// This class is generic and can be reused by any HomeNode project.
class Settings
{
public:
    bool begin(const char* namespaceName = "homenode");

    String getString(
        const char* key,
        const String& defaultValue
    );

    uint32_t getUInt(
        const char* key,
        uint32_t defaultValue
    );

    void setString(
        const char* key,
        const String& value
    );

    void setUInt(
        const char* key,
        uint32_t value
    );

private:
    Preferences _preferences;
    bool _ready = false;
};