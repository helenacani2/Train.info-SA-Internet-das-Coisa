#include <WiFi.h>
#include <PubSubClient.h>
#include <inv.h>

WiFiClient client;
PubSubClient mqtt(client);


const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;
const String topic = "brag";

const String brokerUser = "";
const String brokerPass = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("conectando no WiFi");
  while(WiFi.status()!= WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  // put your setup code here, to run once:

Serial.println("Conectado com sucesso!");
mqtt.setServer(BROKER_URL, 8883);
String ClientID = "S2-";
ClientID += String(random(0xffff),HEX);
while (mqtt.connect(ClientID.c_str()) == 0) {
Serial.print(".");
delay(200);
}
mqtt.subscribe(topic.c_str());
mqtt.setCallback(callback);
Serial.println("\nConectado ao broker!");

}

void loop() {
  // put your main code here, to run repeatedly:
  String mensagem = "";
  if(Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    Serial.println("A mensagem foi: ");
    Serial.println(mensagem);
    mensagem = "Rafa: " + mensagem;
    mqtt.publish("brag1", mensagem.c_str());


  }
    mqtt.loop();
}
void callback(char* topic, byte* payload, unsigned long length) {

String mensagemRecebida = "";
for(int i = 0; i < length; i++) {
  mensagemRecebida += (char) payload[i];
}
Serial.println(mensagemRecebida);
}


