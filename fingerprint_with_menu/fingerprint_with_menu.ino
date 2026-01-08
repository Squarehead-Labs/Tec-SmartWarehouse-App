#include <Adafruit_Fingerprint.h>

// ====== CONFIG ======
static const uint32_t PC_BAUD     = 115200;
static const uint32_t SENSOR_BAUD = 57600;   // si tu R307 no responde, prueba 9600

// Usamos Serial1 en Mega (en tu placa: RX1=18, TX1=19 según serigrafía)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

// ====== UTILS ======
String readLineBlocking() {
  while (!Serial.available()) { delay(5); }
  String s = Serial.readStringUntil('\n');
  s.trim();
  // Si te llega '\r' por el monitor serial, trim() lo quita.
  return s;
}

int readIntBlocking() {
  String s = readLineBlocking();
  return s.toInt();
}

void printMenu() {
  Serial.println();
  Serial.println(F("========= R307 MENU ========="));
  Serial.println(F("1) Enrolar (guardar) huella en ID"));
  Serial.println(F("2) Leer / Buscar huella (match)"));
  Serial.println(F("3) Borrar huella por ID"));
  Serial.println(F("4) Vaciar TODA la memoria"));
  Serial.println(F("5) Info del sensor"));
  Serial.println(F("m) Mostrar menu"));
  Serial.println(F("============================="));
  Serial.print(F("Opcion: "));
}

void waitFingerRemoved() {
  // Espera a que el dedo se quite
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(50);
  }
}

void waitFingerPlaced() {
  // Espera a que ponga el dedo y la imagen sea OK
  while (true) {
    int p = finger.getImage();
    if (p == FINGERPRINT_OK) return;
    if (p != FINGERPRINT_NOFINGER) {
      // Errores intermitentes
      delay(100);
    }
    delay(50);
  }
}

// ====== SENSOR OPS ======
bool ensureSensor() {
  if (finger.verifyPassword()) return true;

  Serial.println(F("❌ No detecto el sensor / password incorrecto."));
  Serial.println(F("Checklist:"));
  Serial.println(F("- VCC->5V, GND->GND"));
  Serial.println(F("- R307 TX -> Mega RX1 (tu pin 18)"));
  Serial.println(F("- R307 RX -> Mega TX1 (tu pin 19)"));
  Serial.print(F("- Baud actual: ")); Serial.println(SENSOR_BAUD);
  Serial.println(F("Tip: si no responde, cambia SENSOR_BAUD a 9600 y reintenta."));
  return false;
}

void showInfo() {
  Serial.println();
  Serial.println(F("=== Info del sensor ==="));
  Serial.print(F("Status: "));
  Serial.println(ensureSensor() ? F("OK") : F("ERROR"));

  Serial.print(F("Capacidad: "));
  Serial.println(finger.capacity);

  Serial.print(F("Nivel seguridad: "));
  Serial.println(finger.security_level);

  Serial.print(F("Device address: 0x"));
  Serial.println(finger.device_addr, HEX);

  Serial.print(F("Packet length: "));
  Serial.println(finger.packet_len);

  Serial.println(F("======================="));
}

// Enrolar huella en un ID (1..127 normalmente)
void enroll() {
  Serial.println();
  Serial.println(F("== ENROLAR HUELLA =="));
  Serial.print(F("Ingresa ID (1-127): "));
  int id = readIntBlocking();

  if (id < 1 || id > 127) {
    Serial.println(F("❌ ID invalido."));
    return;
  }

  Serial.print(F("ID seleccionado: "));
  Serial.println(id);

  Serial.println(F("Coloca el dedo..."));
  waitFingerPlaced();

  int p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ Error al convertir imagen (paso 1)."));
    return;
  }

  Serial.println(F("✅ Imagen 1 capturada. Retira el dedo..."));
  delay(1200);
  waitFingerRemoved();

  Serial.println(F("Coloca el MISMO dedo otra vez..."));
  waitFingerPlaced();

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ Error al convertir imagen (paso 2)."));
    return;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ No pude crear el modelo (¿mismo dedo? ¿se movio?)."));
    return;
  }

  p = finger.storeModel((uint16_t)id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("✅ Huella guardada correctamente."));
  } else {
    Serial.println(F("❌ Error al guardar la huella."));
  }
}

// Buscar una huella (match) una sola vez
void searchOnce() {
  Serial.println();
  Serial.println(F("== BUSCAR HUELLA (1 intento) =="));
  Serial.println(F("Coloca el dedo..."));

  // Esperar dedo
  waitFingerPlaced();

  int p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("❌ Error al convertir imagen."));
    waitFingerRemoved();
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print(F("✅ Encontrada! ID="));
    Serial.print(finger.fingerID);
    Serial.print(F("  Confianza="));
    Serial.println(finger.confidence);
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println(F("❌ No encontrada (no coincide / no esta enrolada)."));
  } else {
    Serial.println(F("⚠️ Error buscando huella."));
  }

  Serial.println(F("Retira el dedo..."));
  waitFingerRemoved();
}

// Buscar huellas en modo continuo hasta que presiones ENTER
void searchLoop() {
  Serial.println();
  Serial.println(F("== BUSCAR HUELLA (continuo) =="));
  Serial.println(F("Pon tu dedo para buscar. Presiona ENTER para salir."));
  Serial.println();

  // Vaciar buffer por si quedó algo
  while (Serial.available()) Serial.read();

  while (true) {
    if (Serial.available()) {
      // ENTER para salir
      char c = Serial.read();
      (void)c;
      Serial.println(F("\nSaliendo de busqueda continua."));
      return;
    }

    int p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(30);
      continue;
    }
    if (p != FINGERPRINT_OK) {
      delay(80);
      continue;
    }

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) {
      Serial.println(F("❌ No pude convertir imagen."));
      waitFingerRemoved();
      continue;
    }

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
      Serial.print(F("✅ Match! ID="));
      Serial.print(finger.fingerID);
      Serial.print(F("  Confianza="));
      Serial.println(finger.confidence);
    } else if (p == FINGERPRINT_NOTFOUND) {
      Serial.println(F("❌ No coincide."));
    } else {
      Serial.println(F("⚠️ Error en busqueda."));
    }

    waitFingerRemoved();
    delay(150);
  }
}

void deleteById() {
  Serial.println();
  Serial.println(F("== BORRAR POR ID =="));
  Serial.print(F("Ingresa ID a borrar (1-127): "));
  int id = readIntBlocking();

  if (id < 1 || id > 127) {
    Serial.println(F("❌ ID invalido."));
    return;
  }

  int p = finger.deleteModel((uint16_t)id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("✅ Huella borrada."));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(F("❌ Error de comunicacion al borrar."));
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println(F("❌ Ubicacion (ID) invalida."));
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println(F("❌ Error escribiendo en memoria."));
  } else {
    Serial.println(F("❌ No se pudo borrar (error desconocido)."));
  }
}

void emptyDatabase() {
  Serial.println();
  Serial.println(F("== VACIAR TODA LA MEMORIA =="));
  Serial.println(F("⚠️ Esto borra TODAS las huellas del sensor."));
  Serial.print(F("Escribe 'SI' para confirmar: "));

  String confirm = readLineBlocking();
  confirm.toUpperCase();

  if (confirm != "SI") {
    Serial.println(F("Cancelado."));
    return;
  }

  int p = finger.emptyDatabase();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("✅ Memoria vaciada."));
  } else {
    Serial.println(F("❌ Error vaciando memoria."));
  }
}

// ====== SETUP/LOOP ======
void setup() {
  Serial.begin(PC_BAUD);
  while (!Serial) {}

  Serial.println(F("\n[R307] Menu por Serial (Mega + Serial1)"));
  Serial.print(F("PC baud: ")); Serial.println(PC_BAUD);
  Serial.print(F("Sensor baud: ")); Serial.println(SENSOR_BAUD);

  Serial1.begin(SENSOR_BAUD);
  finger.begin(SENSOR_BAUD);

  if (ensureSensor()) {
    Serial.println(F("✅ Sensor listo."));
    showInfo();
  } else {
    Serial.println(F("❌ Sensor no listo (revisa cables/baud)."));
  }

  printMenu();
}

void loop() {
  if (!Serial.available()) return;

  String opt = readLineBlocking();
  opt.trim();
  if (opt.length() == 0) {
    printMenu();
    return;
  }

  char c = opt.charAt(0);

  // Siempre intenta verificar antes de operar
  if (!ensureSensor()) {
    printMenu();
    return;
  }

  switch (c) {
    case '1':
      enroll();
      break;

    case '2': {
      Serial.println();
      Serial.println(F("Leer/Buscar:"));
      Serial.println(F("a) Un intento"));
      Serial.println(F("b) Continuo (ENTER para salir)"));
      Serial.print(F("Elige (a/b): "));
      String mode = readLineBlocking();
      if (mode.length() && (mode[0] == 'b' || mode[0] == 'B')) searchLoop();
      else searchOnce();
      break;
    }

    case '3':
      deleteById();
      break;

    case '4':
      emptyDatabase();
      break;

    case '5':
      showInfo();
      break;

    case 'm':
    case 'M':
      // solo menu
      break;

    default:
      Serial.println(F("Opcion no valida."));
      break;
  }

  printMenu();
}
