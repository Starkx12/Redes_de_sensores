#include <WiFi.h>
#include <ESP32Ping.h>

const char* ssid = "xaxo12";
const char* password = "1a2b3c4dj";
const char* hostName = "google.com";
int pingResult;
void setup(){
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

void loop(){

  Serial.print("Pinging ");
  Serial.print(hostName);
  Serial.print(": ");

 bool success = Ping.ping("www.google.com", 3);
 
  if(!success){
    Serial.println("Ping failed");
    return;
  }else{
    Serial.println("Ping succesful.");
  }
 
  

  delay(5000);



}