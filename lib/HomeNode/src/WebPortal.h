#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include <functional>
#include <vector>

#include "Settings.h"


class WebPortal
{
public:

    // =================================================
    // Construction and setup
    // =================================================

    WebPortal(
        WebServer& server,
        Settings& settings
    );

    void begin();


    // =================================================
    // General configuration
    // =================================================

    // Sets the title shown in the web interface.
    void setTitle(
        const char* title
    );

    // Sets the password used for firmware updates.
    void setOtaPassword(
        const char* password
    );


    // =================================================
    // Settings fields
    // =================================================

    // Adds a persistent text setting.
    void addTextSetting(
        const char* key,
        const char* label,
        const char* defaultValue
    );

    // Adds a persistent numeric setting.
    void addNumberSetting(
        const char* key,
        const char* label,
        uint32_t defaultValue,
        uint32_t minValue,
        uint32_t maxValue
    );


    // =================================================
    // Live status
    // =================================================

    // Adds a live status value to the dashboard.
    // The getter is called whenever the value is needed.
    void addStatus(
        const char* label,
        std::function<String()> getter
    );


    // =================================================
    // Callbacks
    // =================================================

    // Called after settings have been saved.
    void onSettingsSaved(
        std::function<void()> callback
    );


private:

    // =================================================
    // Internal data types
    // =================================================

    enum class FieldType
    {
        Text,
        Number
    };

    struct SettingField
    {
        FieldType type;

        String key;
        String label;
        String defaultValue;

        uint32_t minValue = 0;
        uint32_t maxValue = 0;
    };

    struct StatusField
    {
        String label;

        std::function<String()> getter;
    };


    // =================================================
    // Web routes and page generation
    // =================================================

    // Generates the main dashboard.
    void showHomePage();

    // Handles submitted settings.
    void saveSettings();

    // Returns current status values as JSON.
    void showStatusJson();

    // Registers firmware update routes.
    void setupFirmwareUpdate();


    // =================================================
    // Authentication
    // =================================================

    // Checks authentication for firmware updates.
    bool otaAuthorized();


    // =================================================
    // Encoding helpers
    // =================================================

    // Escapes text before inserting it into HTML.
    String htmlEscape(
        const String& input
    );

    // Escapes text before inserting it into JSON.
    String jsonEscape(
        const String& input
    );


    // =================================================
    // Dependencies
    // =================================================

    WebServer& _server;
    Settings& _settings;


    // =================================================
    // Web portal configuration
    // =================================================

    String _title = "HomeNode";
    String _otaPassword;


    // =================================================
    // Registered fields
    // =================================================

    std::vector<SettingField> _settingFields;
    std::vector<StatusField> _statusFields;


    // =================================================
    // Callbacks
    // =================================================

    std::function<void()> _settingsSavedCallback;
};