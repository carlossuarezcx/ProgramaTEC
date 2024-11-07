#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENA D5
#define ENB D6

int velocidad = 100;
int tiempo = 1000;

const char* ssid = "PC";
const char* password = "68+1O2h8";

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
    }
}
void adelante() {
  Serial.println("  Adelante");
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
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void izquierda() {
  Serial.println("  Izquierda");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void derecha() {
  Serial.println("  Derecha");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(tiempo);
  Serial.println("  Detener");
  detenerMotores();
}

void detenerMotores() {
  Serial.println("  DETENIDO ");
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
  Serial.println("Conectado a WiFi");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/play", HTTP_POST, iniciar);
  server.on("/stop", HTTP_POST, detener);
  server.on("/velocidad", HTTP_POST, setVelocidad);
  server.on("/tiempo", HTTP_POST, setTiempo);

  server.begin();

  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
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
      server.send(200, "text/plain", "Nueva velocidad procesada" + velocidad);
    }
  } else {
    server.send(400, "text/plain", "No se recibió velocidad");
  }
}
void setTiempo(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("plain")) {
    int nuevoTiempo = server.arg("plain").toInt();
    if(nuevoTiempo >=0 && nuevoTiempo <= 10){
      tiempo = nuevoTiempo * 1000;
      Serial.println("Nuevo tiempo: " + String(tiempo));
      server.send(200, "text/plain", "Nueva tiempo procesado" + tiempo);
    }
  } else {
    server.send(400, "text/plain", "No se recibió velocidad");
  }
}
void loop() {
  server.handleClient();
}
