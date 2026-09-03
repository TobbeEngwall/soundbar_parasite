#include "WebPortal.h"

#include <Update.h>

#include "WebAssets.h"


// =====================================================
// Internal helpers
// =====================================================

namespace
{

String statusClassForValue(
    const String& value
)
{
    String normalized =
        value;

    normalized.toUpperCase();

    String cssClass =
        "status-value";

    if (
        normalized == "ON" ||
        normalized == "YES" ||
        normalized == "CONNECTED"
    )
    {
        cssClass +=
            " status-good";
    }
    else if (
        normalized == "OFF" ||
        normalized == "NO" ||
        normalized == "DISCONNECTED"
    )
    {
        cssClass +=
            " status-off";
    }
    else if (
        normalized == "UNKNOWN"
    )
    {
        cssClass +=
            " status-warning";
    }

    return cssClass;
}

}


// =====================================================
// Constructor
// =====================================================

WebPortal::WebPortal(
    WebServer& server,
    Settings& settings
)
    : _server(server),
      _settings(settings)
{
}


// =====================================================
// Configuration
// =====================================================

void WebPortal::setTitle(
    const char* title
)
{
    _title = title;
}


void WebPortal::setOtaPassword(
    const char* password
)
{
    _otaPassword = password;
}


// =====================================================
// Settings registration
// =====================================================

void WebPortal::addTextSetting(
    const char* key,
    const char* label,
    const char* defaultValue
)
{
    SettingField field;

    field.type =
        FieldType::Text;

    field.key =
        key;

    field.label =
        label;

    field.defaultValue =
        defaultValue;

    _settingFields.push_back(
        field
    );
}


void WebPortal::addNumberSetting(
    const char* key,
    const char* label,
    uint32_t defaultValue,
    uint32_t minValue,
    uint32_t maxValue
)
{
    SettingField field;

    field.type =
        FieldType::Number;

    field.key =
        key;

    field.label =
        label;

    field.defaultValue =
        String(defaultValue);

    field.minValue =
        minValue;

    field.maxValue =
        maxValue;

    _settingFields.push_back(
        field
    );
}


// =====================================================
// Status registration
// =====================================================

void WebPortal::addStatus(
    const char* label,
    std::function<String()> getter
)
{
    StatusField field;

    field.label =
        label;

    field.getter =
        getter;

    _statusFields.push_back(
        field
    );
}


// =====================================================
// Callbacks
// =====================================================

void WebPortal::onSettingsSaved(
    std::function<void()> callback
)
{
    _settingsSavedCallback =
        callback;
}


// =====================================================
// Start web interface
// =====================================================

void WebPortal::begin()
{
    // Main dashboard.
    _server.on(
        "/",
        HTTP_GET,
        [this]()
        {
            showHomePage();
        }
    );

    // Settings form.
    _server.on(
        "/save",
        HTTP_POST,
        [this]()
        {
            saveSettings();
        }
    );

    // Live dashboard status.
    _server.on(
        "/api/status",
        HTTP_GET,
        [this]()
        {
            showStatusJson();
        }
    );

    // Firmware update routes.
    setupFirmwareUpdate();
}


// =====================================================
// Main page
// =====================================================

void WebPortal::showHomePage()
{
    String html;

    html.reserve(
        7000
    );


    // =================================================
    // Document head
    // =================================================

    html += R"rawliteral(
<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta
    name="viewport"
    content="width=device-width, initial-scale=1">

<meta
    name="theme-color"
    content="#0b1014">

<title>
)rawliteral";

    html +=
        htmlEscape(_title);

    html += R"rawliteral(
</title>

<style>
)rawliteral";

    html +=
        HOME_NODE_CSS;

    html += R"rawliteral(
</style>

</head>

<body>

<div class="container">
)rawliteral";


    // =================================================
    // Header
    // =================================================

    html += R"rawliteral(

<div class="header">

<div class="brand-row">

<div class="brand-dot"></div>

<h1>
)rawliteral";

    html +=
        htmlEscape(_title);

    html += R"rawliteral(
</h1>

</div>

<div class="subtitle">
Local device control
</div>

</div>
)rawliteral";


    // =================================================
    // Status card
    // =================================================

    html += R"rawliteral(

<div class="card">

<div class="card-title">

<div class="card-dot"></div>

<h2>Status</h2>

</div>
)rawliteral";

    for (
        size_t i = 0;
        i < _statusFields.size();
        i++
    )
    {
        const StatusField& field =
            _statusFields[i];

        const String value =
            field.getter();

        const String valueClass =
            statusClassForValue(
                value
            );

        html += R"rawliteral(

<div class="status-row">

<span class="status-label">
)rawliteral";

        html +=
            htmlEscape(
                field.label
            );

        html += R"rawliteral(
</span>

<span
    class="
)rawliteral";

        html +=
            valueClass;

        html += R"rawliteral("
    data-status-index="
)rawliteral";

        html +=
            String(i);

        html += R"rawliteral(
">
)rawliteral";

        html +=
            htmlEscape(
                value
            );

        html += R"rawliteral(
</span>

</div>
)rawliteral";
    }

    html += R"rawliteral(

</div>
)rawliteral";


    // =================================================
    // Settings card
    // =================================================

    html += R"rawliteral(

<div class="card">

<div class="card-title">

<div class="card-dot"></div>

<h2>Settings</h2>

</div>

<form
    method="POST"
    action="/save">
)rawliteral";

    for (
        const SettingField& field :
        _settingFields
    )
    {
        html += R"rawliteral(

<div class="form-group">

<label>
)rawliteral";

        html +=
            htmlEscape(
                field.label
            );

        html += R"rawliteral(
</label>
)rawliteral";


        // =============================================
        // Text field
        // =============================================

        if (
            field.type ==
            FieldType::Text
        )
        {
            const String value =
                _settings.getString(
                    field.key.c_str(),
                    field.defaultValue
                );

            html += R"rawliteral(

<input
    type="text"
    name="
)rawliteral";

            html +=
                htmlEscape(
                    field.key
                );

            html += R"rawliteral("
    value="
)rawliteral";

            html +=
                htmlEscape(
                    value
                );

            html += R"rawliteral(
">
)rawliteral";
        }


        // =============================================
        // Number field
        // =============================================

        else
        {
            const uint32_t defaultValue =
                field.defaultValue.toInt();

            const uint32_t value =
                _settings.getUInt(
                    field.key.c_str(),
                    defaultValue
                );

            html += R"rawliteral(

<input
    type="number"
    name="
)rawliteral";

            html +=
                htmlEscape(
                    field.key
                );

            html += R"rawliteral("
    value="
)rawliteral";

            html +=
                String(value);

            html += R"rawliteral("
    min="
)rawliteral";

            html +=
                String(
                    field.minValue
                );

            html += R"rawliteral("
    max="
)rawliteral";

            html +=
                String(
                    field.maxValue
                );

            html += R"rawliteral(
">
)rawliteral";
        }

        html += R"rawliteral(

</div>
)rawliteral";
    }

    html += R"rawliteral(

<button type="submit">
Save settings
</button>

</form>

</div>
)rawliteral";


    // =================================================
    // Firmware card
    // =================================================

    html += R"rawliteral(

<div class="card">

<div class="card-title">

<div class="card-dot"></div>

<h2>Firmware</h2>

</div>

<p class="description">
Upload a new firmware build directly to this device.
</p>

<a
    class="button button-secondary"
    href="/update">

Update firmware

</a>

</div>
)rawliteral";


    // =================================================
    // Footer
    // =================================================

    html += R"rawliteral(

<div class="footer">
HomeNode
</div>

</div>
)rawliteral";


    // =================================================
    // Live status JavaScript
    // =================================================

    html += R"rawliteral(

<script>
)rawliteral";

    html +=
        LIVE_STATUS_JS;

    html += R"rawliteral(
</script>

</body>

</html>
)rawliteral";


    // =================================================
    // Send page
    // =================================================

    _server.send(
        200,
        "text/html",
        html
    );
}


// =====================================================
// Live status API
// =====================================================

void WebPortal::showStatusJson()
{
    String json;

    json.reserve(
        512
    );

    json +=
        "{\"values\":[";

    for (
        size_t i = 0;
        i < _statusFields.size();
        i++
    )
    {
        if (
            i > 0
        )
        {
            json += ",";
        }

        const String value =
            _statusFields[i]
                .getter();

        json += "\"";

        json +=
            jsonEscape(
                value
            );

        json += "\"";
    }

    json += "]}";


    // Prevent browsers from caching live state.
    _server.sendHeader(
        "Cache-Control",
        "no-store"
    );

    _server.send(
        200,
        "application/json",
        json
    );
}


// =====================================================
// Save settings
// =====================================================

void WebPortal::saveSettings()
{
    for (
        const SettingField& field :
        _settingFields
    )
    {
        if (
            !_server.hasArg(
                field.key
            )
        )
        {
            continue;
        }

        const String value =
            _server.arg(
                field.key
            );


        // =============================================
        // Text setting
        // =============================================

        if (
            field.type ==
            FieldType::Text
        )
        {
            _settings.setString(
                field.key.c_str(),
                value
            );

            continue;
        }


        // =============================================
        // Numeric setting
        // =============================================

        char* endPointer =
            nullptr;

        const long number =
            strtol(
                value.c_str(),
                &endPointer,
                10
            );

        if (
            endPointer ==
                value.c_str() ||

            *endPointer != '\0' ||

            number < 0 ||

            static_cast<uint32_t>(
                number
            ) < field.minValue ||

            static_cast<uint32_t>(
                number
            ) > field.maxValue
        )
        {
            _server.send(
                400,
                "text/plain",
                "Invalid value for: " +
                    field.label
            );

            return;
        }

        _settings.setUInt(
            field.key.c_str(),
            static_cast<uint32_t>(
                number
            )
        );
    }

    Serial.println(
        "Settings saved from web interface"
    );


    // Apply new settings immediately if the
    // project registered a callback.
    if (
        _settingsSavedCallback
    )
    {
        _settingsSavedCallback();
    }


    // Return to the main page.
    _server.sendHeader(
        "Location",
        "/"
    );

    _server.send(
        303,
        "text/plain",
        ""
    );
}


// =====================================================
// OTA authentication
// =====================================================

bool WebPortal::otaAuthorized()
{
    if (
        _otaPassword.length() ==
        0
    )
    {
        return true;
    }

    return _server.authenticate(
        "admin",
        _otaPassword.c_str()
    );
}


// =====================================================
// Firmware update
// =====================================================

void WebPortal::setupFirmwareUpdate()
{
    // =================================================
    // Firmware page
    // =================================================

    _server.on(
        "/update",
        HTTP_GET,

        [this]()
        {
            if (
                !otaAuthorized()
            )
            {
                _server.requestAuthentication();

                return;
            }


            String page;

            page.reserve(
                5000
            );


            // =========================================
            // Document head
            // =========================================

            page += R"rawliteral(
<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta
    name="viewport"
    content="width=device-width, initial-scale=1">

<meta
    name="theme-color"
    content="#0b1014">

<title>Firmware Update</title>

<style>
)rawliteral";

            page +=
                HOME_NODE_CSS;

            page += R"rawliteral(
</style>

</head>

<body>

<div class="container">
)rawliteral";


            // =========================================
            // Header
            // =========================================

            page += R"rawliteral(

<div class="header">

<div class="brand-row">

<div class="brand-dot"></div>

<h1>Firmware Update</h1>

</div>

<div class="subtitle">
HomeNode OTA
</div>

</div>
)rawliteral";


            // =========================================
            // Firmware card
            // =========================================

            page += R"rawliteral(

<div class="card">

<div class="card-title">

<div class="card-dot"></div>

<h2>Upload firmware</h2>

</div>

<p class="description">
Choose a compiled firmware .bin file.
The device will restart automatically after a successful update.
</p>

<form
    method="POST"
    action="/update"
    enctype="multipart/form-data">

<input
    type="file"
    name="firmware"
    accept=".bin"
    required>

<button type="submit">
Upload firmware
</button>

</form>

<a
    class="back-link"
    href="/">
← Back to device
</a>

</div>
)rawliteral";


            // =========================================
            // Footer
            // =========================================

            page += R"rawliteral(

<div class="footer">
HomeNode
</div>

</div>

</body>

</html>
)rawliteral";


            _server.send(
                200,
                "text/html",
                page
            );
        }
    );


    // =================================================
    // Firmware upload
    // =================================================

    _server.on(
        "/update",
        HTTP_POST,


        // =============================================
        // Upload completed
        // =============================================

        [this]()
        {
            if (
                !otaAuthorized()
            )
            {
                _server.requestAuthentication();

                return;
            }

            if (
                Update.hasError()
            )
            {
                _server.send(
                    500,
                    "text/plain",
                    "Firmware update failed"
                );

                return;
            }

            _server.send(
                200,
                "text/plain",
                "Update complete. Rebooting..."
            );

            delay(
                1000
            );

            ESP.restart();
        },


        // =============================================
        // Incoming firmware data
        // =============================================

        [this]()
        {
            if (
                !otaAuthorized()
            )
            {
                return;
            }

            HTTPUpload& upload =
                _server.upload();


            // -----------------------------------------
            // Upload started
            // -----------------------------------------

            if (
                upload.status ==
                UPLOAD_FILE_START
            )
            {
                Serial.print(
                    "Firmware upload started: "
                );

                Serial.println(
                    upload.filename
                );

                if (
                    !Update.begin(
                        UPDATE_SIZE_UNKNOWN
                    )
                )
                {
                    Update.printError(
                        Serial
                    );
                }
            }


            // -----------------------------------------
            // Firmware data
            // -----------------------------------------

            else if (
                upload.status ==
                UPLOAD_FILE_WRITE
            )
            {
                const size_t written =
                    Update.write(
                        upload.buf,
                        upload.currentSize
                    );

                if (
                    written !=
                    upload.currentSize
                )
                {
                    Update.printError(
                        Serial
                    );
                }
            }


            // -----------------------------------------
            // Upload finished
            // -----------------------------------------

            else if (
                upload.status ==
                UPLOAD_FILE_END
            )
            {
                if (
                    Update.end(
                        true
                    )
                )
                {
                    Serial.print(
                        "Firmware upload complete: "
                    );

                    Serial.print(
                        upload.totalSize
                    );

                    Serial.println(
                        " bytes"
                    );
                }
                else
                {
                    Update.printError(
                        Serial
                    );
                }
            }
        }
    );
}


// =====================================================
// HTML escaping
// =====================================================

String WebPortal::htmlEscape(
    const String& input
)
{
    String output =
        input;

    output.replace(
        "&",
        "&amp;"
    );

    output.replace(
        "<",
        "&lt;"
    );

    output.replace(
        ">",
        "&gt;"
    );

    output.replace(
        "\"",
        "&quot;"
    );

    return output;
}


// =====================================================
// JSON escaping
// =====================================================

String WebPortal::jsonEscape(
    const String& input
)
{
    String output;

    output.reserve(
        input.length() + 8
    );

    for (
        size_t i = 0;
        i < input.length();
        i++
    )
    {
        const char c =
            input[i];

        switch (c)
        {
            case '\\':
                output += "\\\\";
                break;

            case '"':
                output += "\\\"";
                break;

            case '\n':
                output += "\\n";
                break;

            case '\r':
                output += "\\r";
                break;

            case '\t':
                output += "\\t";
                break;

            default:
                output += c;
                break;
        }
    }

    return output;
}