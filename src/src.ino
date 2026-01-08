#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>

#define DHTPIN 7       // Pin donde conectaste DATA
#define DHTTYPE DHT22  // Tipo de sensor

DHT dht(DHTPIN, DHTTYPE);


// Cambia 0x27 por la dirección que te salga en el scanner
LiquidCrystal_I2C lcd(0x27, 20, 2);  // si tu LCD es 16x2 cambia a (0x27, 16, 2)

int seconds = 0;
String LCD_MESSAGE = "";
const int BUZZER = 13;



const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
 {'1', '2', '3', 'A'},
 {'4', '5', '6', 'B'},
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {39,41, 43, 45}; // Connect to row pins
byte colPins[COLS] = {47, 49, 51, 53}; // Connect to column pins
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


void setup()
{
  Serial.begin(9600);

  dht.begin();
  Wire.begin();
  lcd.init();   
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
}

void loop()
{
  char key = customKeypad.getKey();
  if (key) {
    tone(BUZZER, 440, 100);
    if(key == 'D') {
      int lastIndex = LCD_MESSAGE.length() - 1;
      LCD_MESSAGE.remove(lastIndex);
    } else {
      LCD_MESSAGE += key;
    }
    
    Serial.println(LCD_MESSAGE);
    lcd.flush();
    lcd.setCursor(0, 0);
    lcd.print("# EMPLEADO:");

    lcd.setCursor(0, 1);
    lcd.print(LCD_MESSAGE);
    delay(100);
  }

  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature(); // °C

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error leyendo el sensor DHT11");
    return;
  }


  lcd.flush();
  lcd.setCursor(0, 0);
  lcd.print("Temp  ");
  lcd.print(temperatura);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Hum   ");
  lcd.print(humedad);
  lcd.print("%");


  Serial.print("Temp.");
  Serial.print(temperatura);
  Serial.println(" C");
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println(" %");


  delay(3000); // El DHT11 es lento, mínimo 1–2s
}


