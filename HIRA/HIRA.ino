#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifi_client;

PubSubClient mqtt(wifi_client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;
const String topic = "deltarune";

const String brokerUser = "";
const String brokerPass = "";

void setup() {

  Serial.begin(115200);
  pinMode(2,OUTPUT);
  WiFi.begin(SSID, PASS);

  Serial.println("Conectando no Wifi");

  while(WiFi.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(200);

  }

  Serial.println("Conectado com sucesso");

  mqtt.setServer(brokerURL.c_str(),brokerPort);

  String clientID = "HIRA";
  clientID += String(random(0xffff),HEX);

  Serial.println("Conectando ao Broker");

  while(mqtt.connect(clientID.c_str()) == 0) {

    Serial.print(".");
    delay(200);

  }

  mqtt.subscribe(topic.c_str());

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



void callback(char* topic, byte* payload, unsigned long length) {

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