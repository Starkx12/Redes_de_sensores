/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "xaxo12";
const char* password = "1a2b3c4dj";
bool date2zero = false;

// NTP variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){  
  struct tm timeinfo;
  char buffer[50];
  Serial.println(var);
  if(var == "Date"){
    if(getLocalTime(&timeinfo)){
  //    if(date2zero){
  //      timeinfo.tm_hour = 0;
  //      timeinfo.tm_min = 0;
  //      timeinfo.tm_sec = 0;      
  //      date2zero = false;
  //    }
      strftime(buffer, sizeof(buffer), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      
      return (String)buffer;
    }
  }
  return String();
}

String empty(const String& var){
  int i = 0;
  

  return String();
}
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Connect to NTP server
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false,processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });  
  
  // Reset the date
  server.on("/RESTART", HTTP_GET, [](AsyncWebServerRequest *request){        
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}
 
void loop(){
 
  
}
