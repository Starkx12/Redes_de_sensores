//Includes
#include <arduino.h>
#include "BBTimer.hpp"
#include <Wire.h>
#include <Arduino_LSM9DS1.h>

//External pins
int LEDPin = D2;

//Global variables
bool send_flag = false;
bool recived_flag = false;
int Counter_200ms = 0;
char i2cSend[8];
float x_axes;
float y_axes;
float z_axes;
float x_degrees;
float y_degrees;
float z_degrees;
float x_magnetic;
float y_magnetic;
float z_magnetic;
float Measures[5][3][3];


//Functions

//Interruptions
BBTimer my_t1(BB_TIMER1);

void t1Callback()
{
  recived_flag = false;
  send_flag = true; // Flag de lectura del ADC
  Counter_200ms = (Counter_200ms+1)%5;
}

void receiveEvent(int howMany){
  String data = "";
  while(Wire.available()){
    char lectura = Wire.read();
    data += lectura;
  }
  Serial.println(data);  
  recived_flag = true;
}

//Control
//#define writter
#define reader
void setup() {

  Serial.begin(9600);
  while(!Serial);

  #ifdef writter

    Wire.begin();    

    my_t1.setupTimer(200000, t1Callback);
    my_t1.timerStart();

    if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1);
    }

  #endif

  #ifdef reader

    Wire.begin(8);
    Wire.onReceive(receiveEvent);

    if(!IMU.begin()){
      Serial.println("Error al inicializar el IMU!");
      while(1);
    }

    my_t1.setupTimer(1000000, t1Callback);

  #endif

}

void loop() {
  
  #ifdef writter

  Wire.beginTransmission(8);
  if (Counter_200ms<5 and Counter_200ms>=0 and send_flag){

    send_flag = false;

    if(IMU.accelerationAvailable() and IMU.gyroscopeAvailable() and IMU.magneticFieldAvailable()){  
      IMU.readAcceleration(x_axes, y_axes, z_axes);
      IMU.readGyroscope(x_degrees, y_degrees, z_degrees);
      IMU.readMagneticField(x_magnetic, y_magnetic, z_magnetic);
    }

    Measures[Counter_200ms][0][0] = x_axes*100;
    Measures[Counter_200ms][0][1] = y_axes*100;
    Measures[Counter_200ms][0][2] = z_axes*100; 

    Measures[Counter_200ms][1][0] = x_degrees;
    Measures[Counter_200ms][1][1] = y_degrees;
    Measures[Counter_200ms][1][2] = z_degrees;

    Measures[Counter_200ms][2][0] = x_magnetic;
    Measures[Counter_200ms][2][1] = y_magnetic;
    Measures[Counter_200ms][2][2] = z_magnetic;

    if(Counter_200ms==4){
      for(int sample = 0; sample<5; sample++){

        for(int meas=0; meas <3; meas++){

          for(int axe=0; axe <3; axe++){

            float data = Measures[sample][meas][axe];
            sprintf(i2cSend, "%.4f", data);            
            Serial.println(i2cSend);
            Wire.write(i2cSend);

          }           
        }  
      }
      Wire.endTransmission(); 
    } 
  }  

  #endif

  #ifdef reader
    if(recived_flag){
        my_t1.timerStart();
        digitalWrite(LEDPin, HIGH);
    }else{
        my_t1.timerStop();
        digitalWrite(LEDPin, LOW);
    }

  #endif



}
