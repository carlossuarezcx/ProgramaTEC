#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "HOME";
const char* password = "3029251610";
const int tiempoEspera = 3000;

ESP8266WebServer server(80);

void procesarSecuencia(String secuencia) {
    for (int i = 0; i < secuencia.length(); i++) {
        char movimiento = secuencia.charAt(i);
        switch (movimiento) {
            case 'W':
                moverAdelante();
                break;
            case 'S':
                moverAtras();
                break;
            case 'A':
                moverIzquierda();
                break;
            case 'D':
                moverDerecha();
                break;
            default:
                Serial.println("Comando desconocido: " + String(movimiento));
                break;
        }
        delay(tiempoEspera);
    }
}
void moverAdelante() {
    Serial.println("Moviendo hacia adelante");
}

void moverAtras() {
    Serial.println("Moviendo hacia atrás");
}

void moverIzquierda() {
    Serial.println("Moviendo a la izquierda");
}

void moverDerecha() {
    Serial.println("Moviendo a la derecha");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi!");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/play", HTTP_POST, play);
  server.on("/stop", HTTP_POST, stop);
  server.begin();
}

void play() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    String secuencia = server.arg("plain");
    server.send(200, "text/plain", "Secuencia procesada");
    procesarSecuencia(secuencia);
  } else {
    server.send(400, "text/plain", "No se recibió secuencia");
  }
}
void stop() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    String secuencia = server.arg("plain");
    server.send(200, "text/plain", "Deteniendo");
    Serial.println("Detenido");
  } else {
    server.send(400, "text/plain", "No se recibió secuencia");
  }
}
void loop() {
  server.handleClient();
}
