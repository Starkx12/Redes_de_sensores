#include <ArduinoJson.h>
#include <WiFi.h>
#include "ESP32_FTPClient.h"

const char* ssid = "xaxo12";
const char* password = "1a2b3c4dj";
char ftp_server[] = "155.210.150.77";
char ftp_user[]  = "rsense";
char ftp_pass[] = "rsense";
char ftp_directory[] = "/rsense/735913";
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 2);

const unsigned long interval = 10000;  // Intervalo de 10 segundos
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);  
  randomSeed(analogRead(0));
  connectToWiFi();
}

void loop() {
  unsigned long currentMillis = millis();
  if (!ftp.isConnected()){
    ftp.OpenConnection();
  }
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

  // Subir el JSON al servidor FTP
  uploadJSON(output);
}

void connectToWiFi() {
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conectado a la red WiFi");
}

void uploadJSON(String json) {
  Serial.println("Conectando al servidor FTP...");  

  if (ftp.isConnected()) {
    Serial.println("Conectado al servidor FTP");
    char fileName[32];
    sprintf(fileName,"grupoxxx_%lu.json",millis());
    ftp.InitFile("Type A");
    ftp.ChangeWorkDir(ftp_directory);
    ftp.NewFile(fileName);
    char json_char[100] ;
    json.toCharArray(json_char,100);
    ftp.Write(json_char);
    ftp.CloseFile();    
    ftp.CloseConnection();
  } else {
    Serial.println("Error al conectar al servidor FTP");
  }
}
