#include <Arduino.h>
#include <WiFiUdp.h>

#include "ESP8266WiFi.h"

#define ssid "GATITA"             // Nome da Rede sem fio que vai se conectar
#define password "tiziano1993"    // Senha da Rede sem fio que vai se conectar
#define localPort 8888  // Porta na qual o servidor ficará escutando
#define newHostname "SENSOR001"   // Nome do Host na rede

#define IP_DESTINY "192.168.5.5"
#define PACKET_SIZE 10

int sensorPin = A0;

WiFiUDP Udp;

void setup() {
  Serial.begin(115200);

  pinMode(D1, OUTPUT);  // LED verde
  pinMode(D2, OUTPUT);  // LED amarelo
  pinMode(D3, OUTPUT);  // LED vermelho

  WiFi.mode(WIFI_STA); 
  WiFi.hostname(newHostname);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  if (Udp.begin(localPort)) {
    Serial.printf("UDP server on port %d\n", localPort);
    Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
    Serial.println("");
  }
}

void loop() {
  int sensorValue = 0;
  int packetSize = 0;
  char packetTX[PACKET_SIZE];
  char packetRX[PACKET_SIZE];

  while (1) {
    digitalWrite(D1, 0x01);
    digitalWrite(D2, 0x00);
    sensorValue = analogRead(sensorPin);
    Udp.beginPacket(IP_DESTINY, 8888);
    Udp.write(itoa(sensorValue, packetTX, 10));
    Udp.endPacket();
    Serial.print(">> Pacote enviado para borda: ");
    Serial.println(packetTX);
    delay(1000);
    digitalWrite(D1, 0x00);
    digitalWrite(D2, 0x01);
    packetSize = Udp.parsePacket();
    if (packetSize > 0) {
      int n = Udp.read(packetRX, PACKET_SIZE);
      packetRX[n] = 0;
      Serial.print("<< Pacote recebido da borda: ");
      Serial.println(packetRX);
      packetSize = 0;
    } else {
      Serial.println("<> Ainda nada foi recebido! ");
    }
    delay(1000);
  }
}