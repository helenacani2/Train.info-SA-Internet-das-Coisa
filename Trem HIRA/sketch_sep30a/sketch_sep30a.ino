#include <WiFi.h>
WifFiClient client;

const String SSID = "nome da rede";
const Strinf PASS = "senha da rede";



void setup() {

  Serial.begin(115200);

  client.begin(SSID, PASS);

  Serial.println("Conectando no Wifi");

  while(client.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(200);

  }

}

void loop() {



}
