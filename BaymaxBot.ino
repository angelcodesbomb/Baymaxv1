#include <WiFi.h>
#include "config.h"
#include "mic_capture.h"
#include "groq_client.h"
#include "baymax_actions.h"
#include "display_helper.h"
#include "rgb_led.h"
#include "mp3_player.h"

#define TAP_THRESHOLD_MS 400  // shorter than this = quick tap, longer = hold-to-talk

static void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(400);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.printf("\nWiFi FAILED. Status code: %d\n", WiFi.status());
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  displayInit();
  rgbInit();
  mp3PlayerInit();
  displaySetEyeState(EYE_IDLE);

  connectWiFi();

  Serial.println("Baymax ready. Quick tap = greeting, hold and speak = voice command.");
}

void loop() {
  displayUpdateEyes();

  if (digitalRead(BUTTON_PIN) == LOW) {
    unsigned long pressStart = millis();

    // Wait to see if this is a quick tap or the start of a hold.
    while (digitalRead(BUTTON_PIN) == LOW && millis() - pressStart < TAP_THRESHOLD_MS) {
      delay(10);
    }
    bool stillHeld = (digitalRead(BUTTON_PIN) == LOW);

    if (!stillHeld) {
      // Released quickly -> quick tap -> greeting, no mic/network involved.
      Serial.println("Quick tap -> greeting");
      playGreeting();
    } else {
      // Still held -> voice command flow.
      displayShowText("Listening...", "");
      Serial.println("Held -> recording...");

      micInit();
      size_t wavLen = 0;
      uint8_t* wav = micRecordToWav(&wavLen);

      if (wav) {
        displayShowText("Thinking...", "");
        String text = groqTranscribe(wav, wavLen);
        free(wav);

        Serial.println("Heard: \"" + text + "\"");

        if (text.length() == 0) {
          displayShowText("Didn't catch", "that");
          delay(1200);
        } else if (!routeCommand(text)) {
          displayShowText("Not sure what", "you meant");
          delay(1500);
        }
      } else {
        displayShowText("Mic error", "");
        delay(1000);
      }

      displaySetEyeState(EYE_IDLE);
      while (digitalRead(BUTTON_PIN) == LOW) delay(20);
    }
  }
}