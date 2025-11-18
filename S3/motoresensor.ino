#include <ESP32Servo.h>

Servo meuServo;
const byte SERVO_PIN = 25;
//26 tambem

void setup() {
  Serial.begin(115200);
  meuServo.attach(SERVO_PIN);
}

void loop() {
  // Varre de 0 a 180 graus
  for (int pos = 0; pos <= 180; pos += 1) {
    meuServo.write(pos);
    delay(15);
  }
  
  // Varre de 180 a 0 graus
  for (int pos = 180; pos >= 0; pos -= 1) {
    meuServo.write(pos);
    delay(15);
  }
}

--------------------------------------------------------------------------------------

const byte TRIGGER_PIN = 27;
const byte ECHO_PIN = 14;

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

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
  long distancia = lerDistancia();
  Serial.print(distancia);
  delay (1);
  
  if (distancia < 10) {
    Serial.println("Objeto próximo!");
  }
  
  delay(500);
}