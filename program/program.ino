#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENA D5
#define ENB D6
const int velocidad = 255;

const char* ssid = "HOME";
const char* password = "3029251610";
const int tiempoEspera = 3000;

ESP8266WebServer server(80);

void procesarSecuencia(String secuencia) {
    for (int i = 0; i < secuencia.length(); i++) {
        char movimiento = secuencia.charAt(i);
        switch (movimiento) {
            case 'W':
                adelante();
                break;
            case 'S':
                atras();
                break;
            case 'A':
                izquierda();
                break;
            case 'D':
                derecha();
                break;
            default:
                Serial.println("Comando desconocido: " + String(movimiento));
                break;
        }
        delay(tiempoEspera);
    }
}
void adelante() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void atras() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void izquierda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void derecha() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void detenerMotores() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

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

  server.on("/play", HTTP_POST, iniciar);
  server.on("/stop", HTTP_POST, detener);
  server.begin();

  
  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
}

void iniciar() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    String secuencia = server.arg("plain");
    server.send(200, "text/plain", "Secuencia procesada");
    procesarSecuencia(secuencia);
  } else {
    server.send(400, "text/plain", "No se recibió secuencia");
  }
}
void detener() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  detenerMotores();
  server.send(200, "text/plain", "Motores detenidos");
  Serial.println("Detenido");
}
void loop() {
  server.handleClient();
}
