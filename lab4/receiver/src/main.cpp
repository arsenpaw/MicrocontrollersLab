#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <ringbuffer.hpp>
typedef struct { uint8_t sample; } AudioPacket;
const char* speakerMac = "4f:05:65:f5:e5:33";
const int TIP = 25;
const int RING1 = 26;
void onReceive(const uint8_t* mac_addr, const uint8_t* data, int len) {
  Serial.printf("Got ESP-NOW packet, len=%d. First byte: %02X\n",
                len, len>0 ? data[0] : 0);

  for (int i = 0; i < len; i++) {
    uint8_t sample = data[i];
    Serial.printf("Sample %d: %02X\n", i, sample);
    dacWrite(RING1, sample);  
    dacWrite(TIP, sample);
    delayMicroseconds(125);
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
    pinMode(TIP, OUTPUT);
  pinMode(RING1, OUTPUT);
  digitalWrite(TIP, LOW);
  digitalWrite(RING1, LOW);

} 

void loop() { 
}
