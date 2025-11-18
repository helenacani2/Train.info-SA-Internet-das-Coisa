#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"
#include <ESP32Servo.h>

WiFiClientSecure wifi_client;
PubSubClient mqtt(wifi_client);

const byte TRIGGER_PIN = 27;
const byte ECHO_PIN = 14;

Servo servo1;
const byte SERVO1_PIN = 25;
Servo servo2;
const byte SERVO2_PIN = 24;

#define ledPin 19

void setup() {
  wifi_client.setInsecure();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso!");
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String clientID = "S3-";
  clientID += String(random(0xffff), HEX);
  Serial.println("Conectando ao broker...");
  while (mqtt.connect(clientID.c_str(), BROKER_USR_ID, BROKER_PASS_USR_PASS) == 0) {
    Serial.print(".");
    delay(200);

  }
    // SÓ OS SUBSCRIBE: duas presenças rafael, presença do andré e iluminação helena
    mqtt.subscribe(TOPIC_PRESENCE_2); //feito 
    mqtt.subscribe(TOPIC_PRESENCE_4); //feito 
    mqtt.subscribe(TOPIC_PRESENCE_3); //feito
    mqtt.subscribe(TOPIC_LUMI_1);  //tópico de iluminação da placa 1 - feito

  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");

  servo1.attach(SERVO1_PIN); //helena, lembre-se
  servo2.attach(SERVO2_PIN);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(19, OUTPUT);
}

String mensagem = "";
// mensagem = Serial.readStringUntil('\n');
//    Serial.print("A mensagem foi: ");
//   Serial.println(mensagem);

void loop() {
  long distancia = lerDistancia();
  if (distancia < 10) {
    mqtt.publish(TOPIC_PRESENCE_3, "Presente");
  } else {
    mqtt.publish(TOPIC_PRESENCE_3, "Ausente");
  }
  mqtt.loop();
  delay(500);
}

//função de ler distância:
long lerDistancia() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;

  return distancia;
}


void callback(char* topic, byte* payload, unsigned long length) {
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    //pega cada letra de payload e junta na mensagem
    MensagemRecebida += (char)payload[i];
  }

  if (strcmp(topic, TOPIC_LUMI_1) == 0 && MensagemRecebida == "Escuro") {
    digitalWrite(ledPin, HIGH);
    Serial.println("luz ");
  } else if (strcmp(topic, TOPIC_LUMI_1) == 0 && MensagemRecebida == "Claro") {
    digitalWrite(ledPin, LOW);
    Serial.println("chegou aqui");
  }
  repetir para presenca 1 2 3  servo.write(angulo);... 90 ou 120
// else if (strcmp(topic, TOPIC_LUMI_1) == 0 && MensagemRecebida == "Claro") {
    // digitalWrite(ledPin, LOW);
    // Serial.println("chegou aqui");
// }
}
