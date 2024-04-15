#include <ArduinoJson.h>
const unsigned long interval = 10000;  // Intervalo de 10 segundos
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);  
  randomSeed(analogRead(0));
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    generateAndSendData();
  }
}
  
void generateAndSendData() {
  // Construir el objeto JSON en formato SenML
  StaticJsonDocument<200> doc;
  doc["bv"] = random(0,100); // temperatura
  doc["bu"] = "degC"; // unidades de temperatura
  doc["bt"] = millis() / 1000; // tiempo en segundos desde el inicio del programa

  String output;
  serializeJson(doc, output);
  Serial.println("JSON generado:");
  Serial.println(output);  
}
