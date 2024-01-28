#include <arduino.h>
#include "BBTimer.hpp"
#include "mbed.h"
#include <Wire.h>
#include <Arduino_LSM9DS1.h>

//External pins
int Voltage_pin = A0;
int PWM_pin = D2;

//Peripheral adress

//Global variables
float Voltage = 0.0;
char buffer[50];
bool send_flag = false;
bool read_flag = false;
float PWM_duty = 0.0; //The values are from 0 to 1
int PWM_freq = 5000; //The valueas are in Hz
String timer_freq_str ;
int  timer_freq = 0;
String read_value;
String value_short;
String duty_str;
float x_axes = 0.0;
float y_axes = 0.0;
float z_axes = 0.0;
float x_degrees = 0;
float y_degrees = 0;
float z_degrees = 0;
float x_magnetic = 0;
float y_magnetic = 0;
float z_magnetic = 0;
int Counter_200ms = 0; //1 = 200ms/ 2 = 400ms/ 3 = 600ms/ 4 = 800ms/ 5 = 1s
float Measures[5][3][3] ;
char data_read;
int axes_index = 0;
int axes_sensor = 0;
int n_sample =0;
//Functions
void t1Callback()
{
  send_flag = true; // Flag de lectura del ADC
  Counter_200ms = (Counter_200ms+1)%5;
}

void receiveEvent(int howMany) {
  
   data_read = Wire.read(); // receive a character
   read_flag = true;
   #ifdef I2C_two_boards_reader
    if(data_read == '0'){
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      Serial.println("TURN OFF");
    }
    else if(data_read == '1'){
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      Serial.println("TURN ON");
    }
   #endif

}

int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}

void Process_Acelerometer(float x, float y, float z){
  float degreesX, degreesY, degreesZ;
  if(x > 0.1){
    x = 100*x;
    degreesX = map(x, 0, 97, 0, 90);
    Serial.print("Tilting up ");
    Serial.print(degreesX);
    Serial.println("  degrees");
    }
  if(x < -0.1){
    x = 100*x;
    degreesX = map(x, 0, -100, 0, 90);
    Serial.print("Tilting down ");
    Serial.print(degreesX);
    Serial.println("  degrees");
    }
  if(y > 0.1){
    y = 100*y;
    degreesY = map(y, 0, 97, 0, 90);
    Serial.print("Tilting left ");
    Serial.print(degreesY);
    Serial.println("  degrees");
    }
  if(y < -0.1){
    y = 100*y;
    degreesY = map(y, 0, -100, 0, 90);
    Serial.print("Tilting right ");
    Serial.print(degreesY);
    Serial.println("  degrees");
    }
}

void Process_Gyroscope(float x,float y, float z){
  int plusThreshold = 30;
  int minusThreshold = -30;

  if(y > plusThreshold)
  {
  Serial.println("Collision front");
  
  }
  else if(y < minusThreshold)
  {
  Serial.println("Collision back");
  
  }
  else if(x < minusThreshold)
  {
  Serial.println("Collision right");
  
  }
  else if(x > plusThreshold)
  {
  Serial.println("Collision left");  
  }

}

void Process_MagneticField(float x, float y, float z){

  sprintf(buffer,"The values of the magnetic fields in uT are: %f,%f,%f",x,y,z);
    Serial.println(buffer);
}

char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}

//Interruptions
BBTimer my_t1(BB_TIMER1);
mbed::PwmOut pwmPin(digitalPinToPinName(PWM_pin));

//Control

//#define ADC
//#define ADC_timer
//#define PWM
//#define UART
//#define I2C_two_boards_reader
//#define I2C_two_boards_writter
//#define Sensors
//#define Final_work_writter
#define Final_work_reader

void setup() {
  Serial.begin(9600);

  #ifdef ADC_timer
    my_t1.setupTimer(10000000, t1Callback);
    my_t1.timerStart();
  #endif

  #ifdef PWM    
    pwmPin.period(1.0/PWM_freq);
    pwmPin.write(PWM_duty);
  #endif

  #ifdef I2C_two_boards_reader
    Wire.begin(8);                // join i2c bus with address #8
    Wire.onReceive(receiveEvent); // function that executes whenever data is received from writer
  #endif

  #ifdef I2C_two_boards_writter
    Wire.begin(8); // join i2c bus (address optional for writer)
  #endif

  #ifdef Sensors
    if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1);
    }

  #endif

  #ifdef Final_work_writter

    Wire.begin(8);                // join i2c bus with address #8
    my_t1.setupTimer(200000, t1Callback);
    my_t1.timerStart();
    if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1);
    }
  #endif

  #ifdef Final_work_reader
    Wire.begin(8);                // join i2c bus with address #8
    Wire.onReceive(receiveEvent); // function that executes whenever data is received from writer
  #endif



}

void loop() {
#ifdef ADC

  Voltage = (3.3*analogRead(Voltage_pin))/1023;
  sprintf(buffer,"The voltage in the pin A0 is: %f",Voltage);
  Serial.println(buffer);

#endif

#ifdef ADC_timer

  if (send_flag == true){

    Voltage = (3.3*analogRead(Voltage_pin))/1023;    
    sprintf(buffer,"The voltage in the pin A0 is: %f",Voltage);
    Serial.println(buffer);
    send_flag = false;

  }

#endif

#ifdef PWM

  Voltage = (3.3*analogRead(Voltage_pin))/1023;
  PWM_duty = Voltage/3.3;
  pwmPin.write(PWM_duty);

#endif

#ifdef UART 

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
      Serial.println("El valor de frecuencia es: " + timer_freq_str);

      my_t1.setupTimer(timer_freq*1000000, t1Callback);
      my_t1.timerStart();

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

#endif

#ifdef I2C_two_boards_writter

  Serial.print("Enter 0 to turn off led, enter 1 to turn it on ");
  char ledVal[0];
  readSerial(ledVal);
  Serial.println(ledVal);
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(ledVal);        // sends the given value
  Wire.endTransmission();    // stop transmitting
  delay(500);

#endif

#ifdef Sensors
  if (IMU.accelerationAvailable() and IMU.gyroscopeAvailable() and IMU.magneticFieldAvailable()) {
    IMU.readAcceleration(x_axes, y_axes, z_axes);
    Process_Acelerometer(x_axes, y_axes, z_axes);
    IMU.readGyroscope(x_degrees, y_degrees, z_degrees);
    Process_Gyroscope(x_degrees, y_degrees, z_degrees);
    IMU.readMagneticField(x_magnetic, y_magnetic, z_magnetic);
    Process_MagneticField(x_magnetic, y_magnetic, z_magnetic);
  }
  delay(100); //Warning!!! this type of function block the loop, now it is valid but in other programs not

#endif

#ifdef Final_work_writter 
   Wire.beginTransmission(8);
   if (Counter_200ms<5 and Counter_200ms>=0 and send_flag){
    if (IMU.accelerationAvailable() and IMU.gyroscopeAvailable() and IMU.magneticFieldAvailable()) {
      send_flag = false;
      IMU.readAcceleration(x_axes, y_axes, z_axes); 
      Measures[Counter_200ms][0][0] = x_axes*100;
      Measures[Counter_200ms][0][1] = y_axes*100;
      Measures[Counter_200ms][0][2] = z_axes*100;
      IMU.readGyroscope(x_degrees, y_degrees, z_degrees); 
      Measures[Counter_200ms][1][0] = x_degrees;
      Measures[Counter_200ms][1][1] = y_degrees;
      Measures[Counter_200ms][1][2] = z_degrees;
      IMU.readMagneticField(x_magnetic, y_magnetic, z_magnetic);
      Measures[Counter_200ms][2][0] = x_magnetic;
      Measures[Counter_200ms][2][1] = y_magnetic;
      Measures[Counter_200ms][2][2] = z_magnetic;      
      if(Counter_200ms==4){
        for(int sample = 0; sample<5; sample++){
          for(int meas=0; meas <3; meas++){
            for(int axe=0; axe <3; axe++){
              float data = Measures[sample][meas][axe];
              //dtostrf(data,15,5, buffer);
              sprintf(buffer,"%.4f",data);
              Serial.println(buffer);
              Wire.write(buffer);
            }           
          }  
        }
      }
    }
  }   
  

#endif

#ifdef Final_work_reader
  String dataString = "";
  float measure_I2C = 0.0;
  if(read_flag){
    read_flag = false;
    dataString = dataString + data_read;
    measure_I2C = dataString.toFloat();
    Measures[n_sample][axes_sensor][axes_index];


    if(axes_index ==3 and axes_sensor==3){n_sample=(n_sample+1)%5;}
    if(axes_index ==3){axes_sensor=(axes_sensor+1)%4;}
    axes_index = (axes_index+1)%4;
    
    
  }

#endif
}
