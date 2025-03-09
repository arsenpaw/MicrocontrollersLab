#include <Arduino.h>
#include "asyncstop.h"
#include <ButtonWebServer.h>
#define BUTTON_PIN 4
#define LED_1_PIN 16
#define LED_2_PIN 17
#define LED_3_PIN 5

const int leds[] = {LED_1_PIN, LED_2_PIN, LED_3_PIN}; 
const int numLeds = sizeof(leds) / sizeof(leds[0]);
int buttonState;            
int lastButtonState = HIGH;  
int currentLed = 0;
int lastDebounceTime = 0;
int previousMillis = 0; 
int buttonPressStartTime = 0;
bool isButtonPressed = 0;
bool isButtonEnoughPressed = 0;
unsigned long buttonLastReadTime = 0;

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
int readDebouncedButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {

    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 250){
    if (reading != buttonState) {
      buttonState = reading;
    }
  }

  lastButtonState = reading;
  return reading;
}
bool onRelease()
{

  unsigned int currentMillis = millis();
  int buttonState  = readDebouncedButton();
  if (buttonState == LOW)
   {
    if (!isButtonPressed)
     {
      buttonPressStartTime = currentMillis;
      isButtonPressed = true;
      Serial.println("Button pressed");
    }
    else if (currentMillis - buttonPressStartTime > 2000) 
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
void loop() {
  buttonWebServer.handleClient();
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