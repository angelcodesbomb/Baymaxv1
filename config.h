#pragma once

// ---------- WiFi ----------
#define WIFI_SSID     "YOUR_WIFI"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ---------- Groq API ----------
// Get a free key at https://console.groq.com  (no credit card needed)
#define GROQ_API_KEY  "Your_API_Key"

#define GROQ_STT_URL   "https://api.groq.com/openai/v1/audio/transcriptions"
#define GROQ_CHAT_URL  "https://api.groq.com/openai/v1/chat/completions"
#define GROQ_STT_MODEL  "whisper-large-v3-turbo"
#define GROQ_CHAT_MODEL "llama-3.1-8b-instant"

// ---------- Mic (INMP441, I2S_NUM_0) ----------
#define MIC_WS_PIN   32   // word select / LRCLK
#define MIC_SCK_PIN  33   // bit clock
#define MIC_SD_PIN   34   // data out from mic (input-only pin, fine)


// ---------- Recording ----------
#define SAMPLE_RATE      16000
#define RECORD_SECONDS   3
#define MIC_SHIFT        18   // tune 11-16 if transcription is empty/garbled

// ---------- Audio out ----------
// Using the ESP32's BUILT-IN DAC (no extra DAC chip needed).
// This is fixed hardware - the internal DAC only exists on GPIO25 (channel 1)
// and GPIO26 (channel 2). Wire these straight into PAM8403 IN L / IN R.
// Sound quality is noticeably lower than an external DAC (some hiss/noise),
// but works with zero extra parts.

// ---------- DHT11 ----------
#define DHT_PIN  4
#define DHT_TYPE DHT11
#define TEMP_HOT_THRESHOLD_C 30.0
// ---------- RGB LED ----------
#define RGB_R_PIN 27
#define RGB_G_PIN 14
#define RGB_B_PIN 19

// ---------- Button ----------
#define BUTTON_PIN 13   // other leg to GND, uses INPUT_PULLUP

// ---------- OLED ----------
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDR    0x3C   // common default for SH1106 1.3" boards

