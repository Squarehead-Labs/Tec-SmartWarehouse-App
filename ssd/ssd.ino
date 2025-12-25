#include <SD.h>
#include <SPI.h>

const int SD_CS = 53;

bool sdOk = false;

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

void setup() {
  Serial.begin(115200);

  sdOk = sdInit();
  if (sdOk) logEvent("INFO", "BOOT", "system_start");
  else Serial.println("SD init failed");
}

void loop() {
  // ejemplo
  logEvent("INFO", "HEARTBEAT", "ok");
  delay(5000);
}