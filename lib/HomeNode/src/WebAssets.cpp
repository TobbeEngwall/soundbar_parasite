#include "WebAssets.h"


// =====================================================
// Shared HomeNode styling
// =====================================================

const char HOME_NODE_CSS[] = R"rawliteral(

:root {
    --bg: #0b1014;
    --bg-top: #102027;

    --surface: #11181e;
    --surface-hover: #151e25;

    --border: #26333d;
    --border-soft: rgba(255, 255, 255, 0.05);

    --text: #f2f5f7;
    --text-muted: #8c9aa5;

    --accent: #4fd1c5;
    --accent-hover: #66ddd3;
    --accent-soft: rgba(79, 209, 197, 0.12);

    --success: #45d483;
    --success-soft: rgba(69, 212, 131, 0.12);

    --warning: #e9ba61;
    --warning-soft: rgba(233, 186, 97, 0.12);

    --danger: #ff6b6b;
    --danger-soft: rgba(255, 107, 107, 0.12);
}


/* ====================================================
   Base
   ==================================================== */

* {
    box-sizing: border-box;
}

html {
    background: var(--bg);
}

body {
    margin: 0;

    min-height: 100vh;

    background:
        radial-gradient(
            circle at top center,
            var(--bg-top) 0,
            var(--bg) 420px
        );

    color: var(--text);

    font-family:
        Inter,
        -apple-system,
        BlinkMacSystemFont,
        "Segoe UI",
        Roboto,
        Helvetica,
        Arial,
        sans-serif;

    font-size: 15px;

    -webkit-font-smoothing: antialiased;
}

.container {
    width:
        min(
            680px,
            calc(100% - 32px)
        );

    margin: 0 auto;

    padding:
        56px
        0
        80px;
}


/* ====================================================
   Header
   ==================================================== */

.header {
    margin-bottom: 30px;
}

.brand-row {
    display: flex;

    align-items: center;

    gap: 11px;
}

.brand-dot {
    width: 10px;
    height: 10px;

    border-radius: 50%;

    background: var(--accent);

    box-shadow:
        0 0 16px
        rgba(79, 209, 197, 0.45);
}

h1 {
    margin: 0;

    font-size: 32px;
    font-weight: 700;

    letter-spacing: -0.8px;
}

.subtitle {
    margin-top: 8px;

    color: var(--text-muted);

    font-size: 13px;
}


/* ====================================================
   Cards
   ==================================================== */

.card {
    background:
        linear-gradient(
            180deg,
            rgba(255, 255, 255, 0.015),
            rgba(255, 255, 255, 0)
        ),
        var(--surface);

    border:
        1px solid
        var(--border);

    border-radius: 14px;

    padding: 24px;

    margin-bottom: 18px;

    box-shadow:
        0 12px 35px
        rgba(0, 0, 0, 0.18);
}

.card-title {
    display: flex;

    align-items: center;

    gap: 10px;

    margin-bottom: 20px;
}

.card-dot {
    width: 7px;
    height: 7px;

    border-radius: 50%;

    background: var(--accent);
}

h2 {
    margin: 0;

    font-size: 19px;
    font-weight: 650;

    letter-spacing: -0.2px;
}


/* ====================================================
   Status
   ==================================================== */

.status-row {
    display: flex;

    justify-content: space-between;
    align-items: center;

    gap: 20px;

    min-height: 39px;

    border-bottom:
        1px solid
        var(--border-soft);
}

.status-row:last-child {
    border-bottom: none;
}

.status-label {
    color: var(--text-muted);

    font-size: 14px;
}

.status-value {
    color: var(--text);

    font-size: 13px;
    font-weight: 600;

    background: var(--surface-hover);

    border:
        1px solid
        var(--border);

    border-radius: 999px;

    padding:
        4px
        9px;

    white-space: nowrap;

    transition:
        color 0.15s,
        background 0.15s,
        border-color 0.15s;
}

.status-good {
    color: var(--success);

    background:
        var(--success-soft);

    border-color:
        rgba(69, 212, 131, 0.25);
}

.status-off {
    color: var(--text-muted);
}

.status-warning {
    color: var(--warning);

    background:
        var(--warning-soft);

    border-color:
        rgba(233, 186, 97, 0.25);
}


/* ====================================================
   Forms
   ==================================================== */

.form-group {
    margin-top: 18px;
}

.form-group:first-child {
    margin-top: 0;
}

label {
    display: block;

    color: var(--text-muted);

    font-size: 13px;
    font-weight: 600;

    margin-bottom: 7px;
}

input {
    display: block;

    width: 100%;

    background: #0a1014;
    color: var(--text);

    border:
        1px solid
        var(--border);

    border-radius: 9px;

    padding:
        12px
        13px;

    font-family: inherit;
    font-size: 14px;

    outline: none;

    transition:
        border-color 0.15s,
        box-shadow 0.15s,
        background 0.15s;
}

input:hover {
    border-color: #354550;
}

input:focus {
    background: #0c1318;

    border-color:
        var(--accent);

    box-shadow:
        0 0 0 3px
        var(--accent-soft);
}


/* Remove browser number arrows where possible */

input[type="number"] {
    appearance: textfield;
}

input[type="number"]::-webkit-inner-spin-button,
input[type="number"]::-webkit-outer-spin-button {
    appearance: none;

    margin: 0;
}


/* ====================================================
   File input
   ==================================================== */

input[type="file"] {
    color: var(--text-muted);

    cursor: pointer;
}

input[type="file"]::file-selector-button {
    background: #182229;

    color: var(--text);

    border:
        1px solid
        var(--border);

    border-radius: 7px;

    padding:
        8px
        12px;

    margin-right: 12px;

    font-family: inherit;
    font-weight: 600;

    cursor: pointer;
}

input[type="file"]::file-selector-button:hover {
    background: #202c34;
}


/* ====================================================
   Buttons
   ==================================================== */

button,
.button {
    display: inline-flex;

    align-items: center;
    justify-content: center;

    gap: 8px;

    margin-top: 20px;

    background: var(--accent);

    color: #061110;

    border: none;

    border-radius: 9px;

    padding:
        11px
        18px;

    font-family: inherit;
    font-size: 14px;
    font-weight: 700;

    text-decoration: none;

    cursor: pointer;

    transition:
        background 0.15s,
        transform 0.1s,
        box-shadow 0.15s;
}

button:hover,
.button:hover {
    background:
        var(--accent-hover);

    box-shadow:
        0 5px 18px
        rgba(79, 209, 197, 0.15);

    text-decoration: none;
}

button:active,
.button:active {
    transform:
        translateY(1px);
}

.button-secondary {
    background:
        var(--surface-hover);

    color:
        var(--text);

    border:
        1px solid
        var(--border);
}

.button-secondary:hover {
    background: #1b262e;

    box-shadow: none;
}


/* ====================================================
   Text and links
   ==================================================== */

.description {
    margin:
        0
        0
        20px;

    color: var(--text-muted);

    font-size: 13px;

    line-height: 1.55;
}

.back-link {
    display: inline-block;

    margin-top: 20px;

    color: var(--accent);

    font-size: 13px;

    text-decoration: none;
}

.back-link:hover {
    text-decoration: underline;
}


/* ====================================================
   Footer
   ==================================================== */

.footer {
    padding-top: 6px;

    color: #61707b;

    font-size: 11px;

    text-align: center;
}


/* ====================================================
   Mobile
   ==================================================== */

@media (max-width: 600px) {

    .container {
        width:
            calc(100% - 24px);

        padding:
            30px
            0
            50px;
    }

    h1 {
        font-size: 27px;
    }

    .card {
        padding: 20px;
    }

    .status-row {
        min-height: 42px;
    }
}

)rawliteral";


// =====================================================
// Live status JavaScript
// =====================================================

const char LIVE_STATUS_JS[] = R"rawliteral(

function updateStatusStyle(element, value)
{
    element.classList.remove(
        "status-good",
        "status-off",
        "status-warning"
    );

    const normalized =
        value.toUpperCase();

    if (
        normalized === "ON" ||
        normalized === "YES" ||
        normalized === "CONNECTED"
    )
    {
        element.classList.add(
            "status-good"
        );
    }
    else if (
        normalized === "OFF" ||
        normalized === "NO" ||
        normalized === "DISCONNECTED"
    )
    {
        element.classList.add(
            "status-off"
        );
    }
    else if (
        normalized === "UNKNOWN"
    )
    {
        element.classList.add(
            "status-warning"
        );
    }
}


async function refreshStatus()
{
    try
    {
        const response =
            await fetch(
                "/api/status",
                {
                    cache: "no-store"
                }
            );

        if (!response.ok)
        {
            console.log(
                "Status request failed:",
                response.status
            );

            return;
        }

        const data =
            await response.json();

        const elements =
            document.querySelectorAll(
                "[data-status-index]"
            );

        data.values.forEach(
            (value, index) =>
            {
                const element =
                    elements[index];

                if (!element)
                {
                    return;
                }

                element.textContent =
                    value;

                updateStatusStyle(
                    element,
                    value
                );
            }
        );
    }
    catch (error)
    {
        console.log(
            "Status update error:",
            error
        );
    }
}


// Update immediately.
refreshStatus();


// Then update once per second.
setInterval(
    refreshStatus,
    1000
);

)rawliteral";