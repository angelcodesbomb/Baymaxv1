#include "rgb_led.h"
#include "config.h"

// Core 2.0.17 uses the OLD channel-based LEDC API.
static const int CH_R = 0;
static const int CH_G = 1;
static const int CH_B = 2;
static const int PWM_FREQ = 5000;
static const int PWM_RES  = 8;

void rgbInit() {
  ledcSetup(CH_R, PWM_FREQ, PWM_RES);
  ledcSetup(CH_G, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(RGB_R_PIN, CH_R);
  ledcAttachPin(RGB_G_PIN, CH_G);
  ledcAttachPin(RGB_B_PIN, CH_B);
  rgbOff();
}

void rgbSetColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(CH_R, r);
  ledcWrite(CH_G, g);
  ledcWrite(CH_B, b);
}

void rgbOff() {
  rgbSetColor(0, 0, 0);
}

void rgbDiscoLoop(uint32_t durationMs) {
  uint8_t colors[][3] = {
    {255,0,0},{0,255,0},{0,0,255},{255,255,0},{255,0,255},{0,255,255}
  };
  uint32_t start = millis();
  int i = 0;
  while (millis() - start < durationMs) {
    rgbSetColor(colors[i % 6][0], colors[i % 6][1], colors[i % 6][2]);
    delay(120);
    i++;
  }
  rgbOff();
}
