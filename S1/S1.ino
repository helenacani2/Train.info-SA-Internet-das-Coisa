#include <WiFi.h>
WiFiClient client;
const String SSID = "Nome da rede";
const String PASS = "Senha da rede";

void setup() {
  Serial.begin(115200);
  client.begin(SSID, PASS);
  Serial.println("Conectando no WiFi");
  while(client.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }

}

void loop() {

}
