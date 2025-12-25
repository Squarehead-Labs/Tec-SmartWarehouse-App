const int RELAY1 = 7;
const int RELAY2 = 8;
const int LED_PIN = 53;


void setup() {
  pinMode(RELAY1, OUTPUT);
  // pinMode(RELAY2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Relays apagados al iniciar
  digitalWrite(RELAY1, HIGH);
  // digitalWrite(RELAY2, HIGH);
  digitalWrite(LED_PIN, HIGH); // Enciende el LED
}

void loop() {
  // Activa relay 1
  digitalWrite(LED_PIN, LOW); // Apaga el LED
  digitalWrite(RELAY1, HIGH);
  // digitalWrite(RELAY2, HIGH);
  delay(5000);


  digitalWrite(LED_PIN, HIGH); // Apaga el LED
  digitalWrite(RELAY1, LOW);
  // digitalWrite(RELAY2, LOW);
  delay(5000);
}
