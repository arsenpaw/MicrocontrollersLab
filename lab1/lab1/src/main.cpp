#include <Arduino.h>
#include <ButtonWebServer.h>
#include "AsyncStop.h"

#define BUTTON_PIN 4
#define LED_1_PIN 16
#define LED_2_PIN 17
#define LED_3_PIN 5
#define ButtonDebounceTime 250
#define LedDelayTiem 500
#define ButtonLongPressTime 2000
const uint8_t leds[] = {LED_1_PIN, LED_2_PIN, LED_3_PIN}; 
const uint8_t numLeds = sizeof(leds) / sizeof(leds[0]);
bool buttonState;            
bool lastButtonState = HIGH;  
bool isButtonPressed = 0;
bool isButtonEnoughPressed = 0;
bool buttonToggle = 0;
uint8_t currentLed = 0;
uint32_t lastDebounceTime = 0;
uint32_t previousMillis = 0; 
uint32_t buttonPressStartTime = 0;
ButtonWebServer buttonWebServer("ARSEN_ESP32", "123456789");
void setup()
{
  Serial.begin(115200);
  buttonWebServer.init();
  pinMode(BUTTON_PIN, INPUT); 
  for (int i = 0; i < numLeds; i++) 
  {
    pinMode(leds[i], OUTPUT);
  }
}

void blinkLED()
{
  uint32_t currentMillis = millis();
  if (currentMillis - previousMillis >= LedDelayTiem) 
   {
    digitalWrite(leds[currentLed], LOW);
    currentLed = (currentLed + 1) % numLeds;
    digitalWrite(leds[currentLed], HIGH);
    previousMillis = currentMillis;
  }
}

int readDebouncedButton() {
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) 
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > ButtonDebounceTime)
  {
    if (reading != buttonState)
     {
      buttonState = reading;
    }
  }
  lastButtonState = reading;
  return reading;
}

bool onRelease()
{
  uint32_t currentMillis = millis();
  int buttonState  = readDebouncedButton();
  if (buttonState == LOW)
   {
    if (!isButtonPressed)
     {
      buttonPressStartTime = currentMillis;
      isButtonPressed = true;
      Serial.println("Button pressed");
    }
    else if (currentMillis - buttonPressStartTime > ButtonLongPressTime) 
    {
      Serial.println("Button too long pressed");
      isButtonEnoughPressed = true;
    }
    return false;
  }
  else
  {
    isButtonPressed = false;
  }

  if (isButtonEnoughPressed) 
  {
    isButtonPressed = false;
    isButtonEnoughPressed = false;
    return true;
  }
  
  return false;
}


void blinkLedWithAsyncStop()
{
  if (onRelease()) 
  {
    AsyncStop::getInstance().request();
  }

  if (AsyncStop::getInstance().isActive()) 
  {
    return; 
  }

  blinkLED();

}

void loop() {

  buttonWebServer.handleClient();
  blinkLedWithAsyncStop();
}

