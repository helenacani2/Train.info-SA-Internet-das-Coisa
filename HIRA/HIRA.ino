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

int status = 1;
void setup() {

  Serial.begin(115200);
  pinMode(19, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(18, OUTPUT);

  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);


  wifi_client.setInsecure();
  

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Conectando no Wifi");

  int ConectorWifi = 0;

  while((WiFi.status() != WL_CONNECTED) && (ConectorWifi != 100)) {

    status = 10;

    Serial.print(".");
    delay(200);

    ConectorWifi++;

  }

  if(ConectorWifi == 100) {

    status = 0;

    Serial.println("Erro ao conectar com o Wi-Fi");

  } else {

    Serial.println("Conectado ao Wi-Fi com sucesso");

  }



  mqtt.setServer(BROKER_URL,BROKER_PORT);

  String clientID = "HIRA";
  clientID += String(random(0xffff),HEX);

  Serial.println("Conectando ao Broker");

  int ConectorBroker = 0;

    while((mqtt.connect(clientID.c_str(),BROKER_USER_ID,BROKER_PASS_USR_PASS) == 0) && (ConectorBroker != 100)) {

      status = 20;
      Serial.print(".");
      delay(200);
      ConectorBroker++;
    }

    if(ConectorBroker == 100) {

    status = 0;

    Serial.println("Erro ao conectar com o Broker");

  } else {

    Serial.println("\nConectado ao Broker com sucesso");

  }


  mqtt.subscribe("Train.info/S3/LED");


  mqtt.setCallback(callback);



  
}



void loop() {


  //loop é onde os dados são lidos

  String mensagem = "";

  if(Serial.available() > 0) {

    mensagem = Serial.readStringUntil('\n');

    Serial.println(mensagem);

  //status = atoi(mensagem);

    mqtt.publish("TOPIC_TREM_LEDST", mensagem.c_str());

     

  }

  

  mqtt.loop();

  if (Serial.available() > 0) {

    String mensagem = Serial.readStringUntil('\n');

    Serial.println(mensagem);
    status = mensagem.toInt();
  }


  switch (status) {

    case 0:

      ledcWrite(redPin, 255);
      ledcWrite(greenPin, 0);
      ledcWrite(bluePin, 0);
      break;

    case 10:

      ledcWrite(redPin, 150);
      ledcWrite(greenPin, 2556);
      ledcWrite(bluePin, 0);
      break;

    case 20:

      ledcWrite(redPin, 150);
      ledcWrite(greenPin, 0);
      ledcWrite(bluePin, 255);
      break;

    case 1:

      ledcWrite(redPin, 0);
      ledcWrite(greenPin, 255);
      ledcWrite(bluePin, 0);
      break;

    case 2:

      ledcWrite(redPin, 0);
      ledcWrite(greenPin, 190);
      ledcWrite(bluePin, 255);
      break;

    default:

      for (byte i = 0; i < 4; i++) {

        ledcWrite(redPin, 0);
        ledcWrite(greenPin, 0);
        ledcWrite(bluePin, 255);
        delay(100);
        ledcWrite(redPin, 0);
        ledcWrite(greenPin, 0);
        ledcWrite(bluePin, 0);
        delay(100);
      }
  }
}



void callback(char* topic, byte* payload, unsigned long length) {

  //Callback é onde os dados são processaods

  String MensagemRecebida = "";

  for (int i = 0; i < length; i++) {

    MensagemRecebida += (char)payload[i];
  }



  Serial.println(MensagemRecebida);
}