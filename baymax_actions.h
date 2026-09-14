#pragma once
#include <Arduino.h>

// Matches transcribed text to a behavior and plays the matching local MP3.
// Returns true if a known command was matched.
bool routeCommand(const String& text);
void playGreeting();