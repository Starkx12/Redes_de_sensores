//Includes
#include <Wire.h>
#include <arduino.h>
#include "BBTimer.hpp"
#include <Arduino_LSM9DS1.h>

//External pins
int LEDPin = D2;

//Global variables
bool send_flag = false;
float x_axes = 0.0;
float y_axes = 0.0;
float z_axes = 0.0;
char i2cSend[8];


void t1Callback()
{
  send_flag = true; // ADC interruption flag
  
}
//Interruptions
BBTimer my_t1(BB_TIMER1);




void setup() {

  Serial.begin(9600);
  while(!Serial);

  if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1);
  }   

  Wire.begin();
  
  my_t1.setupTimer(1000000, t1Callback);
  my_t1.timerStart();
  

}

void loop() { 
 

    if(send_flag == true){

      send_flag = false;      
      if(IMU.accelerationAvailable()){
        
        IMU.readAcceleration(x_axes, y_axes, z_axes); 
        x_axes= x_axes*100;
        y_axes = y_axes*100;
        z_axes = z_axes*100;
        
      }else{

        x_axes = -1;
        y_axes = -1;
        z_axes = -1;

      }

      
        Wire.beginTransmission(8);

        sprintf(i2cSend, "%.4f", x_axes);  

        Wire.write("/");
        Serial.print("/");

        Wire.write(i2cSend); 
        Serial.print(i2cSend);

        Wire.write("/");        
        Serial.print("/");

        sprintf(i2cSend, "%.4f", y_axes); 
        
        Wire.write(i2cSend);
        Serial.print(i2cSend);

        Wire.write("/");        
        Serial.print("/");

        sprintf(i2cSend, "%.4f", z_axes);  

        Wire.write(i2cSend);
        Serial.print(i2cSend);

        Wire.write("/");        
        Serial.print("/");

        Wire.endTransmission();



    }

}
