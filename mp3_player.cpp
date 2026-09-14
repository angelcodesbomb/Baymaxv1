#include "mp3_player.h"
#include "display_helper.h"
#include "rgb_led.h"
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <driver/i2s.h>

static AudioGeneratorMP3* mp3 = nullptr;

void mp3PlayerInit() {
  mp3 = new AudioGeneratorMP3();
  Serial.println("mp3PlayerInit: ready");
}

void mp3Play(const uint8_t* data, uint32_t len, bool discoFlash) {
  if (!mp3) {
    Serial.println("mp3Play: called before mp3PlayerInit()!");
    return;
  }
  if (len == 0) {
    Serial.println("mp3Play: empty buffer, skipping");
    return;
  }

  i2s_driver_uninstall(I2S_NUM_0);

  AudioOutputI2S* out = new AudioOutputI2S(0, AudioOutputI2S::INTERNAL_DAC);
  out->SetGain(0.6); // direct-to-speaker, no amp - tune this if too quiet/distorted

  AudioFileSourcePROGMEM* file = new AudioFileSourcePROGMEM(data, len);
  if (!mp3->begin(file, out)) {
    Serial.println("mp3Play: failed to start");
    delete file;
    delete out;
    return;
  }

  uint8_t colors[][3] = {
    {255,0,0},{0,255,0},{0,0,255},{255,255,0},{255,0,255},{0,255,255}
  };
  int colorIdx = 0;
  unsigned long lastFlash = 0;

  while (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop();
    displayUpdateEyes();

    if (discoFlash && millis() - lastFlash > 150) {
      lastFlash = millis();
      rgbSetColor(colors[colorIdx][0], colors[colorIdx][1], colors[colorIdx][2]);
      colorIdx = (colorIdx + 1) % 6;
    }
  }

  if (discoFlash) rgbOff();
  delete file;
  delete out;
}