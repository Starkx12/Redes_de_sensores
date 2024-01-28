//Includes
#include <arduino.h>
#include "BBTimer.hpp"

//External pins
int Voltage_pin = A0;

//Global variables
float Voltage = 0.0;
char buffer[50];
bool send_flag = false;

//Functions
void t1Callback()
{
  send_flag = true; // ADC interruption flag
  
}
//Interruptions
BBTimer my_t1(BB_TIMER1);

void setup() {

  Serial.begin(9600);
  my_t1.setupTimer(10000000, t1Callback);
  my_t1.timerStart();

}

void loop() {
  
  if (send_flag == true){
    
    Voltage = (3.3*analogRead(Voltage_pin))/1023;    
    sprintf(buffer,"The voltage in the pin A0 is: %f",Voltage);
    Serial.println(buffer);
    send_flag = false;

  }

}
