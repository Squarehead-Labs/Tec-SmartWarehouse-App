#include <Adafruit_Fingerprint.h>

// Usamos Serial1 en el Mega
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("\n[R307] Iniciando...");

  // IMPORTANTE: Serial1 usa los pines de hardware (RX1/TX1)
  // En tu caso: RX1=18, TX1=19 (según tu serigrafía)
  Serial1.begin(57600);
  finger.begin(57600);

  delay(200);

  if (finger.verifyPassword()) {
    Serial.println("✅ Sensor detectado y password OK.");
  } else {
    Serial.println("❌ No detecto el sensor.");
    Serial.println("Checklist:");
    Serial.println("- VCC a 5V, GND a GND");
    Serial.println("- R307 TX -> Mega RX1 (tu pin 18)");
    Serial.println("- R307 RX -> Mega TX1 (tu pin 19)");
    Serial.println("- Baud 57600 (si no, prueba 9600)");
    while (true) delay(1);
  }

  Serial.print("Capacidad: "); Serial.println(finger.capacity);
  Serial.print("Seguridad: "); Serial.println(finger.security_level);
  Serial.println("Listo ✅");
}

void loop() {
  // Nada
}
