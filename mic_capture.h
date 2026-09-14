#pragma once
#include <Arduino.h>

void micInit();

// Records up to RECORD_SECONDS of audio and returns a heap-allocated WAV
// buffer (header + PCM data). Caller must free() it. Returns nullptr on failure.
uint8_t* micRecordToWav(size_t* outLen);