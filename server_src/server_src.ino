#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Totalplay-2.4G-d370";
const char* password = "6LUhQH3g6eCyFp86";
int counter = 0;


ESP8266WebServer server(3000); // Puerto HTTP estándar


// ===== SERIAL <-> MEGA PROTOCOL =====
// Mega responde: "OK <bytes>\n" + <bytes> data

bool readLineFromSerial(String &out, uint32_t timeoutMs) {
  out = "";
  uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    while (Serial.available()) {
      char c = (char)Serial.read();
      if (c == '\r') continue;
      if (c == '\n') return true;
      out += c;
      if (out.length() > 220) return true; // header debe ser chico
    }
    delay(0);
  }
  return false;
}

bool requestFileHeaderFromMega(const String &filename, uint32_t &sizeOut) {
  // Limpia cualquier basura pendiente antes de pedir
  while (Serial.available()) Serial.read();

  // Envía SOLO el nombre + \n (como tu Mega espera)
  Serial.print(filename);
  Serial.print('\n');//dv

  String header;
  if (!readLineFromSerial(header, 3000)) return false;

  if (!header.startsWith("OK ")) return false;

  sizeOut = (uint32_t)header.substring(3).toInt();
  return true;
}

bool pipeExactBytesToClient(WiFiClient &client, uint32_t totalBytes, uint32_t idleTimeoutMs) {
  uint8_t buf[256];
  uint32_t sent = 0;
  uint32_t last = millis();

  while (sent < totalBytes) {
    if (Serial.available()) {
      size_t want = (size_t)min<uint32_t>(sizeof(buf), totalBytes - sent);
      size_t n = Serial.readBytes(buf, want);
      if (n) {
        client.write(buf, n);
        sent += n;
        last = millis();
      }
    } else {
      if (millis() - last > idleTimeoutMs) return false;
      delay(0);
    }
  }
  return true;
}



void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <iframe 
        src='https://smartwh.sqhlabs.com' 
        style='width:100%;height:100vh;border:0;'>
    </iframe>
    <script src='https://smartwh.sqhlabs.com/script.js'></script>
    <script>
        document.getElementsByTagName('iframe')[0].addEventListener('load', function() {
            loadData();
        });

        window.addEventListener('message', (event) => {
            const option = event.data;
            if (typeof option !== 'string') return;
            console.info('Opción desde iframe:', option);
            window.location.href = `/?q=${option}`;            
        });
    </script>
  )rawliteral");
}

void handleVersion() {
  server.send(200, "text/plain", "0.0.3");
}

void handleNotFound() {
  String message = "404 - Ruta no encontrada\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMetodo: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";

  server.send(404, "text/plain", message);
}

void handleDataFile(){
  String filename = server.arg("name");
  if (filename.length() == 0) filename = "log.csv";

  uint32_t size = 0;
  if (!requestFileHeaderFromMega(filename, size)) {
    server.send(502, "text/plain", "Mega: respuesta invalida");
    return;
  }

  WiFiClient client = server.client();
  server.setContentLength(size);
  server.send(200, "text/plain", "");

  if (!pipeExactBytesToClient(client, size, 5000)) client.stop();
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // -------- CONEXION WIFI --------
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // -------- RUTAS --------
  server.on("/", HTTP_GET, handleRoot);
  server.on("/version", HTTP_GET, handleVersion);
  server.on("/data/file", HTTP_GET, handleDataFile);
  server.onNotFound(handleNotFound);

  // -------- INICIAR SERVER --------
  server.begin();
  Serial.println("Servidor HTTP iniciado");


  for(int i = 1; i <= 10; i++) {
    Serial.println(WiFi.localIP());
    delay(1000);
  }
}

void loop() {
  server.handleClient(); // MUY IMPORTANTE
}
