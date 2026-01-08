#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

const int CS_PIN = 53;  // Chip Select
RTC_DS3231 rtc;


void setup() {
  Serial.begin(9600);
  while (!Serial) {;}

  pinMode(53, OUTPUT); // MUY IMPORTANTE en Mega

  Serial.println("Inicializando SD...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("❌ Error al inicializar la SD");
    return;
  }

  Serial.println("✅ SD inicializada correctamente");

  // Crear / abrir archivo
  File archivo = SD.open("test.txt", FILE_WRITE);

  if (archivo) {
    archivo.println("Hola desde Arduino Mega 😎");
    archivo.close();
    Serial.println("✍️ Escritura OK");
  } else {
    Serial.println("❌ No se pudo escribir");
  }

  // Leer archivo
  archivo = SD.open("test.txt");

  if (archivo) {
    Serial.println("📖 Contenido:");
    while (archivo.available()) {
      Serial.write(archivo.read());
    }
    archivo.close();
  } else {
    Serial.println("❌ No se pudo leer");
  }

  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("❌ No se encontró el DS3231");
    while (1);
  }

  // ⚠️ SOLO LA PRIMERA VEZ
  // Ajusta la hora al momento de compilación
  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC sin energía, ajustando hora...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}


void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.print(" ");

  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  delay(1000);
}