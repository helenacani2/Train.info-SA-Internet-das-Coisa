#include <WiFi.h>
WiFiClient client;
const String SSID = "nome da rede";
const String PASS = "senha da rede";


void setup() {
  Serial.begin(115200);
client.begin(SSID, PASS);
Serial.println("Conectando no WiFi");
while(client.status()!=WL_CONNECTED) {
Serial.print(".");
delay(200);

}

}

void loop() {
  // put your main code here, to run repeatedly:

}
