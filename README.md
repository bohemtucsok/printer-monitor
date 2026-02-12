# Printer Monitor (OctoPrint / Repetier 3D Printer Monitor)

**Fork of [Qrome/printer-monitor](https://github.com/Qrome/printer-monitor)** by David Payne.
The original project's last release was V2.5 (February 2019), and the last commit was in April 2021. Since the project appears to be unmaintained, this fork continues development with new features and improvements.

## What's New in This Fork

### Open-Meteo Weather API (replaces OpenWeatherMap)
The original project required an OpenWeatherMap API key for weather display. This fork replaces it with [Open-Meteo](https://open-meteo.com/) -- a free, open-source weather API that requires **no API key**. Just enter your city name and GPS coordinates in the web interface.

### Automatic Timezone with DST Support
The manual UTC offset input has been replaced with a **timezone region dropdown** (30+ regions). The ESP8266 uses NTP time synchronization with POSIX timezone rules, so **daylight saving time is handled automatically** -- no more manual clock adjustments twice a year.

### Multi-Language Weather Display
Weather conditions on the OLED display and web interface are now available in **5 languages**:
- English, Hungarian, German, French, Spanish
- Selectable from the web interface weather settings

### Display Improvements
- Fixed frame transition stuttering caused by blocking network calls during OLED animations
- Network operations are now deferred until frame transitions complete
- Configurable frame display time (default: 15 seconds per screen)

### Technical Improvements
- Uses `ESP8266HTTPClient` for reliable HTTP communication (handles chunked transfer encoding)
- NTP-based time synchronization instead of HTTP header parsing
- Increased JSON parse buffer for reliable weather data handling

## Features
* Displays the print status from OctoPrint or Repetier Server
* Option to display time and weather when printer is idle
* Estimated time remaining
* Time Printing
* Percentage complete
* Progress bar
* Bed and Tool Temperature
* Screen turns off when printer is turned off or disconnected
* Screen turns on when printer is Operational or connected
* Option to display a clock screen instead of sleep mode
* Option to display 24 hour clock or AM/PM style
* Option to display Current Weather when printer is off (no API key needed)
* Sample rate is every 60 seconds when not printing
* Sample rate is every 10 seconds when printing
* Fully configurable from the web interface (not required to update Settings.h)
* Supports OTA (loading firmware over WiFi connection on same LAN)
* Basic Authentication to protect your settings
* Update firmware through web interface from a compiled binary
* Can query the Octoprint [PSU Control plugin](https://plugins.octoprint.org/plugins/psucontrol/) to enter clock or blank mode when PSU is off
* Repetier support -- define in Settings.h

## Required Parts
* Wemos D1 Mini: https://amzn.to/2ImqD1n
* 0.96" OLED I2C 128x64 Display (12864) SSD1306: https://amzn.to/3cyJekU
* (optional) 1.3" I2C OLED Display: https://amzn.to/2IP0gRU (must uncomment `#define DISPLAY_SH1106` in Settings.h)

## Wiring for the Wemos D1 Mini to the I2C SSD1306 OLED
SDA -> D2
SCL -> D5 / D1 (D1 for Easy Monitor Board)
VCC -> 5V+
GND -> GND-

![Printer Monitor Wire Diagram](/images/printer_monitor_wiring.jpg)

## 3D Printed Case by Qrome
https://www.thingiverse.com/thing:2884823 -- for the 0.96" OLED Display
https://www.thingiverse.com/thing:2934049 -- for the 1.3" OLED Display

## Firmware Update (Pre-compiled Binaries)
You can upload pre-compiled firmware from the web interface: Main Menu -> "Firmware Update".

| Binary | Hardware | Display | Server |
|--------|----------|---------|--------|
| `printermonitor.ino.d1_mini_SSD1306.bin` | Wemos D1 Mini | 0.96" SSD1306 | OctoPrint |
| `printermonitor.ino.d1_mini_SH1106.bin` | Wemos D1 Mini | 1.3" SH1106 | OctoPrint |
| `printermonitor.ino.d1_mini_repetier_SSD1306.bin` | Wemos D1 Mini | 0.96" SSD1306 | Repetier |
| `printermonitor.ino.d1_mini_repetier_SH1106.bin` | Wemos D1 Mini | 1.3" SH1106 | Repetier |
| `printermonitor.ino.d1_mini_easyboard.bin` | Easy Monitor Board | 0.96" SSD1306 | OctoPrint |
| `printermonitor.ino.d1_mini_easyboard_repetier.bin` | Easy Monitor Board | 0.96" SSD1306 | Repetier |

## Compiling and Loading to Wemos D1 Mini
It is recommended to use Arduino IDE. You will need to configure Arduino IDE to work with the Wemos board and USB port and install the required USB drivers.
* USB CH340G drivers: https://sparks.gogo.co.nz/ch340.html
* Enter `http://arduino.esp8266.com/stable/package_esp8266com_index.json` into Additional Board Manager URLs field.
* Open Boards Manager from Tools > Board menu and install esp8266 Core platform version 2.5.2
* Select Board: "LOLIN(WEMOS) D1 R2 & mini"
* Set 1M SPIFFS -- this project uses SPIFFS for saving and reading configuration settings.

## Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to install and manage libraries: https://www.arduino.cc/en/Guide/Libraries

**Packages** -- the following packages and libraries are used (download and install):
* `ESP8266WiFi.h`
* `ESP8266WebServer.h`
* `ESP8266HTTPClient.h`
* `WiFiManager.h` --> https://github.com/tzapu/WiFiManager
* `ESP8266mDNS.h`
* `ArduinoOTA.h` --> Arduino OTA Library
* `SSD1306Wire.h` --> https://github.com/ThingPulse/esp8266-oled-ssd1306/releases/tag/4.1.0 (version 4.1.0)
* `OLEDDisplayUi.h`

Note: ArduinoJson (version 5.13.1) is included in the project.

## Initial Configuration
All settings may be managed from the Web Interface, however, you may update the **Settings.h** file manually -- but it is not required.
* If you are using the Easy Monitor Board you must set `const int SCL_PIN = D1` in Settings.h.
* By default OctoPrint client is selected. If you wish to use Repetier then uncomment `//#define USE_REPETIER_CLIENT` in Settings.h.
* Your OctoPrint API Key from OctoPrint -> User Settings -> Current API Key (similar for Repetier API Key).
* Weather is powered by Open-Meteo (free, no API key needed). Just enter your city name and GPS coordinates.

NOTE: The settings in Settings.h are the default settings for the first loading. After loading you will manage changes via the Web Interface. If you want to change settings again in Settings.h, you will need to erase the file system on the Wemos or use the "Reset Settings" option in the Web Interface.

## Web Interface
The Printer Monitor uses **WiFiManager** so when it can't find the last network it was connected to, it will become an **AP Hotspot** -- connect to it with your phone and you can then enter your WiFi connection information.

After connecting to your WiFi network it will display the IP address assigned to it, which can be used to open a browser to the Web Interface. **Everything** can be configured there.

<p align="center">
  <img src="/images/shot_01.png" width="200"/>
  <img src="/images/shot_02.png" width="200"/>
  <img src="/images/shot_03.png" width="200"/>
  <img src="/images/shot_04.png" width="200"/>
</p>

## Original Project
This is a fork of [Qrome/printer-monitor](https://github.com/Qrome/printer-monitor) created by David Payne.
The original project is licensed under the MIT License.

### Original Contributors
* David Payne -- Principal developer and architect
* Daniel Eichhorn -- Author of the TimeClient class and OLEDDisplayUi
* Florian Schutte -- added flip display to web interface
* Owen Carter -- Added PSU control setting

## License

The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
