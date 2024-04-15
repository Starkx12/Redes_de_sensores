#include <WiFi.h>
#include "time.h"
#include "ESP32TimerInterrupt.h"

//NTP Variables and constants
const char* ssid       = "xaxo12";
const char* password   = "1a2b3c4dj";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

//TCP Variables and constants
WiFiClient localClient;
const uint port = 4240;
const char* ip = "10.89.155.116"; 
bool send_flag = false;
bool send_enable = false;
uint cnt_array = 0;
char word_received[10]={};
//Interruptions
#define TIMER_INTERVAL_MS 1000
ESP32Timer ITimer(0);
bool IRAM_ATTR TimerHandler(void*) {
  send_flag = true;
  return true;
}


void printLocalTime()
{
  struct tm timeinfo;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  if (localClient.connect(ip, port)) {  // Establish a connection

      if (localClient.connected()) {
        if(send_enable){
        localClient.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");                     
        localClient.println('\n');
      }    }
 
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

  ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler);

}

void loop()
{
  if (send_flag){
    printLocalTime();     
    send_flag = false;
  }

  if(localClient.connected()) {      
      if (localClient.available() > 0) {
        String c =  localClient.readStringUntil('\n');
        Serial.print(c);  
        if (c == "start"){ send_enable = true;}
        if (c == "stop"){ send_enable = false;}
        }
      
    }

  
}