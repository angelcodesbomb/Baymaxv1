#pragma once
#include <Arduino.h>

void rgbInit();
void rgbSetColor(uint8_t r, uint8_t g, uint8_t b);
void rgbOff();

// Blocking disco flash loop for durationMs milliseconds.
// Runs a sequence of bright colors with short pauses.
void rgbDiscoLoop(uint32_t durationMs);
