#include <WiFi.h>
#include "time.h"

//NTP Variables and constants
const char* ssid       = "xaxo12";
const char* password   = "1a2b3c4dj";


//TCP Variables and constants
WiFiClient localClient;
const uint port = 4240;
const char* ip = "192.168.113.119"; 

void Send_Data()
{  
  if (localClient.connect(ip, port)) {  // Establish a connection

      if (localClient.connected()) {
        int temp = random(0,100);
        
        localClient.printf("%d\n",temp);                     
        
        Serial.println(temp);
      }    
 
  }
}



void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");  
  randomSeed(analogRead(0));
}

void loop()
{
  delay(1000);
  Send_Data(); 
}