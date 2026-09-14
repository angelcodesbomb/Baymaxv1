#pragma once
#include <Arduino.h>

// Sets up the internal-DAC audio output. Call once in setup().
void ttsInit();

// Downloads and speaks a line of text via Google's free TTS endpoint,
// played out through the ESP32's built-in DAC (GPIO25/26). Blocking -
// returns once playback finishes (or times out).
void ttsSpeak(const String& text);
