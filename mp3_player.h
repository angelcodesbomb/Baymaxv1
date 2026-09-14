#pragma once
#include <Arduino.h>

void mp3PlayerInit();
void mp3Play(const uint8_t* data, uint32_t len, bool discoFlash = false);