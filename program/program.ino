#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//L298N
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENA D5
#define ENB D6

//74HC595
#define DATA_PIN D7
#define CLOCK_PIN D0
#define LATCH_PIN D8

// Estado de LEDs (bits para cada LED)
const uint8_t LUZ_AMARILLA = 0b00000011;          // Bits 0 y 1 
const uint8_t LUZ_REVERSA = 0b00001100;           // Bits 2 y 3
const uint8_t LUZ_DIRECCIONAL_DER = 0b00010000;   // Bit 4
const uint8_t LUZ_DIRECCIONAL_IZQ = 0b00100000;   // Bit 5
const uint8_t LUZ_FRENO = 0b11000000;             // Bits 6 y 7 
const uint8_t LUCES_APAGADAS = 0b00000000;

int velocidad = 255;
int tiempo = 360;
int tiempoDetenido = 500;

// Conexión WIFI
const char* ssid = "PC";
const char* password = "68+1O2h8";

ESP8266WebServer server(80);

void procesarLuces(uint8_t data) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);
    digitalWrite(LATCH_PIN, HIGH);
}

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
    }
}
void adelante() {
  Serial.println("  Adelante");
  procesarLuces(LUZ_AMARILLA);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void atras() {
  Serial.println("  Atrás");
  procesarLuces(LUZ_REVERSA);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void izquierda() {
  Serial.println("  izquierda");
  procesarLuces(LUZ_DIRECCIONAL_IZQ);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void derecha() {
  Serial.println("  Derecha");
  procesarLuces(LUZ_DIRECCIONAL_DER);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void detenerMotores() {
  Serial.println("  DETENIDO ");
  procesarLuces(LUZ_FRENO);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(tiempoDetenido);
  procesarLuces(LUCES_APAGADAS);
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
  Serial.println("Conectado a WiFi");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/play", HTTP_POST, iniciar);
  server.on("/stop", HTTP_POST, detener);
  server.on("/velocidad", HTTP_POST, setVelocidad);
  server.on("/tiempo", HTTP_POST, setTiempo);
  server.on("/configuraciones", HTTP_GET, getConfiguraciones);

 // Pines del 74HC595
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  server.begin();

  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);

  procesarLuces(LUCES_APAGADAS);
  procesarLuces(LUZ_AMARILLA);
}

void iniciar() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    String secuencia = server.arg("plain");
    server.send(200, "text/plain", "Secuencia procesada");
    Serial.println("Nueva secuencia:" + secuencia);
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
void setVelocidad(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    int nuevaVelocidad = server.arg("plain").toInt();
    if(nuevaVelocidad >=0 && nuevaVelocidad <= 255){
      velocidad = nuevaVelocidad;
      analogWrite(ENA, velocidad);
      analogWrite(ENB, velocidad);
      Serial.println("Nueva velocidad: " + String(velocidad));
      server.send(200, "text/plain", "Nueva velocidad procesada" + String(velocidad));
    }
  } else {
    server.send(400, "text/plain", "No se recibió velocidad");
  }
}
void setTiempo(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    int nuevoTiempo = server.arg("plain").toInt();
    if(nuevoTiempo >=0){
      tiempo = nuevoTiempo;
      Serial.println("Nuevo tiempo: " + String(tiempo));
      server.send(200, "text/plain", "Nueva tiempo procesado" + String(tiempo));
    }
  } else {
    server.send(400, "text/plain", "No se recibió velocidad");
  }
}
void getConfiguraciones() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String response = "{ \"velocidad\": " + String(velocidad) + ", \"tiempo\": " + String(tiempo) + " }";
  server.send(200, "application/json", response);
}
void loop() {
  server.handleClient();
}
