//Includes
#include <arduino.h>
#include "mbed.h"

//External pins
int Voltage_pin = A0;
int PWM_pin = D2;

//Global variables
float PWM_duty = 0.0; //The values are from 0 to 1
int PWM_freq = 5000; //The valueas are in Hz
float Voltage = 0.0;
char buffer[50];

//Functions

//Interruptions
mbed::PwmOut pwmPin(digitalPinToPinName(PWM_pin));


void setup() {

  Serial.begin(9600);
  pwmPin.period(1.0/PWM_freq);
  pwmPin.write(PWM_duty);

}

void loop() {
  
  Voltage = (3.3*analogRead(Voltage_pin))/1023;
  PWM_duty = Voltage/3.3;
  pwmPin.write(PWM_duty);
  sprintf(buffer,"The voltage in the pin A0 is: %f",Voltage);
  Serial.println(buffer);

}
