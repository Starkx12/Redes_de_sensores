#include <Arduino.h>
#define LED 18
void setup() {
  pinMode(LED,OUTPUT);
  Serial.begin(115200);
  delay(1000);

  xTaskCreate(Tarea1,"Tarea1",10000,NULL,1,NULL);
  xTaskCreate(Tarea2,"Tarea2",10000,NULL,1,NULL);

}

void loop() {
  delay(1000);
}

void Tarea1( void * parameter )
{
    while(true){
    Serial.println("Hola mundo");
    delay(1000);
    }
    vTaskDelete( NULL );
}

void Tarea2( void * parameter)
{

    while(true){
    Serial.println("LED toggle");
    digitalWrite(LED,!digitalRead(LED)) ;
    delay(200); 
    
    }
    vTaskDelete( NULL );
}
