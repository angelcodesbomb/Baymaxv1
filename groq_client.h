#pragma once
#include <Arduino.h>

// Uploads a WAV buffer to Groq Whisper and returns the transcribed text.
// Returns an empty string on failure.
String groqTranscribe(uint8_t* wavData, size_t wavLen);