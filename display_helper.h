#pragma once
#include <Arduino.h>

enum EyeState { EYE_IDLE, EYE_TALKING, EYE_EXCITED };

void displayInit();
void displayShowText(const String& line1, const String& line2 = "");

// Call once whenever Baymax's mood/activity changes.
void displaySetEyeState(EyeState state);

// Call every loop() tick (and inside any blocking playback loop) to
// advance blinking (idle/talking) or the happy-arc bounce (excited).
void displayUpdateEyes();