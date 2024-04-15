#include <Arduino.h>
#define LED 18
int list_acc[50] ;
int idx = 0;
bool send_flag = false;
void setup() {
  pinMode(LED,OUTPUT);
  randomSeed(analogRead(0));
  Serial.begin(115200);
  delay(1000);

  xTaskCreate(Tarea1,"Tarea1",10000,NULL,0,NULL);
  xTaskCreate(Tarea2,"Tarea2",10000,NULL,1,NULL);
  xTaskCreate(Tarea3,"Tarea3",10000,NULL,1,NULL);

}

void loop() {
  delay(1000);
}

void Tarea1( void * parameter )
{
    while(true){
    int acc = random(0,100);
    list_acc[idx] = acc;
    idx = (idx+1)%10;
    delay(100);
    }
    vTaskDelete( NULL );
}

void Tarea2( void * parameter)
{

    while(true){
    
    for(int i=0;i<10;i++){
      Serial.println(list_acc[i]);
    }    
    delay(1000); 
    send_flag = true;
    }
    vTaskDelete( NULL );
}

void Tarea3( void * parameter)
{

    while(true){
    if (send_flag){
        digitalWrite(LED,HIGH);
        //Serial.println("LED ON");
        send_flag = false;
    }
    
    delay(200);     
    //Serial.println("LED OFF");
    digitalWrite(LED,LOW);
    }
    vTaskDelete( NULL );
}

