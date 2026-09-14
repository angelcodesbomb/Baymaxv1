#include "tts_player.h"
#include <AudioFileSourceHTTPStream.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <driver/i2s.h>

static AudioOutputI2S* out = nullptr;
static AudioGeneratorMP3* mp3 = nullptr;

void ttsInit() {
  // out(port, mode): port 0, INTERNAL_DAC uses the ESP32's built-in DAC
  // on GPIO25/26 automatically - no pin config needed or possible.
  out = new AudioOutputI2S(0, AudioOutputI2S::INTERNAL_DAC);
  out->SetGain(0.8); // 0.0 - 1.0, turn down if audio clips/distorts
  mp3 = new AudioGeneratorMP3();
}

// Minimal percent-encoding for a TTS query string (spaces, punctuation).
static String urlEncode(const String& s) {
  String out;
  const char* hex = "0123456789ABCDEF";
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (isalnum((unsigned char)c)) {
      out += c;
    } else if (c == ' ') {
      out += "%20";
    } else {
      out += '%';
      out += hex[(c >> 4) & 0xF];
      out += hex[c & 0xF];
    }
  }
  return out;
}

void ttsSpeak(const String& text) {
  if (!out || !mp3) return;

  // Google's endpoint has an undocumented length limit - keep sentences short
  // (the joke/weather lines this project generates are already well under it).
  String query = urlEncode(text);
  String url = "http://translate.google.com/translate_tts?ie=UTF-8&client=tw-ob&tl=en&q=" + query;

  // Defensive: make sure the mic driver isn't holding I2S_NUM_0 right now.
  i2s_driver_uninstall(I2S_NUM_0);

  AudioFileSourceHTTPStream* file = new AudioFileSourceHTTPStream(url.c_str());

  Serial.println("Speaking: " + text);
  if (!mp3->begin(file, out)) {
    Serial.println("ttsSpeak: failed to start playback (check WiFi/URL)");
    delete file;
    return;
  }

  unsigned long start = millis();
  while (mp3->isRunning() && millis() - start < 15000) {
    if (!mp3->loop()) mp3->stop();
  }

  delete file;
}
