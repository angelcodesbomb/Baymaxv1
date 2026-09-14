#include "groq_client.h"
#include "config.h"
#include <WiFiClientSecure.h>

String groqTranscribe(uint8_t* wavData, size_t wavLen) {
  if (!wavData || wavLen == 0) return "";

  Serial.printf("Free heap before upload: %u\n", ESP.getFreeHeap());

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15000);

  if (!client.connect("api.groq.com", 443)) {
    Serial.println("groqTranscribe: connection failed");
    return "";
  }

  String boundary = "BaymaxBoundary7331";
  String head =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"model\"\r\n\r\n" +
    GROQ_STT_MODEL + "\r\n"
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"response_format\"\r\n\r\n"
    "json\r\n"
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n"
    "Content-Type: audio/wav\r\n\r\n";
  String tail = "\r\n--" + boundary + "--\r\n";

  size_t totalLen = head.length() + wavLen + tail.length();

  // Send HTTP request line + headers manually - no HTTPClient, no body duplication.
  client.print("POST /openai/v1/audio/transcriptions HTTP/1.1\r\n");
  client.print("Host: api.groq.com\r\n");
  client.print("Authorization: Bearer " + String(GROQ_API_KEY) + "\r\n");
  client.print("Content-Type: multipart/form-data; boundary=" + boundary + "\r\n");
  client.print("Content-Length: " + String(totalLen) + "\r\n");
  client.print("Connection: close\r\n\r\n");

  // Stream body in three pieces - the WAV data is written straight from the
  // caller's buffer, never copied.
  client.print(head);

  const size_t chunkSize = 1024;
  size_t sent = 0;
  while (sent < wavLen) {
    size_t toSend = min(chunkSize, wavLen - sent);
    client.write(wavData + sent, toSend);
    sent += toSend;
  }

  client.print(tail);

  Serial.printf("Free heap after upload: %u\n", ESP.getFreeHeap());

  // Read response: skip HTTP headers, then grab the body.
  unsigned long start = millis();
  while (client.connected() && !client.available() && millis() - start < 15000) {
    delay(10);
  }

  String line;
  bool headersDone = false;
  String body = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      line = client.readStringUntil('\n');
      if (!headersDone) {
        if (line == "\r") headersDone = true;
      } else {
        body += line + "\n";
      }
    }
    if (!client.connected() && !client.available()) break;
  }
  client.stop();

  // Extract "text":"..." from the JSON body without a full JSON library -
  // keeps this lightweight, fine for Whisper's simple response shape.
  int textIdx = body.indexOf("\"text\":\"");
  if (textIdx < 0) {
    Serial.println("groqTranscribe: unexpected response:");
    Serial.println(body);
    return "";
  }
  int startIdx = textIdx + 8;
  int endIdx = body.indexOf("\"", startIdx);
  // Handle escaped quotes inside the text (rare, but be safe)
  while (endIdx > 0 && body[endIdx - 1] == '\\') {
    endIdx = body.indexOf("\"", endIdx + 1);
  }
  if (endIdx < 0) return "";

  String result = body.substring(startIdx, endIdx);
  result.replace("\\\"", "\"");
  result.trim();
  return result;
}