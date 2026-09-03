#include "Settings.h"

bool Settings::begin(const char* namespaceName)
{
    _ready = _preferences.begin(
        namespaceName,
        false
    );

    if (!_ready)
    {
        Serial.println(
            "Failed to open settings storage"
        );
    }

    return _ready;
}

String Settings::getString(
    const char* key,
    const String& defaultValue
)
{
    if (!_ready)
    {
        return defaultValue;
    }

    return _preferences.getString(
        key,
        defaultValue
    );
}

uint32_t Settings::getUInt(
    const char* key,
    uint32_t defaultValue
)
{
    if (!_ready)
    {
        return defaultValue;
    }

    return _preferences.getULong(
        key,
        defaultValue
    );
}

void Settings::setString(
    const char* key,
    const String& value
)
{
    if (!_ready)
    {
        return;
    }

    _preferences.putString(
        key,
        value
    );
}

void Settings::setUInt(
    const char* key,
    uint32_t value
)
{
    if (!_ready)
    {
        return;
    }

    _preferences.putULong(
        key,
        value
    );
}