#include <Arduino.h>
#include "asyncstop.h"
#include <ButtonWebServer.h>
#define BUTTON_PIN 4
#define LED_1_PIN 16
#define LED_2_PIN 17
#define LED_3_PIN 5

const int leds[] = {LED_1_PIN, LED_2_PIN, LED_3_PIN}; 
const int numLeds = sizeof(leds) / sizeof(leds[0]);
int currentLed = 0;
int previousMillis = 0; 
int buttonPressStartTime = 0;
bool isButtonPressed = 0;
bool isButtonEnoughPressed = 0;

ButtonWebServer buttonWebServer("PC", "123456789");
void setup()
{
  Serial.begin(9600);
  buttonWebServer.init();
  pinMode(BUTTON_PIN, INPUT); 
  for (int i = 0; i < numLeds; i++) 
  {
    pinMode(leds[i], OUTPUT);
  }
}

void blinkLED()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 500)
   {
    digitalWrite(leds[currentLed], LOW);
    
    currentLed = (currentLed + 1) % numLeds;
    
    digitalWrite(leds[currentLed], HIGH);

    previousMillis = currentMillis;
  }
}

bool onRelease()
{
  unsigned int currentMillis = millis();
  
  int buttonState = digitalRead(BUTTON_PIN); 
  if (buttonState == LOW && !isButtonPressed) 
  {
     buttonPressStartTime = currentMillis;
     isButtonPressed = true;
     isButtonEnoughPressed = false;
  }

  if (buttonState == LOW && isButtonPressed) 
  {
    if (currentMillis - buttonPressStartTime > 2000) 
    {
       isButtonEnoughPressed = true;
    }
  }
  if (buttonState == HIGH && isButtonEnoughPressed) {
    isButtonPressed = false;
    isButtonEnoughPressed = false;
    return true;
  }
  return false;
}


void loop() {
  buttonWebServer.handleClient();
  if (onRelease()) {
    AsyncStop::getInstance().request();
  }

  if (AsyncStop::getInstance().isActive()) {
    return; 
  }

  blinkLED();
}