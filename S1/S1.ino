#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure wifi_client;
PubSubClient mqtt(wifi_client);

const String brokerUser = ""; //na SA vai ter, já que esse é publico e a SA não
const String brokerPass = "";

void setup() {
  Serial.begin(115200);

  wifi_client.setInsecure();

    pinMode(2, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Conectando no WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }

  Serial.println("Conectado com sucesso!");

  mqtt.setServer(BROKER_URL, BROKER_PORT); //isso aí cria um nome aleatório que aparece pra plaquinha, que é "S1-" + um código aleatório qualquer
  String clientID = "S1-";
  clientID += String(random(0xffff),HEX);
  Serial.println("Conectando ao broker...");
  while (mqtt.connect (clientID.c_str () , BROKER_USR_ID, BROKER_USR_PASS) == 0 ){ //aqui o cod do iago
    Serial.println(".");
    delay(200);
  }
  mqtt.subscribe(TOPIC_PRESENCE_1); //conectando a inscrição no tópico com outra placa de msm tópico
  mqtt.subscribe(TOPIC_UMID_1); //conectando a inscrição no tópico com outra placa de msm tópico
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");
}

bool waitingForTemp = true; // Começa esperando a Temperatura

void loop() {
    String dadoLido = "";

    if(Serial.available() > 0){ // Checa se há uma nova linha
        
        dadoLido = Serial.readStringUntil('\n'); // Lê a entrada UMA ÚNICA VEZ
        dadoLido.trim(); // Remove espaços em branco ou nova linha extra

        if (waitingForTemp) {
            // Está esperando a Temperatura
            String mensagemTemp = "Helena (Temp): " + dadoLido;
            mqtt.publish(TOPIC_TEMP_1, mensagemTemp.c_str()); 
            Serial.println("Publicado em TEMP: " + mensagemTemp);
            
            // Inverte o estado para esperar o próximo dado (Umidade)
            waitingForTemp = false; 

        } else {
            // Está esperando a Umidade
            String mensagemUmidade = "Helena (Umidade): " + dadoLido;
            mqtt.publish(TOPIC_UMID_1, mensagemUmidade.c_str()); 
            Serial.println("Publicado em UMID: " + mensagemUmidade);
            
            // Inverte o estado para esperar o próximo dado (Temperatura)
            waitingForTemp = true; 
        }
    }

    mqtt.loop();
}
void callback(char * topic, byte* payload, unsigned long length) { //callback para a conexão de inscrição

  String MensagemRecebida = "";

  for(int i = 0; i < length; i++){ //pega cada letra de payload e junta a mensagem, bit por bit (letra por letra)
    MensagemRecebida += (char) payload[i];
  }

  Serial.println(MensagemRecebida); //faz aqui o que quiser com a mensagem -> aqui vamos por o led, provavelmente

  if(MensagemRecebida == "Nome: 1"){
    digitalWrite(2, HIGH);
  }else if(MensagemRecebida == "Nome: 0"){
    digitalWrite(2, LOW);
  }

}