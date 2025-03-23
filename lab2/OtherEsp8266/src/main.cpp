#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include "CommunicationService.h"
#define BUTTON_DEBOUNCE_DELAY 200UL
#define LED_BLINK_INTERVAL 500UL
#define BUTTON_PIN D2 
#define LED1_PIN D1    
#define LED2_PIN D0    
#define LED3_PIN D5     

uint8_t ledOrder[] = {LED1_PIN, LED2_PIN, LED3_PIN}; 
int currentLed = 0;   
bool buttonPressed = false;

uint32_t lastButtonPress = 0;
bool sequenceMode = true; 
SoftwareSerial mySerial(D7, D6, false); //rx tx
CommunicationService communicationService(mySerial, 115200);
const char* ssid = "MyESP";
ESP8266WebServer server(80);


void processButtonAction() {
    if (sequenceMode) {
        sequenceMode = false;
        Serial.println("Manual mode");
    } else {
        int nextLed = (currentLed + 1) % 3; // todo make var for leds 
        if (digitalRead(ledOrder[currentLed]) == LOW) {
            digitalWrite(ledOrder[currentLed], HIGH);
            digitalWrite(ledOrder[nextLed], HIGH);
            Serial.print("OFF LED: ");
            Serial.println(nextLed);
        } else {
            digitalWrite(ledOrder[nextLed], LOW);
            Serial.print("ON LED: ");
            Serial.println(nextLed);
            currentLed = nextLed;
        }
    }
}

void setup() {
    Serial.begin(115200);
    communicationService.init();
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    for (int i = 0; i < 3; i++) {
        digitalWrite(ledOrder[i], LOW);
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    Serial.println("WiFi AP Started");

    server.on("/", []() {
      server.send(200, "text/html",
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>LED Control</title>"
        "<style>"
        "body { font-family: Arial; text-align: center; margin-top: 50px; }"
        "button { font-size: 20px; padding: 10px 20px; }"
        "</style>"
        "</head>"
        "<body>"
        "<h1>LED Control via Web</h1>"
        "<button onclick=\"simulateButton()\">Press Button</button>"
         "<button onclick=\"simulateButtonRemote()\">Press Remote Button</button>"
        "<script>"
        "function simulateButton() {"
        "  fetch('/simulate').then(response => response.text()).then(data => {"
        "    console.log(data);"
        "  });"
        "}"
        "function simulateButtonRemote() {"
        "  fetch('/simulateRemote').then(response => response.text()).then(data => {"
        "    console.log(data);"
        "  });"
        "}"
        "</script>"
        "</body>"
        "</html>" //todo make in func
      );
    });

    server.on("/simulate", []() {
      processButtonAction();
      server.send(200, "text/plain", "Button press simulated.");
    });
    server.on("/simulateRemote", []() {
      communicationService.send(ToogleCommand::ON);
      server.send(200, "text/plain", "Button press simulated.");
    });

    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
    communicationService.onReceive([](ToogleCommand command) {
        if (command == ToogleCommand::ON) {
            buttonPressed = true;
            processButtonAction();
    }});
    static uint32_t lastBlinkTime = 0;
    unsigned long now = millis(); //тип всюди одинаковий 

    int buttonState = digitalRead(BUTTON_PIN);
//function for button
    if (buttonState == HIGH && !buttonPressed && (now - lastButtonPress > BUTTON_DEBOUNCE_DELAY)) {
        buttonPressed = true;
        lastButtonPress = now;
        processButtonAction();
    }

    if (buttonState == LOW) {
        buttonPressed = false;
    }
//function for Led 
    if (sequenceMode && (now - lastBlinkTime > LED_BLINK_INTERVAL)) {
        lastBlinkTime = now;
        digitalWrite(ledOrder[currentLed], LOW);
        currentLed = (currentLed + 1) % 3;
        digitalWrite(ledOrder[currentLed], HIGH);
    }
}
