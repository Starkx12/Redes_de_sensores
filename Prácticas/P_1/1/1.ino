//Includes
#include <arduino.h>

//External pins
int Voltage_pin = A0;

//Global variables
float Voltage = 0.0;
char buffer[50];

//Functions

//Interruptions

void setup() {
  Serial.begin(9600);

}

void loop() {

  Voltage = (3.3*analogRead(Voltage_pin))/1023;
  sprintf(buffer,"The voltage in the pin A0 is: %f",Voltage);
  Serial.println(buffer);

}
