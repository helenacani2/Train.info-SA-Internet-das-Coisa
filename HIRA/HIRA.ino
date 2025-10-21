#include <WiFi.h>
#include <PubSubClient.h>
#include "env.h"

WiFiClient wifi_client;

PubSubClient mqtt(wifi_client);

const String brokerUser = "";
const String brokerPass = "";

void setup() {

  Serial.begin(115200);
  pinMode(2,OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Conectando no Wifi");

  while(WiFi.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(200);

  }

  Serial.println("Conectado com sucesso");

  mqtt.setServer(BROKER_URL.c_str(),BROKER_PORT);

  String clientID = "HIRA";
  clientID += String(random(0xffff),HEX);

  Serial.println("Conectando ao Broker");

  while(mqtt.connect(clientID.c_str()) == 0) {

    Serial.print(".");
    delay(200);

  }

  mqtt.subscribe(TOPIC_TREM.c_str());

  mqtt.setCallback(callback);

  Serial.println("\nConectado ao Broker");

}



void loop() {

  String mensagem = "";

  if(Serial.available() > 0) {

    mensagem = Serial.readStringUntil('\n');

    mensagem = "Nome: " + mensagem;

    Serial.println(mensagem);

    mqtt.publish("undertale", mensagem.c_str());

  }

  mqtt.loop();

}



void callback(char* TOPIC_TREM, byte* payload, unsigned long length) {

  String MensagemRecebida = "";

  for(int i = 0; i < length; i++) {

    MensagemRecebida += (char) payload[i];

  }

  Serial.println(MensagemRecebida);

  if(MensagemRecebida == "Helena: Acenda") {

    digitalWrite(2, true);

  }

  if(MensagemRecebida == "Helena: Apague") {

    digitalWrite(2, false);

  }

}