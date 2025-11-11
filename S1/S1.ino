#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"
#include <DHT.h>

/*
4       / DHT
23     / Ultra_echo
22     / Ultra_trig
34     / LDR
*/

#define led 19
#define pinLDR 34      //ldr
#define pinDHT 4       // Pino digital do ESP32 para o DHT
#define DHTTYPE DHT11  // Tipo de sensor: DHT11

#define TRIGGER_PIN 22
#define ECHO_PIN 23

//no fim, não vou usar
const float LIMITAR_TEMPERATURA = 28.0;  // acima de 28.0°C é considerado "Quente"
const float LIMITAR_UMIDADE = 60.0;      // acima de 60.0% é considerado "Úmido"

DHT dht(pinDHT, DHTTYPE);

WiFiClientSecure wifi_client;
PubSubClient mqtt(wifi_client);

const String brokerUser = "";  //na SA vai ter, já que esse é publico e a SA não
const String brokerPass = "";

void setup() {
  Serial.begin(115200);

  wifi_client.setInsecure();

  pinMode(2, OUTPUT);
  dht.begin();

  pinMode(19, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.println("Conectado com sucesso!");

  mqtt.setServer(BROKER_URL, BROKER_PORT);  //isso aí cria um nome aleatório que aparece pra plaquinha, que é "S1-" + um código aleatório qualquer
  String clientID = "S1-";
  clientID += String(random(0xffff), HEX);
  Serial.println("Conectando ao broker...");
  while (mqtt.connect(clientID.c_str(), BROKER_USR_ID, BROKER_USR_PASS) == 0) {  //aqui o cod do iago
    Serial.println(".");
    delay(200);
  }
  mqtt.subscribe(TOPIC_PRESENCE_1);  //conectando a inscrição no tópico com outra placa de msm tópico
  mqtt.subscribe(TOPIC_UMID_1);
  mqtt.subscribe(TOPIC_TEMP_1);
  mqtt.subscribe(TOPIC_LUMI_1);

  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");
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


void loop() {
  int luz = map(analogRead(pinLDR), 0, 4095, 0, 100);
  if (luz > 50) {
    mqtt.publish(TOPIC_LUMI_1, "Claro");
    digitalWrite(19, LOW);
  } else {
    mqtt.publish(TOPIC_LUMI_1, "Escuro");
    digitalWrite(19, HIGH);
  }

  mqtt.loop();
  delay(500);

  float t = dht.readTemperature();
  // Converte o float 't' para uma String, depois para const char*
  String tempString = String(t, 1);
  mqtt.publish(TOPIC_TEMP_1, tempString.c_str());

  // if (t > LIMITAR_TEMPERATURA){
  // } else {
  //   mqtt.publish(TOPIC_TEMP_1,"Frio");
  // }
  //adicionar o dado com um novo tópico, para o site

  mqtt.loop();
  delay(500);

  float h = dht.readHumidity();
  // Converte o float 'h' para uma String, depois para const char*
  String umidString = String(h, 1);  // 1 dígito decimal de precisão
  mqtt.publish(TOPIC_UMID_1, umidString.c_str());

  //   if (h > LIMITAR_UMIDADE){
  // } else {
  //   mqtt.publish(TOPIC_UMID_1,"Seco");
  //adicionar o dado com um novo tópico, para o site

  mqtt.loop();
  delay(500);

  long distancia = lerDistancia();

  if (distancia < 10) {
    mqtt.publish(TOPIC_PRESENCE_1, "Presente");
  } else {
    mqtt.publish(TOPIC_PRESENCE_1, "Ausente");
  }
  delay(500);
}

/* 

Coisa errada que fiz:
int presenca = digitalRead(pinPIR);
if (presenca == HIGH) {
  mqtt.publish(TOPIC_PRESENCE_1, "Presente");
} else {
  mqtt.publish(TOPIC_PRESENCE_1, "Ausente");
}
} */

void callback(char* topic, byte* payload, unsigned long length) {  //callback para a conexão de inscrição

  /* String MensagemRecebida = "";

  for(int i = 0; i < length; i++){ //pega cada letra de payload e junta a mensagem, bit por bit (letra por letra)
    MensagemRecebida += (char) payload[i];
  }

  Serial.println(MensagemRecebida); //faz aqui o que quiser com a mensagem -> aqui vamos por o led, provavelmente

  if(MensagemRecebida == "Nome: 1"){
    digitalWrite(2, HIGH);
  }else if(MensagemRecebida == "Nome: 0"){
    digitalWrite(2, LOW);
  } */
}