#include <SPI.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR        0x27
#define LCD_COLS        12
#define LCD_ROWS        2
#define TRIG_PIN        A3
#define ECHO_PIN        A2
#define READ_INTERVAL   500  
const int ERROR_LED_PIN = 7; 
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

unsigned long lastReadTime = 0;
float distanceCm = 0;
void measureDistance();
void updateDisplay();
void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
    
  pinMode(DD7, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
}

void loop() {
  unsigned long now = millis();
  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;
    measureDistance();

    updateDisplay();
  }
}

void measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = (duration * 0.0343f) / 2.0f;
  Serial.print("Distance: ");
  Serial.print(distanceCm, 2);
  Serial.println(" cm");
}

void updateDisplay() {
  lcd.setCursor(0, 1);
  
  char buf[10];
  if (distanceCm > 750.0f)
 {
    lcd.print("Irrelevant");
      digitalWrite(ERROR_LED_PIN, HIGH);
    return;
  }
  dtostrf(distanceCm, 5, 2, buf); 
  digitalWrite(ERROR_LED_PIN, LOW);
  lcd.print(buf);
  lcd.print(" cm   ");     

}
