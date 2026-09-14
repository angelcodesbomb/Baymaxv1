#include "display_helper.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <math.h>

static Adafruit_SH1106G display(128, 64, &Wire, -1);

static EyeState eyeState = EYE_IDLE;
static bool blinking = false;
static unsigned long blinkStart = 0;
static unsigned long nextBlinkAt = 0;

void displayInit() {
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  display.begin(OLED_ADDR, true);
  display.clearDisplay();
  display.display();
}

void displayShowText(const String& line1, const String& line2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  if (line2.length() > 0) {
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println(line2);
  }
  display.display();
}

void displaySetEyeState(EyeState state) {
  eyeState = state;
  blinking = false;
  nextBlinkAt = 0; // forces a fresh blink schedule on next update
}

static void drawArcEye(int cx, int cy) {
  int w = 36;
  for (int x = -w / 2; x <= w / 2; x++) {
    int y = cy - (int)(10 * cos((float)x / (w / 2.0) * (PI / 2)));
    display.fillCircle(cx + x, y, 3, SH110X_WHITE);
  }
}

static void drawExcitedEyes() {
  display.clearDisplay();
  int bounce = (int)(4 * sin(millis() / 150.0));
  drawArcEye(34, 32 + bounce);
  drawArcEye(94, 32 + bounce);
  display.display();
}

static void drawBlinkingEyes(unsigned long blinkIntervalMs) {
  unsigned long now = millis();

  if (!blinking && now >= nextBlinkAt) {
    blinking = true;
    blinkStart = now;
    nextBlinkAt = now + blinkIntervalMs;
  }

  int eyeHeight = 32;
  if (blinking) {
    unsigned long t = now - blinkStart;
    if (t < 80) {
      eyeHeight = map(t, 0, 80, 32, 2);
    } else if (t < 160) {
      eyeHeight = map(t, 80, 160, 2, 32);
    } else {
      blinking = false;
      eyeHeight = 32;
    }
  }

  display.clearDisplay();
  int yOffset = (32 - eyeHeight) / 2;
  display.fillRoundRect(14, 16 + yOffset, 40, eyeHeight, 8, SH110X_WHITE);
  display.fillRoundRect(74, 16 + yOffset, 40, eyeHeight, 8, SH110X_WHITE);
  display.display();
}

void displayUpdateEyes() {
  if (eyeState == EYE_EXCITED) {
    drawExcitedEyes();
  } else if (eyeState == EYE_TALKING) {
    drawBlinkingEyes(700);   // faster blink while "talking"
  } else {
    drawBlinkingEyes(random(2500, 5000)); // slow idle blink
  }
}
