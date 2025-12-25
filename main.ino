// button relay
/*
negro/negro -> scan
rojo/rojo -> close
amarillio/naranja -> new
*/

#include <SD.h>
#include <SPI.h>
#include <Keypad.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_LiquidCrystal.h>


int seconds = 0;
bool sdOk = false;
String LCD_MESSAGE = "";


// Temperature sensor settings
const float ADC_VREF = 5.0;
const float ADC_RESOLUTION = 1024.0;
const byte PIN_TMP36 = A0;

// Buzzer and button relay settings
const int BTN_SCAN = 2;
const int BTN_CLOSE = 3;
const int RELAY1 = 7;
const int RELAY2 = 8;
const int BUZZER = 13;
const int LED_PIN = 52;

// SD card settings
const int SD_CS = 53;


// Keypad settings
const byte ROWS = 4;
const byte COLS = 4;
char KEYS[ROWS][COLS] = {
 {'1', '2', '3', 'A'},
 {'4', '5', '6', 'B'},
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'}
};
const byte ROW_PINS[ROWS] = {10, 9, 8, 7};
const byte COL_PINS[COLS] = {6, 5, 4, 3};
Keypad KEYPAD = Keypad(makeKeymap(KEYS), ROW_PINS, COL_PINS, ROWS, COLS);


// Display settings
Adafruit_7segment DISPLAY_TEMPERATURE = Adafruit_7segment();
Adafruit_7segment DISPLAY_HUMIDITY = Adafruit_7segment();
Adafruit_LiquidCrystal DISPLAY_SCREEN(0);



void setup()
{
   Serial.begin(115200);
   DISPLAY_SCREEN.begin(16, 2);
   DISPLAY_TEMPERATURE.begin(112);
   DISPLAY_TEMPERATURE.begin(113);

   pinMode(RELAY1, OUTPUT);
   pinMode(BTN_SCAN, INPUT_PULLUP);
   pinMode(BTN_CLOSE, INPUT_PULLUP);
   pinMode(LED_PIN, OUTPUT);

   sdOk = sdInit();
   if (sdOk) logEvent("INFO", "BOOT", "system_start");
   else Serial.println("SD init failed");
}



void loop() {
  logEvent("INFO", "HEARTBEAT", "ok");

  if (digitalRead(BTN_SCAN) == LOW) {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }

  if (digitalRead(BTN_CLOSE) == LOW) {
    openDoor();
    delay(5000);
    closeDoor();
  }


  int adcVal = analogRead(A0);
  float voltage = adcVal * (ADC_VREF / ADC_RESOLUTION);
  float tempC = (voltage - 0.5) * 100;
  char buffer[5];

  char key = KEYPAD.getKey();
  if (key) {
    tone(BUZZER, 440, 100);
    LCD_MESSAGE += key;
    Serial.println(key);
    DISPLAY_SCREEN.setCursor(0, 0);
    DISPLAY_SCREEN.print(LCD_MESSAGE);
    DISPLAY_SCREEN.setBacklight(1);
    DISPLAY_SCREEN.setBacklight(0);
  }
}


// FUNCTIONS
void openDoor(){
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(RELAY1, LOW);
}

void closeDoor(){
  digitalWrite(LED_PIN, LOW); // Apaga el LED
  digitalWrite(RELAY1, HIGH);
}

bool sdInit() {
  if (!SD.begin(SD_CS)) return false;
  return true;
}

void logEvent(const char* level, const char* event, const char* details) {
  if (!sdOk) return;

  File f = SD.open("/log.txt", FILE_WRITE);
  if (!f) return;

  unsigned long t = millis();
  f.print(t);
  f.print(",");
  f.print(level);
  f.print(",");
  f.print(event);
  f.print(",");
  f.println(details);

  f.close();

  Serial.println('OK');
}
