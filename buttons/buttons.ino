/*
negro/negro -> scan
rojo/rojo -> close
amarillio/naranja -> new
*/

const int btnScan = 2;
const int btnClose = 3;
const int led = 53;


void setup() {
  pinMode(btnScan, INPUT_PULLUP);
  pinMode(btnClose, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(btnScan) == LOW) {
    digitalWrite(led, HIGH);
    delay(300);
  }

  if (digitalRead(btnClose) == LOW) {
    digitalWrite(led, HIGH);
    delay(300);
  }

  digitalWrite(led, LOW);
}
