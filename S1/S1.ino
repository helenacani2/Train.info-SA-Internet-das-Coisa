#include <WiFi.h> //biblioteca para gerenciar a conexão Wi-Fi do ESP32
#include <PubSubClient.h> //biblioteca para o protocolo MQTT (envio das mensagens e os tópicos)
#include <WiFiClientSecure.h> //cliente Wi-Fi seguro, essencial para MQTT sobre SSL/TLS
#include "env.h" //o arquivo local dos tópicos que a placa envia e se inscreve
#include <DHT.h> //biblioteca para o sensor de temperatura e umidade

/*

// mapeamento dos pinos do que serão usados do ESP32 para os sensores e atuadores
4       / DHT
23     / Ultra_echo
22     / Ultra_trig
34     / LDR
*/

#define ledPin 19      //pino digital do led
#define pinLDR 34      //ldr
#define pinDHT 4       // pino digital do ESP32 para o DHT
#define DHTTYPE DHT11  // tipo de sensor: DHT11

#define TRIGGER_PIN 22
#define ECHO_PIN 23

//constantes de limite (não utilizadas, mas mntidas para contexto)
const float LIMITAR_TEMPERATURA = 28.0;  // acima de 28.0°C é considerado "Quente"
const float LIMITAR_UMIDADE = 60.0;      // acima de 60.0% é considerado "Úmido"

DHT dht(pinDHT, DHTTYPE); // criação do objeto DHT, passando o pino e o tipo de sensor

WiFiClientSecure wifi_client; 
PubSubClient mqtt(wifi_client); 

void setup() {
  //inicializa a comunicação serial para debug e monitoramento
  Serial.begin(115200);

  wifi_client.setInsecure();

  //configuração de pinos:
  dht.begin(); //inicialização do sensor dht

  pinMode(19, OUTPUT); //led

  pinMode(TRIGGER_PIN, OUTPUT); //sensor ultrassônico
  pinMode(ECHO_PIN, INPUT);

  //conexão Wi-Fi (validação e lógica)

  // inicia a conexão Wi-Fi usando credenciais do em "env.h"
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Conectando no WiFi");

  // espera até a conexão ser estabelecida, imprimindo um ponto a cada 200ms
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }

  Serial.println("Conectado com sucesso!"); 

  mqtt.setServer(BROKER_URL, BROKER_PORT);  //isso aí cria um nome aleatório que aparece pra plaquinha, que é "S1-" + um código aleatório qualquer
  String clientID = "S1-";
  clientID += String(random(0xffff), HEX);
  Serial.println("Conectando ao broker...");
  // tenta conectar ao broker com o ID único e as credenciais do usuário/senha
  // o loop continua tentando até a conexão ser bem-sucedida
  while (mqtt.connect(clientID.c_str(), BROKER_USR_ID, BROKER_USR_PASS) == 0) {  
    Serial.println(".");
    delay(200);
  }

  // se a conexão for bem-sucedida, subscreve aos tópicos que a placa precisa ouvir
  // (receber mensagens de outras placas)
  mqtt.subscribe(TOPIC_PRESENCE_1);  //conectando a inscrição no tópico com outra placa de msm tópico
  mqtt.subscribe(TOPIC_UMID_1);
  mqtt.subscribe(TOPIC_TEMP_1);
  mqtt.subscribe(TOPIC_LUMI_1);

  // define a função que será chamada sempre que uma mensagem for recebida em um tópico subscrito
  mqtt.setCallback(callback);
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker!");
}

//função de ler distância (em cm):
long lerDistancia() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH);
  long distancia = duracao * 349.24 / 2 / 10000;

  return distancia;
}

void loop() {
  int luz = map(analogRead(pinLDR), 0, 4095, 0, 100);
  Serial.println(luz);
  if (luz > 95) {
    mqtt.publish(TOPIC_LUMI_1, "Escuro");
    // digitalWrite(19, LOW);
  } else {
    mqtt.publish(TOPIC_LUMI_1, "Claro");
    // digitalWrite(19, HIGH);
  }


  float t = dht.readTemperature(); // leitura da temperatura em celsius

  // converte o float 't' para uma String, depois para const char*
  String tempString = String(t, 1); // 1 dígito decimal de precisão
  mqtt.publish(TOPIC_TEMP_1, tempString.c_str());// valor da temperatura lida pelo sensor

  // if (t > LIMITAR_TEMPERATURA){
  // } else {
  //   mqtt.publish(TOPIC_TEMP_1,"Frio");
  // }
  //adicionar o dado com um novo tópico, para o site


  float h = dht.readHumidity(); // leitura da umidade relativa

  // Converte o float 'h' para uma String, depois para const char*
  String umidString = String(h, 1);  // 1 dígito decimal de precisão
  mqtt.publish(TOPIC_UMID_1, umidString.c_str()); // valor da umidade lida pelo sensor

  //   if (h > LIMITAR_UMIDADE){
  // } else {
  //   mqtt.publish(TOPIC_UMID_1,"Seco");
  //adicionar o dado com um novo tópico, para o site


  long distancia = lerDistancia(); // chama a função para obter a distância em cm

  if (distancia < 10) {
    mqtt.publish(TOPIC_PRESENCE_1, "Presente"); // se algo estiver a menos de 10 cm (Considerado "Presente")
  } else {
    mqtt.publish(TOPIC_PRESENCE_1, "Ausente"); // se não houver nada próximo (Considerado "Ausente")
  }
 
  mqtt.loop();
  delay(500);
}

void callback(char* topic, byte* payload, unsigned long length) {  //callback para a conexão de inscrição

  String MensagemRecebida = "";

  for (int i = 0; i < length; i++) {  //pega cada letra de payload e junta a mensagem, bit por bit (letra por letra) - string legível
    MensagemRecebida += (char)payload[i];
  }

  Serial.println(MensagemRecebida);  //faz aqui o que quiser com a mensagem -> aqui vamos por o led, provavelmente

  // verifica se o tópico recebido é o TOPIC_LUMI_1 E se a mensagem é "Escuro", ligando a luz
  if (strcmp(topic, TOPIC_LUMI_1) == 0 && MensagemRecebida == "Escuro") {
    digitalWrite(ledPin, HIGH);
    Serial.println("luz ");

    // verifica se o tópico recebido é o TOPIC_LUMI_1 E se a mensagem é "Claro", apagando a luz
  } else if (strcmp(topic, TOPIC_LUMI_1) == 0 && MensagemRecebida == "Claro") {
    digitalWrite(ledPin, LOW);
    Serial.println("chegou aqui");
  }
}