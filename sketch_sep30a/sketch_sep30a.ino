#include <WiFi.h>
WiFiClient client;
const String SSID = "nome da rede";
const String PASS = "senha da rede";

void setup() {
  Serial.begin(115200);
  client.begin(SSID, PASS);
  serial.println("conectando no WiFi");
  while(client.satus()!= WL_CONNECTED) {
    serial.print(".");
    delay(200);
  }
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
