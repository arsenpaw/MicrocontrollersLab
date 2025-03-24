#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include "CommunicationService.h"

#define BUTTON_DEBOUNCE_DELAY 200UL
#define LED_BLINK_INTERVAL 500UL
#define BUTTON_PIN 16U  
#define LED1_PIN 15U    
#define LED2_PIN 14U    
#define LED3_PIN 0U     

uint32_t ledOrder[] = {LED1_PIN, LED2_PIN, LED3_PIN}; 
uint32_t currentLed = 0;
bool buttonPressed = false;
uint32_t lastButtonPress = 0;
bool sequenceMode = true; 

SoftwareSerial mySerial(D7, D6, false);
CommunicationService communicationService(mySerial, 115200);
const char* ssid = "MyESP";
ESP8266WebServer server(80);

void processButtonAction() {
    if (sequenceMode) 
    {
        sequenceMode = false;
        Serial.println("Manual mode");
        return;
    }

    uint32_t nextLed = (currentLed + 1) % 3;
    if (digitalRead(ledOrder[currentLed]) == LOW) 
    {
        digitalWrite(ledOrder[currentLed], HIGH);
        digitalWrite(ledOrder[nextLed], HIGH);
        Serial.print("OFF LED: ");
        Serial.println(nextLed);
        return;
    } 
    digitalWrite(ledOrder[nextLed], LOW);
    Serial.print("ON LED: ");
    Serial.println(nextLed);
    currentLed = nextLed;    
}

void proceesRootRequest() {
    String markup =  "<!DOCTYPE html>"
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
        "</html>";
        server.send(200, "text/html", markup);
}

void setup() {
    Serial.begin(112500);
    communicationService.init();
    pinMode(BUTTON_PIN, INPUT); 
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    for (int i = 0; i < 3; i++) 
    {
        digitalWrite(ledOrder[i], LOW);
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    Serial.println("WiFi AP Started");

    server.on("/", HTTP_GET, proceesRootRequest);

    server.on("/simulate", []() 
    {
        processButtonAction();
        server.send(200, "text/plain", "Button press simulated.");
    });

    server.on("/simulateRemote", []() 
    {
        communicationService.send(ToogleCommand::ON);
        server.send(200, "text/plain", "Button press simulated.");
    });

    server.begin();
    Serial.println("Server started");
}

void handleButtonPress() {
    static uint32_t lastPressTime = 0;
    uint32_t now = millis();
    int buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == HIGH && !buttonPressed && (now - lastPressTime > BUTTON_DEBOUNCE_DELAY))
    {
        buttonPressed = true;
        lastPressTime = now;
        processButtonAction();
    }

    if (buttonState == LOW) 
    {
        buttonPressed = false;
    }
}

void handleLedSequence() {
    static uint32_t lastBlinkTime = 0;
    uint32_t now = millis();

    if (sequenceMode && (now - lastBlinkTime > LED_BLINK_INTERVAL)) 
    {
        lastBlinkTime = now;
        digitalWrite(ledOrder[currentLed], LOW);
        currentLed = (currentLed + 1) % (sizeof(ledOrder) / sizeof(ledOrder[0]));//in separet  var
        digitalWrite(ledOrder[currentLed], HIGH);
    }
}

void loop() {
    server.handleClient();
    communicationService.onReceive([](ToogleCommand command) {
        if (command == ToogleCommand::ON) {
            buttonPressed = true;
            processButtonAction();
    }});
    handleButtonPress();
    handleLedSequence();
}