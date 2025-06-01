#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include "BluetoothA2DPSource.h"
#include <ringbuffer.hpp>

typedef struct { uint8_t sample; } AudioPacket;
BluetoothA2DPSource a2dp_source;
const char* speakerMac = "4f:05:65:f5:e5:33";
jnk0le::Ringbuffer<int8_t, 8192> soundBuffer;

int32_t get_sound_data(uint8_t* data, int32_t byteCount) {
  int32_t bytesFilled = 0;

  // Кожні 4 байти → 1 стереосемпл
  while (bytesFilled + 3 < byteCount) {
    if (soundBuffer.isEmpty()) {
      Serial.println("Buffer empty, filling silence...");
      memset(data + bytesFilled, 0, byteCount - bytesFilled);
      bytesFilled = byteCount;
      break;
    }
    int8_t raw8;
    soundBuffer.remove(raw8);
    Serial.print("Got raw sample: ");
    Serial.println(raw8);

    int16_t pcm16 = raw8 << 8; 
    uint8_t lo = uint8_t(pcm16 & 0xFF);
    uint8_t hi = uint8_t((pcm16 >> 8) & 0xFF);

    data[bytesFilled++] = lo;
    data[bytesFilled++] = hi;
    data[bytesFilled++] = lo;
    data[bytesFilled++] = hi;
  }

  // у випадку, якщо byteCount % 4 != 0
  while (bytesFilled < byteCount) {
    data[bytesFilled++] = 0;
  }

  return byteCount;
}


void onReceive(const uint8_t* mac_addr, const uint8_t* data, int data_len) {
  Serial.printf("Got ESP-NOW packet, len=%d. First byte: %02X\n", data_len, data_len > 0 ? data[0] : 0);
  for (int i = 0; i < data_len; i++) {
    soundBuffer.insert(static_cast<int8_t>(data[i]));
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
  uint8_t peer[6];
sscanf(speakerMac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
       &peer[0], &peer[1], &peer[2],
       &peer[3], &peer[4], &peer[5]);
  Serial.printf("Connecting to %s\n", speakerMac);
  a2dp_source.set_auto_reconnect(false);
  a2dp_source.set_data_callback(get_sound_data);
  a2dp_source.start("CHARGE3");
    bool ok = a2dp_source.connect_to(peer);
    if (ok) {
    Serial.printf("Connected");
  } else {
    Serial.println("[ERROR] a2dp_source.connect() called successfully");
  }

}

void loop() { 
}
