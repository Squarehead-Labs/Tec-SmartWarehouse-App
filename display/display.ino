// /*
// gnd red 
// vcc  white
// sda azul
// scl whie
// */
// #include <Wire.h> 
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 20, 2);

// void setup() {
//   lcd.init(); // Inicializa el módulo LCD
  
//   lcd.backlight(); // Enciende la luz de fondo
  
//   Serial.begin(9600); // Inicia el monitor serie para depuración
//   Serial.println("Iniciando LCD...");

//   lcd.clear(); // Limpia la pantalla
//   lcd.setCursor(0, 0); // Posiciona el cursor en la columna 0, fila 0 (esquina superior izquierda)
//   lcd.print("Hola Mundo!"); // Imprime el texto

//   // Puedes añadir más texto o variables
//   lcd.setCursor(0, 1); // Mueve el cursor a la segunda fila
//   lcd.print("Arduino I2C");
// }

// void loop() {
//   Serial.println('aaaa');
// lcd.setCursor(0, 1); // Posiciona el cursor
//   lcd.print("Contador: ");
//   lcd.print(millis() / 1000); // Muestra segundos transcurridos
//   delay(1000); // Espera 1 segundo
// }


// #include <Wire.h>

// void setup() {
//   Wire.begin();
//   Serial.begin(9600);
//   while (!Serial) {}

//   Serial.println("Escaneando I2C...");
//   for (byte addr = 1; addr < 127; addr++) {
//     Wire.beginTransmission(addr);
//     if (Wire.endTransmission() == 0) {
//       Serial.print("Encontrado en 0x");
//       Serial.println(addr, HEX);
//     }
//   }
// }

// void loop() {}


// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 20, 2); // <-- cambia 0x27 por la tuya

// void setup() {
//   lcd.init();
//   lcd.backlight();
//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("LCD OK");
//   lcd.setCursor(0,1);
//   lcd.print("I2C funcionando");
// }

// void loop() {}


// #include <Wire.h>
// #include <hd44780.h>
// #include <hd44780ioClass/hd44780_I2Cexp.h>

// // crea el objeto LCD
// hd44780_I2Cexp lcd;

// void setup() {
//   Serial.begin(9600);

//   int status = lcd.begin(20, 2);  // cambia a (20, 4) si tu LCD es 20x4

//   if (status) {
//     Serial.print("LCD error: ");
//     Serial.println(status);
//     while (1); // se queda aquí si falla
//   }

//   lcd.backlight();
//   lcd.clear();

//   lcd.setCursor(0,0);
//   lcd.print("LCD OK");
//   lcd.setCursor(0,1);
//   lcd.print("hd44780 listo");
// }

// void loop() {}


// #include <Wire.h>
// #include <hd44780.h>
// #include <hd44780ioClass/hd44780_I2Cexp.h>

// hd44780_I2Cexp lcd;

// void setup() {
//   Serial.begin(9600);
//   while(!Serial) {}

//   Serial.println("Iniciando LCD...");

//   int status = lcd.begin(16, 2); // cambia a 20,4 si es 20x4
//   Serial.print("lcd.begin status = ");
//   Serial.println(status);

//   if (status) {
//     Serial.println("Fallo al inicializar LCD.");
//     Serial.println("Revisa: soldadura del backpack, SDA/SCL, GND comun, 5V.");
//     while (1);
//   }

//   lcd.backlight();
//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("LCD OK");
//   lcd.setCursor(0,1);
//   lcd.print("hd44780 OK");

//   Serial.println("Texto enviado al LCD.");
// }

// void loop() {}










// #include <Wire.h>

// void setup() {
//   Wire.begin();
//   Serial.begin(9600);
//   while (!Serial);

//   Serial.println("Scanning...");
// }

// void loop() {
//   byte error, address;
//   int nDevices = 0;

//   for (address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.print("I2C device found at address 0x");
//       if (address < 16) Serial.print("0");
//       Serial.println(address, HEX);
//       nDevices++;
//     }
//   }

//   if (nDevices == 0)
//     Serial.println("No I2C devices found");

//   delay(5000);
// }


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 2);  // dirección correcta ✅

void setup() {
  lcd.init();        // inicializa el LCD
  lcd.backlight();  // enciende la luz
  lcd.setCursor(0, 0);
  lcd.print("Hola mundo");
  Serial.begin(9600);
  Serial.println("ok");
}

void loop() {
}


