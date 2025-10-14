#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifi_client;
PubSubClient mqtt(wifi_client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org"; //na SA vai diferente
const int brokerPort = 1883; //na SA vai diferente
const String topic = "undertale"; //topico onde vão estar as mensagens

const String brokerUser = ""; //na SA vai ter, já que esse é publico e a SA não
const String brokerPass = "";

void setup() {
  Serial.begin(115200);

    pinMode(2, OUTPUT);

  WiFi.begin(SSID, PASS);
  Serial.println("Conectando no WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }

  Serial.println("Conectado com sucesso!");

  mqtt.setServer(brokerURL.c_str(),brokerPort); //isso aí cria um nome aleatório que aparece pra plaquinha, que é "S1-" + um código aleatório qualquer
  String clientID = "S1-";
  clientID += String(random(0xffff),HEX);
  Serial.println("Conectando ao broker...");
  while(mqtt.connect(clientID.c_str()) == 0){
    Serial.println(".");
    delay(200);
  }
  mqtt.subscribe(topic.c_str()); //conectando a inscrição no tópico com outra placa de msm tópico
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");
}

void loop() {

  String mensagem = "";
  if(Serial.available() > 0){ //checagem na fila de mensagens

    mensagem = Serial.readStringUntil('\n');
    mensagem = "Helena: " + mensagem;
    mqtt.publish("deltarune", mensagem.c_str()); //aq vai o tópico, e a mensagem
    Serial.println(mensagem);

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
