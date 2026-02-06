#include <DHT.h>
#include <SPI.h>
#include <SD.h>
#include <Keypad.h>
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <ezButton.h>


// ====== COMUNICACION CON ESP ............................................................................................
String espLine = "";
const size_t ESP_MAX_LINE = 64;
void sendFileToEsp(const String& filename);
void readEspCommands();





// ====== LECTOR MICRO SD Y RELOJ .........................................................................................
const int SD_CS = 53;
bool sdOk = false;
RTC_DS3231 rtc;








// ====== HUMEDAD Y TEMPERATURA ...........................................................................................
#define DHTPIN 9
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);







// ====== DISPLAY LCD .....................................................................................................
String LCD_MESSAGE = "";
LiquidCrystal_I2C lcd(0x27, 20, 2);








// ====== KEYPAD ..........................................................................................................
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
 {'1', '2', '3', 'A'},
 {'4', '5', '6', 'B'},
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {29, 31, 33, 35};
byte colPins[COLS] = {37, 39, 41, 43};
Keypad KEYPAD = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);







// ====== LECTOR DE HUELLAS ...............................................................................................
void enroll();
void searchOnce();
void searchLoop();
void deleteById();
void emptyDatabase();

static const uint32_t FINGER_SENSOR_BAUD = 57600;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);









// ====== RELAY DE PUERTA Y BOTONES .......................................................................................
void openDoor(int fingerID);
void closeDoor();

const int RELAY = 47;
const int BTN_CLOSE = 5;
const int BTN_SCAN = 6;
const int BTN_NEW = 7;







// ====== UTILIDADES ......................................................................................................
const int BUZZER = 8;
int loopCounter = 0;




// ====== SWITCHES ......................................................................................................
ezButton top1(7);
ezButton top2(8);
ezButton bot1(9);
ezButton bot2(10);




// ======= FLUJO PRINCIPAL ................................................................................................
void setup()
{
  Serial.begin(9600);
  Serial1.begin(FINGER_SENSOR_BAUD);
  Serial2.begin(115200);  // ESP8266


  finger.begin(FINGER_SENSOR_BAUD);

  Wire.begin();
  sdOk = sdInit();

  if (!rtc.begin()) {
    Serial.println("No se encontró el DS3231");
    writeSystem("ERROR: CLOCK DS3231 NOT FOUND");
    while (1);
  } else {
    writeSystem("INFO-COMP: CLOCK OK");
    if (rtc.lostPower()) {
      Serial.println("RTC perdió energia, ajustando hora...");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // hora de compilación
    }
  }

  if (sdOk) {
    writeSystem("INFO-COMP: STARTING SD");
    writeSystem("INFO-COMP: MICRO SD OK");
    Serial.println("SD OK");
  }
  else Serial.println("SD init failed");

  writeSystem("==================================================================");
  writeSystem("INFO-SYS: SYSTEM BOOT");
  writeSystem("INFO-SYS: BEGIN SERIAL 9600");

  pinMode(RELAY, OUTPUT);

  writeSystem("INFO-SYS: BEGIN DHT SENSOR");
  dht.begin();

  writeSystem("INFO-SYS: BEGIN LCD I2C");
  lcd.init();   
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("helllo");
  lcd.setCursor(0, 1);
  lcd.print("world!!");


  writeSystem("INFO-SYS: SETTING PIN CONFIGURATION");
  pinMode(BTN_SCAN, INPUT_PULLUP);
  pinMode(BTN_CLOSE, INPUT_PULLUP);
  pinMode(BTN_NEW, INPUT_PULLUP);


  // writeSystem("INFO-COMP: STARTING CLOCK (DS3231)");

  top1.setDebounceTime(50);
  top2.setDebounceTime(50);
  bot1.setDebounceTime(50);
  bot2.setDebounceTime(50);

  for (int i = 29; i <= 43; i += 2) {
    // Serial.println(i);
    pinMode(i, INPUT_PULLUP);
    // pinMode(i, OUTPUT);
  }
  // delay(3000);
}

void loop()
{
  top1.loop();
  top2.loop();
  bot1.loop();
  bot2.loop();

  // INNER SWITCHES
  if(top1.isPressed())
    writeActivity("SWITCH: TOP LEFT SWITCH IS IN USE");

  if(top1.isReleased())
    writeActivity("SWITCH: TOP LEFT SWITCH IS REMOVED");

  if(top2.isPressed())
    writeActivity("SWITCH: TOP RIGHT SWITCH IS IN USE");

  if(top2.isReleased())
    writeActivity("SWITCH: TOP RIGHT SWITCH IS REMOVED");


  if(bot1.isPressed())
    writeActivity("SWITCH: BOT LEFT SWITCH IS IN USE");

  if(bot1.isReleased())
    writeActivity("SWITCH: BOT LEFT SWITCH IS REMOVED");

  if(bot2.isPressed())
    writeActivity("SWITCH: BOT RIGHT SWITCH IS IN USE");

  if(bot2.isReleased())
    writeActivity("SWITCH: BOT RIGHT SWITCH IS REMOVED");


  // PANEL BUTTONS
  if (digitalRead(BTN_SCAN) == LOW) {
    writeActivity("BUTTON: SCAN BUTTON PRESS");
    tone(BUZZER, 440, 500);
    lcd.clear();
    lcd.print("Escanear dedo");
    searchOnce();
  }

  if (digitalRead(BTN_CLOSE) == LOW) {
    writeActivity("BUTTON: CLOSE BUTTON PRESS");
    tone(BUZZER, 9000, 500);
    lcd.clear();
    lcd.print("Cerrando...");
    closeDoor();
  }

  if (digitalRead(BTN_NEW) == LOW) {
    writeActivity("BUTTON: NEW BUTTON PRESS");
    tone(BUZZER, 1000, 500);
    lcd.clear();
    lcd.print("Nueva Huella ");
    enroll();
  }



  if(loopCounter % 10 != 0) {
    if(loopCounter == 1) {
      lcd.clear();
      lcdPrintRow(0, "TEC SMART WARE");
      lcdPrintRow(1, "10.124.7.174:3000");
    }
    loopCounter++;
  } else {
    float humity = dht.readHumidity();
    float temperature = dht.readTemperature(); // °C

    if (isnan(humity) || isnan(temperature)) {
      Serial.println("Error leyendo el sensor DHT11");
      writeSystem("ERROR: CANNOT READ TEMPERATURE");
    }

    writeTemperature(temperature, humity);
    showTemperature(temperature, humity);
    delay(2000);
    loopCounter = 1;
  }


  readEspCommands();
  delay(1000); 
}


bool sdInit() {
  if (!SD.begin(SD_CS)) return false;
  return true;
}


// LCD UTILITIES ..........................................................................................................
void lcdPrintRow(int row, String text){
  lcd.setCursor(0, row);
  lcd.print("                    "); // 20 spaces to clear the line
  lcd.setCursor(0, row);
  lcd.print(text);
}

void showTemperature(float temperature, float humidity) {
  lcd.clear();
  lcdPrintRow(0, "Temp  " + String(temperature) + " C");
  lcdPrintRow(1, "Hum   " + String(humidity) + "%");
}

void printMenu(){

}



// DOOR SYSTEM ............................................................................................................
void openDoor(int fingerID){
  digitalWrite(RELAY, HIGH);
  writeActivity("Door opened for fingerprint ID: " + String(fingerID));
}

void closeDoor(){
  digitalWrite(RELAY, LOW);
  writeActivity("Door closed");
}




// FIGERPRINT SYSTEM ......................................................................................................
int readIntBlocking() {
  File dataFile = SD.open("emp.csv", FILE_READ);
  int maxId = 0;
  if (dataFile) {
    while (dataFile.available()) {
      String line = dataFile.readStringUntil('\n');
      int commaIndex = line.indexOf(',');
      if (commaIndex != -1) {
        int id = line.substring(0, commaIndex).toInt();
        if (id > maxId) {
          maxId = id;
        }
      }
    }
    dataFile.close();
  }
  return maxId + 1;
}

void waitFingerRemoved() {
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(50);
  }
}

void waitFingerPlaced() {
  while (true) {
    int p = finger.getImage();
    if (p == FINGERPRINT_OK) return;
    if (p != FINGERPRINT_NOFINGER) {
      delay(100);
    }
    delay(50);
  }
}


void enroll() {
  int id = readIntBlocking();
  Serial.print(F("ID seleccionado: "));
  Serial.println(id);

  Serial.println(F("Coloca el dedo..."));
  lcd.clear();
  lcdPrintRow(0, "Coloca el dedo");
  waitFingerPlaced();

  int p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ Error al convertir imagen (paso 1)."));
    lcdPrintRow(0, "Error leyendo 1");
    return;
  }

  delay(1200);
  waitFingerRemoved();

  Serial.println(F("Coloca el MISMO dedo otra vez..."));
  lcdPrintRow(0, "Coloca el MISMO dedo");
  waitFingerPlaced();

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ Error al convertir imagen (paso 2)."));
    lcdPrintRow(0, "Error leyendo 2");
    return;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ No pude crear el modelo (¿mismo dedo? ¿se movio?)."));
    lcdPrintRow(0, "Error creando modelo");
    return;
  }

  p = finger.storeModel((uint16_t)id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("✅ Huella guardada correctamente."));
    lcdPrintRow(0, "Huella guardada!");
    delay(2000);

    lcd.clear();
    lcdPrintRow(0, "# EMPLEADO:");
    lcdPrintRow(1, LCD_MESSAGE);
    
    while(true) {
      char key = KEYPAD.getKey();
      if (key) {
        tone(BUZZER, 440, 100);
        if(key == 'D') {
          int lastIndex = LCD_MESSAGE.length() - 1;
          LCD_MESSAGE.remove(lastIndex);
        } else {
          LCD_MESSAGE += key;
        }
        
        lcdPrintRow(0, "# EMPLEADO:");
        lcdPrintRow(1, LCD_MESSAGE);
        Serial.println(LCD_MESSAGE);
        delay(100);

        if (key == 'A') {
          File dataFile = SD.open("emp.csv", FILE_WRITE);
          if (dataFile) {
            dataFile.print(id);
            dataFile.print(",");
            dataFile.println(LCD_MESSAGE);
            dataFile.close();
            Serial.println("✅ Empleado guardado en emp.csv");
            lcdPrintRow(0, "Empleado guardado");
            lcdPrintRow(1, LCD_MESSAGE);
            LCD_MESSAGE = "";
            writeSystem("Empleado ID " + String(id) + " guardado como " + LCD_MESSAGE);
            delay(2000);
          } else {
            Serial.println("❌ Error al abrir emp.csv");
            writeSystem("ERROR: Al abrir emp.csv para guardar empleado ID " + String(id));
            lcdPrintRow(0, "Error guardando");
            delay(2000);
          }

          break;
        }
      }
    }
  } else {
    Serial.println(F("❌ Error al guardar la huella."));
    lcdPrintRow(0, "Error guardando");
  }
}

void searchOnce() {
  lcdPrintRow(0, "Coloca el dedo");
  waitFingerPlaced();

  int p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("Error al convertir imagen."));
    lcdPrintRow(0, "Error leyendo");
    lcdPrintRow(1, "quita el dedo");
    waitFingerRemoved();
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print(finger.fingerID);
    Serial.println(finger.confidence);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Huella ID:");
    lcd.print(finger.fingerID);
    lcd.setCursor(0, 1);
    lcd.print("Acceso Permitido");
    openDoor(finger.fingerID);
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println(F("No encontrada (no coincide / no esta enrolada)."));
    lcd.clear();
    lcdPrintRow(0, "Huella NO");
    lcdPrintRow(1, "Reconocida");
  } else {
    Serial.println(F("Error buscando huella."));
    lcd.clear();
    lcdPrintRow(0, "Error buscando");
  }

  delay(3000);
  Serial.println(F("Retira el dedo..."));
  waitFingerRemoved();
}

void deleteById(int id) {
  int p = finger.deleteModel((uint16_t)id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("✅ Huella borrada."));
    writeSystem("Huella ID " + String(id) + " borrada");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("❌ Error de comunicacion al borrar."));
    writeSystem("ERROR: Comunicación al borrar huella ID " + String(id));
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println(F("❌ Ubicacion (ID) invalida."));
    writeSystem("ERROR: Ubicación inválida al borrar huella ID " + String(id));
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println(F("❌ Error escribiendo en memoria."));
    writeSystem("ERROR: Escritura en memoria al borrar huella ID " + String(id));
  } else {
    Serial.println(F("❌ No se pudo borrar (error desconocido)."));
    writeSystem("ERROR: Desconocido al borrar huella ID " + String(id));
  }
}

void emptyDatabase() {
  int p = finger.emptyDatabase();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("✅ Memoria vaciada."));
    writeSystem("Memoria de huellas vaciada");
  } else {
    Serial.println(F("❌ Error vaciando memoria."));
    writeSystem("ERROR: Al vaciar memoria de huellas");
  }
}






// LOG SYSTEM ......................................................................................................

void writeActivity(String message) {
  File dataFile = SD.open("ACTIV.CSV", FILE_WRITE);
  if (dataFile) {
    DateTime now = rtc.now();
    dataFile.print(now.year());
    dataFile.print("-");
    dataFile.print(now.month());
    dataFile.print("-");
    dataFile.print(now.day());
    dataFile.print(" ");
    dataFile.print(now.hour());
    dataFile.print(":");
    dataFile.print(now.minute());
    dataFile.print(":");
    dataFile.print(now.second());
    dataFile.print(",");
    dataFile.println(message);
    dataFile.close();
  } else {
    Serial.println("Error al abrir activity.csv");
  }
}

void writeTemperature(float temperature, float humidity) {

  if (!SD.exists("TEMPER.CSV")) {
    File headerFile = SD.open("TEMPER.CSV", FILE_WRITE);
    if (headerFile) {
      headerFile.println("Timestamp,Temperature,Humidity");
      headerFile.close();
    } else {
      Serial.println("Error al crear TEMPER.CSV");
    }
  }


  File dataFile = SD.open("TEMPER.CSV", FILE_WRITE);

  if (dataFile) {
    DateTime now = rtc.now();
    dataFile.print(now.year());
    dataFile.print("-");
    dataFile.print(now.month());
    dataFile.print("-");
    dataFile.print(now.day());
    dataFile.print(" ");
    dataFile.print(now.hour());
    dataFile.print(":");
    dataFile.print(now.minute());
    dataFile.print(":");
    dataFile.print(now.second());
    dataFile.print(",");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.println(humidity);
    dataFile.close();
  } else {
    Serial.println("Error al abrir temperature.csv");
  }
}

void writeSystem(String message) {
  File dataFile = SD.open("SYSTEM.LOG", FILE_WRITE);
  if (dataFile) {
    DateTime now = rtc.now();
    dataFile.print(now.year());
    dataFile.print("-");
    dataFile.print(now.month());
    dataFile.print("-");
    dataFile.print(now.day());
    dataFile.print(" ");
    dataFile.print(now.hour());
    dataFile.print(":");
    dataFile.print(now.minute());
    dataFile.print(":");
    dataFile.print(now.second());
    dataFile.print(" - ");
    dataFile.println(message);
    dataFile.close();
  } else {
    Serial.println("Error al abrir SYSTEM.LOG");
  }
}


void sendFileToEsp(const String& filename) {

  String targetFilename = "";

  if(filename == "A") {
    targetFilename = "ACTIV.CSV";
  } else if (filename == "T") {
    targetFilename = "TEMPER.CSV";
  } else if (filename == "S") {
    targetFilename = "SYSTEM.LOG";
  } else {
    return;
  }

  File f = SD.open(targetFilename, FILE_READ);

  // Sin validaciones “de nombre”; pero sí hay que evitar crashear si algo falla:
  if (!f) {
    Serial2.print("OK 0\n");   // fallback simple: 0 bytes
    return;
  }

  uint32_t size = f.size();

  // Header
  Serial2.print("OK ");
  Serial2.print(size);
  Serial2.print("\n");

  // Stream por chunks
  uint8_t buf[64];
  while (f.available()) {
    size_t n = f.read(buf, sizeof(buf));
    if (n) Serial2.write(buf, n);
  }

  f.close();
}


void readEspCommands() {
    char c = (char)Serial2.read();

    if (c == '\r') {
    } else if (c == '\n') {
      espLine.trim();
      espLine = espLine.substring(espLine.indexOf('F'));
      if (espLine.length() > 0) {
        if(!espLine.startsWith("F")) {
          Serial.print("Serial Data: ");
          Serial.println(espLine);
        }
        else {
          Serial.print("Sending file content from: ");
          espLine = espLine.substring(1);
          Serial.println(espLine);
          sendFileToEsp(espLine);
        }
      }
      espLine = "";
      return;
    } else {
      if (espLine.length() < ESP_MAX_LINE) {
        espLine += c;
      } else {
        espLine = "";
        return;
      }
  }
}

