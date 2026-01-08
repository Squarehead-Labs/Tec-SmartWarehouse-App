#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Cambia 0x27 por la dirección que te salga en el scanner
LiquidCrystal_I2C lcd(0x27, 20, 2);  // si tu LCD es 16x2 cambia a (0x27, 16, 2)

void setup() {
  Wire.begin();
  lcd.init();          // inicializa el LCD
  lcd.backlight();     // enciende la luz de fondo

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD I2C OK");
  lcd.setCursor(0, 1);
  lcd.print("Arduino Mega");
}

void loop() {
  // demo: contador
  static unsigned long t0 = 0;
  static int sec = 0;

  if (millis() - t0 >= 1000) {
    t0 = millis();
    sec++;
    lcd.setCursor(15, 1); // esquina (ajusta si es 20x2)
    lcd.print(sec % 10);
  }
}
