# BaymaxBot — ESP32 voice companion (no extra parts version)

Push-to-talk voice assistant: press a button, speak, ESP32 transcribes your
speech with Groq Whisper, matches it to one of four behaviors, and responds
with sound, an OLED message, and (for disco) RGB lights.

This version uses **only your original parts list** — ESP32's built-in DAC
drives the PAM8403 directly, no extra DAC chip needed.

## Parts used

- ESP32 dev board (an ESP32 with PSRAM helps for the recording buffer, but
  it'll work on a plain WROOM-32 too - just keep RECORD_SECONDS modest)
- PAM8403 class-D amp + 8Ω speaker
- INMP441 I2S MEMS microphone
- DHT11 temperature/humidity sensor
- RGB LED module (common cathode or common anode, common pin to GND/VCC)
- 1.3" OLED, SH1106 driver, I2C (4-pin: VCC/GND/SCL/SDA)
- Push button + breadboard + 5V/2A power supply

## Wiring

| Signal | ESP32 Pin | Notes |
|---|---|---|
| **INMP441 mic** | | |
| WS (LRCLK) | GPIO32 | |
| SCK (BCLK) | GPIO33 | |
| SD (DOUT) | GPIO34 | input-only pin, fine — mic only sends data |
| L/R | GND | selects left channel |
| VDD | 3.3V | |
| GND | GND | |
| **Audio out -> PAM8403** (built-in DAC, fixed pins) | | |
| GPIO25 (DAC channel 1) | → PAM8403 IN L | |
| GPIO26 (DAC channel 2) | → PAM8403 IN R | |
| PAM8403 VCC | separate 5V supply if possible | don't power off ESP32 5V pin if avoidable |
| PAM8403 OUT+/OUT- | Speaker | |
| **DHT11** | | |
| DATA | GPIO4 | 10kΩ pull-up to 3.3V if not on the module |
| **RGB LED** | | |
| R | GPIO16 | via ~220-330Ω resistor if bare LED |
| G | GPIO17 | |
| B | GPIO18 | |
| **OLED (I2C)** | | |
| SDA | GPIO21 | |
| SCL | GPIO22 | |
| **Push button** | GPIO13 | other leg to GND, uses internal pull-up |

All grounds common. GPIO25/26 are the *only* pins the ESP32's internal DAC
can use - they're fixed, not configurable.

**Sound quality note:** the built-in DAC is 8-bit and has audible
hiss/noise compared to an external DAC chip. It's genuinely usable for
speech, just don't expect hi-fi. If you ever pick up a $2 PCM5102A
breakout later, that's the easy upgrade path.

## Libraries to install (Arduino Library Manager)

1. **ESP8266Audio** by Earle Philhower — MP3 decode + internal-DAC output
   (works fine on ESP32 despite the name)
2. **ArduinoJson** (v7) — parsing Groq API responses
3. **Adafruit_SH110X** + **Adafruit_GFX** — OLED (SH1106 driver, common for 1.3" boards)
4. **DHT sensor library** by Adafruit + **Adafruit Unified Sensor**
5. Board package: **esp32 by Espressif Systems** (Boards Manager)

Board settings: enable **PSRAM** in Tools menu if your board has it.
Partition scheme: "Huge APP" if you're low on flash.

## Setup

1. Open `config.h`, fill in your WiFi SSID/password and your Groq API key
   (free at console.groq.com — no credit card needed).
2. Upload `BaymaxBot.ino` (multi-tab sketch — keep all files in one folder
   named `BaymaxBot`, matching the .ino filename).
3. Open Serial Monitor at 115200 baud to watch what's happening.
4. Hold the button, say a command, release. Try: *"how's the weather"*,
   *"start disco"*, *"hi five"*, *"I'm sad"*.

## How it decides what to do

Every recording is transcribed by Whisper (that's the only way to know what
you said), then the text is matched **locally** against keywords — no LLM
call needed for weather/disco/hi-five. Only the "sad" command calls Groq's
LLM, to generate an actual joke. Speech output uses Google's free TTS
endpoint (no key needed), decoded as MP3 and played through the ESP32's
built-in DAC.

## Known rough edges to expect on first bring-up

- INMP441 gain: raw mic samples might come out too quiet or clipped —
  there's a `MIC_SHIFT` constant in `config.h` to tune this (try 11-16).
- The mic (I2S_NUM_0, recording) and the internal-DAC output (also
  I2S_NUM_0, playback) share the same hardware peripheral. The code
  defensively uninstalls one before starting the other — if you ever hear
  garbled audio right after a recording, add a short `delay(100)` between
  them as extra insurance.
- Google's TTS endpoint has an undocumented text-length limit and can
  occasionally rate-limit — if `ttsSpeak()` silently fails, check Serial
  Monitor for the HTTP/playback error it prints.
- First WiFi/HTTPS connection can take a few seconds — normal.
