#include <WiFi.h>
#include <PubSubClient.h>  //Conecta as bibliotecas
#include <WiFiClientSecure.h>
#include "env.h"  //Conecta o env

WiFiClientSecure wifi_client;

PubSubClient mqtt(wifi_client);

const String brokerUser = "";
const String brokerPass = "";

const byte redPin = 19;
const byte greenPin = 21;
const byte bluePin = 18;

const byte RedPinVel = 23;
const byte GreenPinVel = 22;

int status = 1;
void setup() {

  Serial.begin(115200);
  pinMode(19, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(18, OUTPUT);

  pinMode(RedPinVel, OUTPUT);
  pinMode(GreenPinVel, OUTPUT);

  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);


  wifi_client.setInsecure();


  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Conectando no Wifi");

  int ConectorWifi = 0;

  while ((WiFi.status() != WL_CONNECTED) && (ConectorWifi != 100)) {

    ledcWrite(redPin, 150);
    ledcWrite(greenPin, 256);
    ledcWrite(bluePin, 0);

    Serial.print(".");
    delay(200);

    ConectorWifi++;
  }

  if (ConectorWifi == 100) {

    ledcWrite(redPin, 255);
    ledcWrite(greenPin, 0);
    ledcWrite(bluePin, 0);

    Serial.println("Erro ao conectar com o Wi-Fi");

  } else {

    Serial.println("Conectado ao Wi-Fi com sucesso");
  }



  mqtt.setServer(BROKER_URL, BROKER_PORT);

  String clientID = "HIRA";
  clientID += String(random(0xffff), HEX);

  Serial.println("Conectando ao Broker");

  int ConectorBroker = 0;

  while ((mqtt.connect(clientID.c_str(), BROKER_USER_ID, BROKER_PASS_USR_PASS) == 0) && (ConectorBroker != 100)) {

    ledcWrite(redPin, 150);
    ledcWrite(greenPin, 0);
    ledcWrite(bluePin, 255);

    Serial.print(".");
    delay(200);
    ConectorBroker++;
  }

  if (ConectorBroker == 100) {

    ledcWrite(redPin, 255);
    ledcWrite(greenPin, 0);
    ledcWrite(bluePin, 0);

    Serial.println("Erro ao conectar com o Broker");

  } else {

    Serial.println("\nConectado ao Broker com sucesso");
  }


  mqtt.subscribe(TOPIC_TREM_VELOCIDADE);


  mqtt.setCallback(callback);
}



void loop() {


  //loop é onde os dados são lidos

  String mensagem = "";

  if (Serial.available() > 0) {

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
      ledcWrite(greenPin, 256);
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

      //for (byte i = 0; i < 4; i++) {

      if ((status != 0) && (status != 1) && (status != 2) && (status != 10) && (status != 20)) {

        ledcWrite(redPin, 0);
        ledcWrite(greenPin, 0);
        ledcWrite(bluePin, 255);
        delay(500);
        ledcWrite(redPin, 0);
        ledcWrite(greenPin, 0);
        ledcWrite(bluePin, 0);
        delay(500);
      }
  }
}



void callback(char* topic, byte* payload, unsigned long length) {

  //Callback é onde os dados são processaods

  String MensagemRecebida = "";

  for (int i = 0; i < length; i++) {

    MensagemRecebida += (char)payload[i];

  }

  int VelocidadeTrem = MensagemRecebida.toInt();

  if ((strcmp(topic, TOPIC_TREM_VELOCIDADE)) == 0 && VelocidadeTrem >= 0) {

    MensagemRecebida = "Velocidade positiva, igual a: " + MensagemRecebida + "km/h";

    digitalWrite(RedPinVel, false);
    digitalWrite(GreenPinVel, true);

  } else {

    MensagemRecebida = "Velocidade negativa, igual a: " + MensagemRecebida + "km/h";

    digitalWrite(RedPinVel, true);
    digitalWrite(GreenPinVel, false);

  }


  Serial.println(MensagemRecebida);

}