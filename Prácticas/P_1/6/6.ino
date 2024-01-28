//Includes
#include <arduino.h>
#include "BBTimer.hpp"
#include "mbed.h"
#include <Arduino_LSM9DS1.h>

//External pins

//Global variables
float x_axes = 0.0;
float y_axes = 0.0;
float z_axes = 0.0;
float x_degrees = 0;
float y_degrees = 0;
float z_degrees = 0;
float x_magnetic = 0;
float y_magnetic = 0;
float z_magnetic = 0;
bool send_flag = false;
int Counter_100ms = 0;
float Measures[10][3][3] ;
//Functions

//Interruptions
BBTimer my_t1(BB_TIMER1);

void t1Callback()
{
  send_flag = true; // Flag de lectura 
  Counter_100ms = (Counter_100ms+1)%10;
}

void setup() {

  Serial.begin(9600);
  if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1);
    }
  my_t1.setupTimer(100000, t1Callback);
  my_t1.timerStart();
}

void loop() {  
     
   if (Counter_100ms<10 and Counter_100ms>=0 and send_flag){

    if (IMU.accelerationAvailable() and IMU.gyroscopeAvailable() and IMU.magneticFieldAvailable()) {
      send_flag = false;

      IMU.readAcceleration(x_axes, y_axes, z_axes); 
      Measures[Counter_100ms][0][0] = x_axes*100;
      Measures[Counter_100ms][0][1] = y_axes*100;
      Measures[Counter_100ms][0][2] = z_axes*100;

      IMU.readGyroscope(x_degrees, y_degrees, z_degrees); 
      Measures[Counter_100ms][1][0] = x_degrees;
      Measures[Counter_100ms][1][1] = y_degrees;
      Measures[Counter_100ms][1][2] = z_degrees;

      IMU.readMagneticField(x_magnetic, y_magnetic, z_magnetic);
      Measures[Counter_100ms][2][0] = x_magnetic;
      Measures[Counter_100ms][2][1] = y_magnetic;
      Measures[Counter_100ms][2][2] = z_magnetic;   

      if(Counter_100ms==9){
        for(int sample = 0; sample<9; sample++){          

          for(int meas=0; meas <3; meas++){

            Serial.println('\n');
            if(meas==0){Serial.println("The sensor is an acceleroemter:  ");}
            if(meas==1){Serial.println("The sensor is a gyroscope:  ");}
            if(meas==2){Serial.println("The sensor is a magnetic field sensor:  ");}
            
            for(int axe=0; axe <3; axe++){
              float data = Measures[sample][meas][axe];   
              Serial.print('|');           
              Serial.print(data);
              Serial.print('|');
              if(axe==2){
                Serial.print(" Sample number: ");
                Serial.print(sample);
                Serial.print('|');
              }
            }           
          }  
        }
      }
    }
  }   
  


}
