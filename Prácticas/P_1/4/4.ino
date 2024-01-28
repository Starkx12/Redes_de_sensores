//Includes
#include <arduino.h>
#include "mbed.h"
#include "BBTimer.hpp"
//External pins
int Voltage_pin = A0;
int PWM_pin = D2;

//Global variables
String read_value;
String value_short;
float Voltage = 0.0;
char buffer[50];
String timer_freq_str ;
int  timer_freq = 0;
String duty_str;
float PWM_duty = 0.0; //The values are from 0 to 1
int PWM_freq = 5000; //The valueas are in Hz
bool send_flag = false;

//Functions
void t1Callback()
{
  send_flag = true; // Flag de lectura del ADC
  
}

//Interruptions
BBTimer my_t1(BB_TIMER1);
mbed::PwmOut pwmPin(digitalPinToPinName(PWM_pin));

void setup() {
  
  Serial.begin(9600);

}

void loop() {
  
    if(Serial.available() != 0){

    read_value = Serial.readStringUntil('\n');
    value_short = read_value.substring(0,4);

    if (value_short.equals("ADC")){

      my_t1.timerStop();
      Voltage = (3.3*analogRead(Voltage_pin))/1023;
      sprintf(buffer,"The actual voltage in the pin A0 is: %f",Voltage);
      Serial.println(buffer);

    }else if(value_short.equals("ADC(")){

      int pos_1 = read_value.indexOf("(");
      int pos_2 = read_value.indexOf(")");
      timer_freq_str =  read_value.substring(pos_1+1,pos_2);      
      timer_freq = timer_freq_str.toInt();
      if(timer_freq<=0){
        
        my_t1.timerStop(); 
        Serial.println("ADC read stoped");

      }else{

        Serial.println("El valor de frecuencia es: " + timer_freq_str);
        my_t1.setupTimer(timer_freq*1000000, t1Callback);
        my_t1.timerStart();

      }
    }else if(value_short.equals("PWM(")){

      int pos_1 = read_value.indexOf("(");
      int pos_2 = read_value.indexOf(")");
      duty_str =  read_value.substring(pos_1+1,pos_2);      
      
      PWM_duty = timer_freq_str.toInt();

      if (PWM_duty > 10){ PWM_duty = 1;}
      else if (PWM_duty < 0){PWM_duty = 0;}
      else {PWM_duty = PWM_duty/10;}

      Serial.println("El valor de Duty es: " + duty_str);

      pwmPin.period(1.0/PWM_freq);
      pwmPin.write(PWM_duty);

    }else if(read_value.equals("STOP")){

      pwmPin.write(0);
      my_t1.timerStop();
      Serial.println("ALL the perrpherical were stoped");

    }else{
      Serial.println("Not valid command");
    }
  }

  if (send_flag == true){

    Voltage = (3.3*analogRead(Voltage_pin))/1023;    
    sprintf(buffer,"The voltage in the pin A0 is: %f",Voltage);
    Serial.println(buffer);
    send_flag = false;

  }
}
