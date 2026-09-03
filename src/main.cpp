//
// Soundbar Parasite
//

#include <Arduino.h>

#include <HomeNode.h>

#include "secrets.h"

#include "SoundbarController.h"
#include "TvDetector.h"

// =====================================================
// Hardware
// =====================================================

// GPIO0 drives the optocoupler connected
// across the soundbar power button.
constexpr uint8_t POWER_BUTTON_PIN = 0;

// GPIO1 reads the divided USB 5 V signal.
// HIGH = soundbar on
// LOW  = soundbar off
constexpr uint8_t SOUNDBAR_STATUS_PIN = 1;

// =====================================================
// Default project settings
// =====================================================

constexpr char DEFAULT_TV_IP[] =
    "192.168.0.90";

constexpr uint32_t DEFAULT_TV_CHECK_MS =
    1000;

// These strings are NVS keys.
// Keep them short.
constexpr char SETTING_TV_IP[] =
    "tv_ip";

constexpr char SETTING_TV_CHECK_MS[] =
    "tv_check_ms";

// =====================================================
// Components
// =====================================================

// Generic IoT platform.
HomeNode node(
    "soundbar-parasite",
    "Soundbar Parasite"
);

// Soundbar-specific hardware controller.
SoundbarController soundbar(
    POWER_BUTTON_PIN,
    SOUNDBAR_STATUS_PIN
);

// Samsung TV network detector.
TvDetector tv;

// =====================================================
// Apply saved settings
// =====================================================

void applySettings()
{
    // -------------------------------------------------
    // TV IP address
    // -------------------------------------------------

    String tvIpString =
        node.settings().getString(
            SETTING_TV_IP,
            DEFAULT_TV_IP
        );

    IPAddress tvIp;

    if (
        !tvIp.fromString(
            tvIpString
        )
    )
    {
        Serial.println(
            "Invalid saved TV IP, using default"
        );

        tvIp.fromString(
            DEFAULT_TV_IP
        );

        node.settings().setString(
            SETTING_TV_IP,
            DEFAULT_TV_IP
        );
    }

    // -------------------------------------------------
    // TV polling interval
    // -------------------------------------------------

    uint32_t checkInterval =
        node.settings().getUInt(
            SETTING_TV_CHECK_MS,
            DEFAULT_TV_CHECK_MS
        );

    if (
        checkInterval < 500 ||
        checkInterval > 60000
    )
    {
        checkInterval =
            DEFAULT_TV_CHECK_MS;

        node.settings().setUInt(
            SETTING_TV_CHECK_MS,
            checkInterval
        );
    }

    // Apply changes without rebooting.
    tv.setTvIp(
        tvIp
    );

    tv.setCheckInterval(
        checkInterval
    );
}

// =====================================================
// Setup
// =====================================================

void setup()
{
    Serial.begin(
        115200
    );

    delay(1000);

    Serial.println();
    Serial.println(
        "Starting Soundbar Parasite..."
    );

    // =================================================
    // Soundbar hardware
    // =================================================

    soundbar.begin();

    // =================================================
    // Project-specific web settings
    // =================================================

    node.web().addTextSetting(
        SETTING_TV_IP,
        "TV IP address",
        DEFAULT_TV_IP
    );

    node.web().addNumberSetting(
        SETTING_TV_CHECK_MS,
        "TV check interval (ms)",
        DEFAULT_TV_CHECK_MS,
        500,
        60000
    );

    // =================================================
    // Project-specific live status
    // =================================================

    node.web().addStatus(
        "TV",
        []()
        {
            if (
                !tv.isStatusKnown()
            )
            {
                return String(
                    "UNKNOWN"
                );
            }

            return tv.isOn()
                ? String("ON")
                : String("OFF");
        }
    );

    node.web().addStatus(
        "Soundbar",
        []()
        {
            return soundbar.isOn()
                ? String("ON")
                : String("OFF");
        }
    );

    node.web().addStatus(
        "Managed by TV",
        []()
        {
            return soundbar.isManagedByTv()
                ? String("YES")
                : String("NO");
        }
    );

    // Reapply project settings immediately
    // whenever Save is pressed on the website.
    node.web().onSettingsSaved(
        []()
        {
            applySettings();
        }
    );

    // =================================================
    // Start generic IoT platform
    // =================================================

    node.begin(
        WIFI_SSID,
        WIFI_PASSWORD,
        OTA_PASSWORD
    );

    // =================================================
    // Start TV detector
    // =================================================

    String tvIpString =
        node.settings().getString(
            SETTING_TV_IP,
            DEFAULT_TV_IP
        );

    IPAddress tvIp;

    if (
        !tvIp.fromString(
            tvIpString
        )
    )
    {
        tvIp.fromString(
            DEFAULT_TV_IP
        );
    }

    const uint32_t interval =
        node.settings().getUInt(
            SETTING_TV_CHECK_MS,
            DEFAULT_TV_CHECK_MS
        );

    tv.begin(
        tvIp,
        interval
    );

    Serial.println(
        "Startup complete"
    );
}

// =====================================================
// Main loop
// =====================================================

void loop()
{
    // Generic IoT infrastructure:
    // Wi-Fi, web server, mDNS and OTA.
    node.update();

    // Project-specific TV detection.
    tv.update();

    // Project-specific soundbar automation.
    soundbar.update(
        tv.isStatusKnown(),
        tv.isOn()
    );
}