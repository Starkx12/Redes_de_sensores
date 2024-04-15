#include <WiFi.h>
#include "time.h"

//NTP Variables and constants
const char* ssid       = "xaxo12";
const char* password   = "1a2b3c4dj";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

//TCP Variables and constants
WiFiClient localClient;
const uint port = 4240;
const char* ip = "10.112.122.248"; 

void printLocalTime()
{
  struct tm timeinfo;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  if (localClient.connect(ip, port)) {  // Establish a connection

      if (localClient.connected()) {
        localClient.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");                     
        localClient.println('\n');
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
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  
}

void loop()
{
  delay(1000);
  printLocalTime();  
}