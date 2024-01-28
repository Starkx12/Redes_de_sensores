//Includes
#include <Wire.h>
#include <arduino.h>

//External pins
int LEDPin = D2;

//Global variables

//Functions
int readSerial(char resultado[]){
  int i = 0;
  while(1){
    while(Serial.available() > 0){
      char inChar = Serial.read();
      if (inChar == '\n') {
        resultado[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        resultado[i] = inChar;
        i++;
      }
    }
  }
}

//Interruptions
void receiveEvent(int howMany){
  char lectura = Wire.read(); 
  if(lectura == '0'){
    digitalWrite(LEDPin, LOW);
  }
  if(lectura == '1'){
    digitalWrite(LEDPin, HIGH);
  }
}

//Control
//#define writter
#define reader

void setup() {

  Serial.begin(9600);
  while(!Serial);

  #ifdef writter    
    Wire.begin();
  #endif

  #ifdef reader
    Wire.begin(8);
    Wire.onReceive(receiveEvent);
  #endif

}

void loop() {
  
  #ifdef writter

    Serial.println("Enter 0 to turn off led, enter 1 to turn it on ");
    char ledVal[0];
    readSerial(ledVal);
    Wire.beginTransmission(8);
    Wire.write(ledVal);
    Wire.endTransmission();
    delay(100);

  #endif

}
