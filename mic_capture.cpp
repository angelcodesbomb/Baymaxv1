#include "mic_capture.h"
#include "config.h"
#include <driver/i2s.h>

static const i2s_port_t MIC_I2S_PORT = I2S_NUM_0;

void micInit() {
  i2s_driver_uninstall(MIC_I2S_PORT); // defensive, in case anything else claimed it

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = MIC_SCK_PIN,
    .ws_io_num = MIC_WS_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD_PIN
  };

  i2s_driver_install(MIC_I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(MIC_I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(MIC_I2S_PORT);
}

static void writeWavHeader(uint8_t* buf, uint32_t dataLen, uint32_t sampleRate) {
  uint32_t byteRate = sampleRate * 2;
  uint32_t chunkSize = 36 + dataLen;

  memcpy(buf, "RIFF", 4);
  memcpy(buf + 4, &chunkSize, 4);
  memcpy(buf + 8, "WAVE", 4);
  memcpy(buf + 12, "fmt ", 4);
  uint32_t subchunk1Size = 16;
  memcpy(buf + 16, &subchunk1Size, 4);
  uint16_t audioFormat = 1;
  memcpy(buf + 20, &audioFormat, 2);
  uint16_t numChannels = 1;
  memcpy(buf + 22, &numChannels, 2);
  memcpy(buf + 24, &sampleRate, 4);
  memcpy(buf + 28, &byteRate, 4);
  uint16_t blockAlign = 2;
  memcpy(buf + 32, &blockAlign, 2);
  uint16_t bitsPerSample = 16;
  memcpy(buf + 34, &bitsPerSample, 2);
  memcpy(buf + 36, "data", 4);
  memcpy(buf + 40, &dataLen, 4);
}

uint8_t* micRecordToWav(size_t* outLen) {
  const uint32_t numSamples = SAMPLE_RATE * RECORD_SECONDS;
  const size_t pcmBytes = numSamples * 2;
  const size_t totalBytes = 44 + pcmBytes;

  uint8_t* wavBuf = (uint8_t*)malloc(totalBytes);
  if (!wavBuf) {
    Serial.println("micRecordToWav: allocation failed");
    *outLen = 0;
    return nullptr;
  }

  writeWavHeader(wavBuf, pcmBytes, SAMPLE_RATE);
  int16_t* pcmOut = (int16_t*)(wavBuf + 44);

  const size_t readChunkSamples = 512;
  int32_t rawBuf[readChunkSamples];
  size_t samplesWritten = 0;

  while (samplesWritten < numSamples) {
    size_t bytesRead = 0;
    i2s_read(MIC_I2S_PORT, rawBuf, readChunkSamples * sizeof(int32_t), &bytesRead, portMAX_DELAY);
    size_t samplesRead = bytesRead / sizeof(int32_t);

    for (size_t i = 0; i < samplesRead && samplesWritten < numSamples; i++) {
      int32_t sample = rawBuf[i] >> MIC_SHIFT;
      if (sample > 32767) sample = 32767;
      if (sample < -32768) sample = -32768;
      pcmOut[samplesWritten++] = (int16_t)sample;
    }
  }

  *outLen = totalBytes;
  return wavBuf;
}