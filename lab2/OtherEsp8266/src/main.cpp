#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D7,D6); // (RX, TX
 // RX, TX, inverse_logic, buffer_size
 void setup() {
  // Initialize the hardware Serial for debugging (via USB)
  Serial.begin(9600);
  // Initialize SoftwareSerial at 11520 baud (ensure this matches the transmitting device)
 mySerial.begin(115200, SWSERIAL_6E2, D7,D6);
  
  Serial.println("SoftwareSerial on Wemos D1 Mini started at 11520 baud.");
}

void loop() {
  // Check if data is available on the SoftwareSerial port
  if (mySerial.available() > 0) {
    int incomingByte = mySerial.read();
    Serial.print("Received: ");
    Serial.println(incomingByte);
  }
  mySerial.write(32);
  delay(2000);
}