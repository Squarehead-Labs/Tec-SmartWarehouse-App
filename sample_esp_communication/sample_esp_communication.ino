// ====== COMUNICACION CON ESP ............................................................................................
String espLine = "";
const size_t ESP_MAX_LINE = 64;
void readEspCommands();
void sendFileToEsp(const String& filename);

void setup() {
  Serial.begin(115200);
}

int counter = 1;
void loop() {
  readEspCommands();
  Serial.println(counter);
  counter++;
  delay(1000);
}



void readEspCommands() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    Serial.println(c);

    if (c == '\r') continue;

    if (c == '\n') {
      espLine.trim();
      if (espLine.length() > 0) {
        if(!espLine.startsWith("F")) Serial.println(espLine);
        else sendFileToEsp(espLine.substring(1, espLine.length() - 1));
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
}

void sendFileToEsp(const String& filename) {
  // Payload de prueba (puede ser cualquier cosa)
  const char* payload =
    "time,temp,hum\n"
    "2026-01-23 20:00,25.1,60\n"
    "2026-01-23 20:02,25.0,61\n";

  uint32_t size = strlen(payload);

  // 1) Header requerido por tu ESP
  Serial.print("OK ");
  Serial.print(size);
  Serial.print("\n");

  // 2) Bytes exactos (size)
  Serial.write((const uint8_t*)payload, size);
}

