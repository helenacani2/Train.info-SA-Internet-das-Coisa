#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure wifi_client;
PubSubClient mqtt(wifi_client);

const String SSID = "";
const String PASS = "";

const String brokerUser = "";
const String brokerPass = "";

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
  while(mqtt.connect(clientID.c_str(),BROKER_USR_ID,BROKER_PASS_USR_PASS) == 0) {
    Serial.print(".");
    delay(200);
  }
  //mqtt.subscribe(topic.c_str());
  //mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");
}
String mensagem = "";
// mensagem = Serial.readStringUntil('\n');
//    Serial.print("A mensagem foi: ");
//   Serial.println(mensagem);
void loop() {
  String mensagem = "";
  if (Serial.available() > 0) {

    if (Serial.available() > 0) {
      mensagem = Serial.readStringUntil('\n');
      mqtt.publish(TOPIC_LEDS_3, mensagem.c_str());
      Serial.println(mensagem);
    }
  }
    mqtt.loop();
}
/*
void callback(char* topic, byte* payload, unsigned long length) {
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    //pega cada letra de payload e junta na mensagem
    MensagemRecebida += (char)payload[i];
  }
  //Posso fazer o que quiser com a mensagem
  Serial.println(MensagemRecebida);
}

*/