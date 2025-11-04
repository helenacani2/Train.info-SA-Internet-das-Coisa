#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure wifi_client;

PubSubClient mqtt(wifi_client);

const String brokerUser = "";
const String brokerPass = "";

const byte redPin = 19;
const byte greenPin = 21;
const byte bluePin = 18;

void setup() {

  Serial.begin(115200);
  pinMode(19,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(18,OUTPUT);

  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);

  wifi_client.setInsecure();
  


  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Conectando no Wifi");

  while(WiFi.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(200);

  }

  Serial.println("Conectado com sucesso");

  mqtt.setServer(BROKER_URL,BROKER_PORT);
  //mqtt.setServer(BROKER_URL.c_str(),BROKER_PORT);

  String clientID = "HIRA";
  clientID += String(random(0xffff),HEX);

  Serial.println("Conectando ao Broker");

    while(mqtt.connect(clientID.c_str(),BROKER_USER_ID,BROKER_PASS_USR_PASS) == 0) {
      Serial.print(".");
      delay(200);
    }

  //mqtt.subscribe("Train.info/S1/T");
  mqtt.subscribe("Train.info/S3/LED");
  //mqtt.subscribe(TOPIC_TREM_LEDST.c_str());

  mqtt.setCallback(callback);

  Serial.println("\nConectado ao Broker");

}



void loop() {

  
  //loop é onde os dados são lidos

  String mensagem = "";

  if(Serial.available() > 0) {

    mensagem = Serial.readStringUntil('\n');

    Serial.println(mensagem);

    //mqtt.publish("TOPICO_PLACEHOLDER", mensagem.c_str());

    mqtt.publish("TOPIC_TREM_LEDST", mensagem.c_str());

  }

  mqtt.loop();

}



void callback(char* topic, byte* payload, unsigned long length) {

  //Callback é onde os dados são processaods

  String MensagemRecebida = "";

  for(int i = 0; i < length; i++) {

    MensagemRecebida += (char) payload[i];

  }

  Serial.println(MensagemRecebida);

  if(MensagemRecebida == "1") {

    ledcWrite(redPin, 255);
    ledcWrite(greenPin, 0);
    ledcWrite(bluePin, 0);

  }

  if(MensagemRecebida == "0") {

    ledcWrite(redPin, 0);
    ledcWrite(greenPin, 0);
    ledcWrite(bluePin, 0);

  }

}