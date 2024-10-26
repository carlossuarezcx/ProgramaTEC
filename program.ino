#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "HOME";
const char* password = "3029251610";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi!");

  Serial.print("Dirección IP del servidor: ");
  Serial.println(WiFi.localIP());

  server.on("/secuencia", HTTP_POST, handleSecuencia);
  server.begin();
}

void handleSecuencia() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  Serial.println("Peticion recibida");
  if (server.hasArg("plain")) {
    String secuencia = server.arg("plain");
    Serial.println("Secuencia recibida: " + secuencia);
    server.send(200, "text/plain", "Secuencia procesada");
  } else {
    server.send(400, "text/plain", "No se recibió secuencia");
  }
}

void loop() {
  server.handleClient();
}
