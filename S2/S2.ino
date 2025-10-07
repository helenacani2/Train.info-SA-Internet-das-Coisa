#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

const String SSID = "FIESC_IOT_EDU"; // nome da rede
const String PASS = "8120gv08"; // senha da rede

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;

const String brokerUser = "";
const String brokerPass = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  Serial.println("conectando no WiFi");
  while(WiFi.status()!= WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  // put your setup code here, to run once:

Serial.println("Conectado com sucesso!");
mqtt.setServer(brokerURL.c_str(),brokerPort);
String ClientID = "S2-";
ClientID += String(random(0xffff),HEX);
while (mqtt.connect(ClientID.c_str()) == 0) {
Serial.print(".");
delay(200);
}
Serial.println("\nConectado ao broker!");

}

void loop() {
  // put your main code here, to run repeatedly:

}
