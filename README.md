# Elegoo CC1 Monitor

A 3D print monitor for Elegoo Centauri Carbon printer(CC1), running on an ESP32-C3 with an OLED display and a web-based configuration panel.

## Features

- Reads printer status via the SDCP protocol (WebSocket, port 3030): state, progress, elapsed/remaining time, nozzle/bed/chamber temperatures, layers.
- OLED display 128×64 (I2C) with animated screens: printing, paused, complete, idle, WiFi setup (AP mode), connecting.
- Web panel (served directly from the ESP32) for:
  - WiFi configuration (network scan or manual SSID entry),
  - setting the printer's IP address, poll interval, and OLED rotation,
  - viewing the current print status,
  - factory reset.
- Automatic fallback to AP mode (`ElegooMonitor` / `elegoo123`, `192.168.4.1`) when no network is saved or the connection fails.
- mDNS: the panel is reachable at `http://elegoo-monitor.local`.
- Configuration is stored in flash (NVS/`Preferences`) — survives restarts and power loss.

## Hardware

- ESP32-C3
- SSD1306 OLED display, 128×64, I2C, address `0x3C`
  - SDA → GPIO10
  - SCL → GPIO9

## Building and flashing

This project uses [PlatformIO](https://platformio.org/).

```sh
pio run              # build
pio run -t upload    # flash the ESP32-C3
pio device monitor   # serial log (115200 baud)
```

## First run

1. After flashing, the ESP32-C3 boots into access-point mode: network `ElegooMonitor`, password `elegoo123`.
2. Connect to that network and open `http://192.168.4.1`.
3. Pick your WiFi network (or enter the SSID manually), enter the password, and save — the ESP32 will restart and connect to your home network.
4. On the "Settings" tab, enter the printer's IP address (shown on its display or in your router's admin page) and save.
5. The panel is now available at the ESP32's IP address (shown in the Serial Monitor) or at `http://elegoo-monitor.local`.

## Configuration

All settings (SSID, password, printer IP, poll interval, OLED rotation) are stored in NVS (`Preferences`, namespace `elegoo_mon`).

To restore factory defaults: use the "Factory Reset" button in the web panel (Settings tab), or call `GET /api/reset`.

## API (used by the web panel)

| Endpoint         | Method | Description                                  |
|------------------|--------|-----------------------------------------------|
| `/api/scan`      | GET    | scans for available WiFi networks             |
| `/api/config`    | GET    | returns the current configuration and WiFi status |
| `/api/printer`   | GET    | returns the current printer status             |
| `/api/save`      | POST   | saves configuration (WiFi and/or printer)      |
| `/api/reset`     | GET    | clears flash and restarts the ESP32            |

## Project structure

- `src/main.cpp` — all the logic: WiFi/AP, web server with the embedded HTML/JS panel, SDCP protocol client, OLED screen rendering.
- `platformio.ini` — PlatformIO configuration (`board = esp32-c3-devkitm-1`, `framework = arduino`).

## Dependencies

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [ArduinoJson](https://arduinojson.org/) ^7
