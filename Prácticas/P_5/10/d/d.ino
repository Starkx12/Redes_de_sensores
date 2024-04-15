#include "config.h"  // Archivo de configuración con credenciales de Adafruit IO
#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
//#include "AdafruitIO_esp32.h"

/************************* Configuración *******************************/

  // const char* WIFI_SSID = "xaxo12" ;
  // const char* WIFI_PASS = "1a2b3c4dj";
  // #define IO_SERVER "io.adafruit.com"
  // #define IO_SERVERPORT 1883
  // #define IO_USERNAME  "735913"
  // #define IO_KEY       "aio_NvZI65gn8HHZ3YbQKjFUb5icLIf3"


/************************* Variables de Adafruit IO *******************************/

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_KEY);
AdafruitIO_Feed *feed = io.feed("P5_10");

/************************* Funciones *******************************/

void setup() {
  Serial.begin(115200);

  // Conexión WiFi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conexión WiFi establecida");

  // Inicialización de Adafruit IO
  io.connect();

  // Asignación de función de callback para manejar mensajes del feed
  feed->onMessage(feedMessage);
}

void loop() {
  // Procesamiento de mensajes y mantenimiento de la conexión
  io.run();
}

// Función de callback para manejar mensajes entrantes del feed
void feedMessage(AdafruitIO_Data *data) {
  Serial.print("Nuevo mensaje en el feed: ");
  Serial.println(data->value());
}