/*
negro/negro -> scan
rojo/rojo -> close
amarillio/naranja -> new
*/

const int btnScan = 2;
const int btnClose = 3;
const int LED_PIN = 53;

const int RELAY1 = 7;
const int RELAY2 = 8;


void setup() {
  pinMode(RELAY1, OUTPUT);
  pinMode(btnScan, INPUT_PULLUP);
  pinMode(btnClose, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(btnScan) == LOW) {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
  }

  if (digitalRead(btnClose) == LOW) {
    openDoor();
    delay(5000);
    closeDoor();
  }
}

void openDoor(){
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(RELAY1, LOW);
}

void closeDoor(){
  digitalWrite(LED_PIN, LOW); // Apaga el LED
  digitalWrite(RELAY1, HIGH);
}