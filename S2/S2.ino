#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

//Sensores que tem que publicar são
//"presença 2" "presença 4"

//TOPIC_PRESENCE_2 //esse tem q ter
//TOPIC_PRESENCE_4 //esse tem q ter
//TOPIC_LUMI_1     //esse tem q ter

//Atuadores para ser utilizado são
//LED iluminação

//Pino dos sensores
#define TRIGGER_PIN_1 22
#define ECHO_PIN_1 23

#define TRIGGER_PIN_2 18
#define ECHO_PIN_2 19

//Pino do led
#define ledPin 12

WiFiClientSecure wifi_client;
PubSubClient mqtt(wifi_client);

void setup() {
  Serial.begin(115200);
  wifi_client.setInsecure();

  pinMode(ledPin, OUTPUT);

  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);

  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.println("Conectado com sucesso!");

  mqtt.setServer(BROKER_URL, BROKER_PORT);  //isso aí cria um nome aleatório que aparece pra plaquinha, que é "S2-" + um código aleatório qualquer
  String clientID = "S2-";
  clientID += String(random(0xffff), HEX);
  Serial.println("Conectando ao broker...");
  while (mqtt.connect(clientID.c_str(), BROKER_USR_ID, BROKER_USR_PASS) == 0) {
    Serial.println(".");
    delay(200);
  }

  //conectando a inscrição no tópico com outra placa de msm tópico

  mqtt.subscribe(TOPIC_PRESENCE_2);  //p1
  mqtt.subscribe(TOPIC_PRESENCE_4);  //p2
  mqtt.subscribe(TOPIC_LUMI_1);      //tópico de iluminação da placa 1

  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");
}

//funções para ler a distância e configurar a distância

long lerDistancia1() {
  digitalWrite(TRIGGER_PIN_1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN_1, LOW);
  long duracao = pulseIn(ECHO_PIN_1, HIGH);
  long distancia1 = duracao * 349.24 / 2 / 10000;
  return distancia1;
}

long lerDistancia2() {
  digitalWrite(TRIGGER_PIN_2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN_2, LOW);
  long duracao = pulseIn(ECHO_PIN_2, HIGH);
  long distancia2 = duracao * 349.24 / 2 / 10000;
  return distancia2;
}

//verifica se o trem está presente ou ausente
void loop() {
  long distancia1 = lerDistancia1();
  long distancia2 = lerDistancia2();

  if (distancia1 < 10) {
    mqtt.publish(TOPIC_PRESENCE_2, "Presente");
  } else {
    mqtt.publish(TOPIC_PRESENCE_2, "Ausente");
  }

  if (distancia2 < 10) {
    mqtt.publish(TOPIC_PRESENCE_4, "Presente");
  } else {
    mqtt.publish(TOPIC_PRESENCE_4, "Ausente");
  }
  mqtt.loop();
  delay(500);
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
}