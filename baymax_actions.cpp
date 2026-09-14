#include "baymax_actions.h"
#include "config.h"
#include "display_helper.h"
#include "mp3_player.h"
#include <DHT.h>

#include "audio_greeting.h"
#include "audio_hot.h"
#include "audio_pleasant.h"
#include "audio_disco.h"
#include "audio_fistbump.h"

static DHT dht(DHT_PIN, DHT_TYPE);
static bool dhtStarted = false;

static String lower(const String& s) {
  String out = s;
  out.toLowerCase();
  return out;
}

void playGreeting() {
  displaySetEyeState(EYE_TALKING);
  displayShowText("Hi! I'm Baymax", "your assistant");
  mp3Play(audio_greeting_mp3, audio_greeting_mp3_len);
  displaySetEyeState(EYE_IDLE);
}

static void doWeather() {
  if (!dhtStarted) { dht.begin(); dhtStarted = true; delay(250); }

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    displayShowText("Sensor", "error");
    delay(1500);
    displaySetEyeState(EYE_IDLE);
    return;
  }

  char line1[16], line2[24];
  snprintf(line1, sizeof(line1), "%.0fC", temp);
  snprintf(line2, sizeof(line2), "Humidity %.0f%%", hum);
  displayShowText(line1, line2);
  delay(1500);

  displaySetEyeState(EYE_TALKING);
  if (temp >= TEMP_HOT_THRESHOLD_C) {
    displayShowText("Baymax says:", "It's quite hot!");
    mp3Play(audio_hot_mp3, audio_hot_mp3_len);
  } else {
    displayShowText("Baymax says:", "Pleasant today!");
    mp3Play(audio_pleasant_mp3, audio_pleasant_mp3_len);
  }
  displaySetEyeState(EYE_IDLE);
}

static void doDisco() {
  displaySetEyeState(EYE_EXCITED);
  displayShowText("Disco time!", "");
  mp3Play(audio_disco_mp3, audio_disco_mp3_len, true);
  displaySetEyeState(EYE_IDLE);
}

static void doFistBump() {
  displaySetEyeState(EYE_TALKING);
  displayShowText("Fist bump!", "ta-da-da-da");
  mp3Play(audio_fistbump_mp3, audio_fistbump_mp3_len);
  displaySetEyeState(EYE_IDLE);
}

bool routeCommand(const String& text) {
  String t = lower(text);

  if (t.indexOf("weather") >= 0) { doWeather(); return true; }
  if (t.indexOf("disco") >= 0)   { doDisco(); return true; }
  if (t.indexOf("five") >= 0 || t.indexOf("hi 5") >= 0) { doFistBump(); return true; }

  return false;
}