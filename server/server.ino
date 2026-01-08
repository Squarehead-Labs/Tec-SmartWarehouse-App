#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Totalplay-2.4G-d370";
const char* password = "6LUhQH3g6eCyFp86";

ESP8266WebServer server(3000); // Puerto HTTP estándar

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
            window.location.href = `test.html?q=${option}`;            
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

void handleTemperatureCsv(){
  server.send(200, "text/plain",R"rawliteral(date,temperature °C,humidity %
2026-01-01 00:00:00,22.4,55
2026-01-01 01:00:00,22.1,56
2026-01-01 02:00:00,21.8,57
2026-01-01 03:00:00,21.5,58
2026-01-01 04:00:00,21.2,60
2026-01-01 05:00:00,21.0,61
2026-01-01 06:00:00,21.6,62
2026-01-01 07:00:00,22.3,60
2026-01-01 08:00:00,23.5,58
2026-01-01 09:00:00,24.8,55
2026-01-01 10:00:00,26.1,52
2026-01-01 11:00:00,27.4,50
2026-01-01 12:00:00,28.2,48
2026-01-01 13:00:00,29.0,47
2026-01-01 14:00:00,29.5,45
2026-01-01 15:00:00,29.1,46
2026-01-01 16:00:00,28.4,48
2026-01-01 17:00:00,27.6,50
2026-01-01 18:00:00,26.3,52
2026-01-01 19:00:00,25.1,53)rawliteral");
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
  server.on("/data/temperature", HTTP_GET, handleTemperatureCsv);
  server.onNotFound(handleNotFound);

  // -------- INICIAR SERVER --------
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient(); // MUY IMPORTANTE
}
