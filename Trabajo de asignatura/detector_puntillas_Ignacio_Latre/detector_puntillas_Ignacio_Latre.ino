//Includes
#include <arduino.h>
#include "BBTimer.hpp"
#include "mbed.h"
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

//Defines
#define alpha (0.7F) 
#define UUID_MEAN_ANGLE   "1"
#define UUID_MAX_ANGLE    "2"
#define UUID_MIN_ANGLE    "3"
#define UUID_TIME_UP      "4"
#define UUID_TIME_CYCLE   "5"
#define UUID_START        "6"
#define BLE_UUID_TEST_SERVICE   "9A48ECBA-2E92-082F-C079-9E75AAE428B1"

//BLE
BLEService testService( BLE_UUID_TEST_SERVICE );
BLEStringCharacteristic Characteristic_MEAN( UUID_MEAN_ANGLE, BLERead | BLENotify,12 );
BLEStringCharacteristic Characteristic_MAX( UUID_MAX_ANGLE, BLERead | BLENotify,12 );
BLEStringCharacteristic Characteristic_MIN( UUID_MIN_ANGLE, BLERead | BLENotify,12 );
BLEStringCharacteristic Characteristic_UP( UUID_TIME_UP, BLERead | BLENotify,12 );
BLEStringCharacteristic Characteristic_CYCLE( UUID_TIME_CYCLE, BLERead | BLENotify,12 );
BLEByteCharacteristic switchCharacteristic(BLE_UUID_TEST_SERVICE, BLERead | BLEWrite);

//Global variables
bool send_flag = false;
float acc_z_filtered = 0.0;
float gyr_y_filtered = 0.0;
int counter_50Hz = 0;
float abs_angle_y = 0.0;
int dist_min = 5;
int flag_first = 0;
float cycle_time = 0.0;
float top_time = 0.0;
float max_angle = 0.0;
float min_angle = 0.0;
float mean_angle =.0;
float max_ref = 0.0;
float min_ref = 100000.0;
int counter_state = 0;
float time_up = 0.0;
float time_top = 0.0;
float time_down = 0.0;
float mean_aux = 0.0;
bool start = false;

// Definición de los estados
enum State {REST, UP, TOP, DOWN};
State actual_State = REST; 

//Interruptions
BBTimer my_t1(BB_TIMER1);

void t1Callback()
{
  send_flag = true; 
}

void setup() {

  Serial.begin(9600);

  if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU!");
      while (1);
    }

  // begin initialization
  if (!BLE.begin()) {
        while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Ignacio");
  BLE.setAdvertisedService(testService);

  // add the characteristic to the service
  testService.addCharacteristic( Characteristic_MEAN);
  testService.addCharacteristic( Characteristic_MAX);
  testService.addCharacteristic( Characteristic_MIN);
  testService.addCharacteristic( Characteristic_UP);
  testService.addCharacteristic( Characteristic_CYCLE);
  testService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(testService);

  // start advertising
  BLE.advertise();

  my_t1.setupTimer(20000, t1Callback);
  my_t1.timerStart(); 

}

void loop() { 

  //Local variables 
  float x_dps = 0.0;
  float y_dps = 0.0;
  float z_dps = 0.0;
  float x_acc = 0.0;
  float y_acc = 0.0;
  float z_acc = 0.0;  
  float angle_y = 0.0;
  
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();
  if (central) {

    while (central.connected()) { 

      if(switchCharacteristic.written()) {
        start =  (switchCharacteristic.value()>0) ? true : false;  
        if(start){
        Serial.println("ON");
        }else{
          Serial.println("OFF");
        }      
      }

      if(start==false){
        
        send_flag = false;
        acc_z_filtered = 0.0;
        gyr_y_filtered = 0.0;
        counter_50Hz = 0;
        abs_angle_y = 0.0;
        dist_min = 5;
        flag_first = 0;
        cycle_time = 0.0;
        top_time = 0.0;
        max_angle = 0.0;
        min_angle = 0.0;
        mean_angle =.0;
        max_ref = 0.0;
        min_ref = 100000.0;
        counter_state = 0;
        time_up = 0.0;
        time_top = 0.0;
        time_down = 0.0;
        mean_aux = 0.0;
        start = false;
        actual_State = REST; 

      }
      

      if (IMU.gyroscopeAvailable() and IMU.accelerationAvailable() and send_flag and start){

        send_flag = false;

        if (flag_first<50 and actual_State == REST) {flag_first+=1;}

        ///////////////////////////////////////////////////////////////////////////////////////////GET DATA    
        
        IMU.readGyroscope(x_dps, y_dps, z_dps); 
        IMU.readAcceleration(x_acc, y_acc, z_acc);  

        //Value adaptation
        x_dps *=-1.0;
        z_dps *=-1.0;          

        //Filtering data      
        acc_z_filtered = acc_z_filtered*alpha + z_acc*(1-alpha);
        gyr_y_filtered = gyr_y_filtered*alpha + z_dps*(1-alpha);

        //Saturate data     
        acc_z_filtered = (abs(acc_z_filtered) < 1e-4)? 0.0F : acc_z_filtered;
        gyr_y_filtered = (abs(gyr_y_filtered) < 0.5)? 0.0F : gyr_y_filtered;
          
        
        ///////////////////////////////////////////////////////////////////////////////////////////MEF
        
        //Update the distance between  key points
        dist_min -= 1;    
        
        //From rest to up
        if ((acc_z_filtered > 1e-3) and (dist_min<=0) and (actual_State == REST)){

          counter_state+=1;
          if(counter_state>=5){

            dist_min=5;
            counter_state=0;
            actual_State = UP ;
            time_up = millis();   

          }

        //From up to top
        }else if ( (abs(acc_z_filtered) < 5e-1) and (dist_min<=0) and (actual_State ==UP)){

          counter_state+=1;
          if(counter_state>=3){

            dist_min=5;
            counter_state=0;
            actual_State = TOP ;
            time_top = millis();
            cycle_time = abs(time_top - time_up);      
            
          }

        //From top to down
        }else if ((acc_z_filtered < -5e-4) and (dist_min<=0) and (actual_State ==TOP)){

          counter_state+=1;
          if(counter_state>=3){

            dist_min=5;
            counter_state=0;
            actual_State = DOWN;
            time_down = millis();
            top_time = abs(time_down-time_top);
            
          }
        //From down to rest
        }else if ((abs(acc_z_filtered) < 5e-2) and (dist_min<=0) and (actual_State == DOWN)){

          counter_state+=1;
          if(counter_state>=10){

            dist_min=5;
            counter_state=0;
            actual_State = REST ;    
            
          }  
            
        }else{

          actual_State = actual_State;

        }

        ///////////////////////////////////////////////////////////////////////////////////////////GET ANGLE AND RESET VALUES
        
        if(actual_State != REST){

          //Get the increment in degrees each 20ms
          angle_y = gyr_y_filtered*0.02; 
          abs_angle_y = (abs(angle_y) > 2.0F)? (abs_angle_y+angle_y) : abs_angle_y ;
          
        }else{

          angle_y = 0;
          abs_angle_y = 0;
          acc_z_filtered = 0;
          gyr_y_filtered = 0;

        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////GET ANGLE DATA
        
        if (actual_State ==TOP){

          counter_50Hz +=1; 

          if(abs_angle_y > max_ref and abs_angle_y > 0){
            max_ref = abs_angle_y;
          }

          if(abs_angle_y < min_ref and abs_angle_y > 0){
            min_ref = abs_angle_y;
          }

          if(abs_angle_y > 0){
            mean_aux +=abs_angle_y;
          }
          
        }else{

          if((max_ref>0) and (min_ref <100000)){

            max_angle = max_ref;
            min_angle = min_ref;
            mean_angle = mean_aux/counter_50Hz;

            max_ref = 0.0;
            min_ref = 100000.0;
            counter_50Hz =0;  
            mean_aux=0;            
            
            Characteristic_MEAN.writeValue( String(mean_angle));
            Characteristic_MAX.writeValue( String(max_angle));
            Characteristic_MIN.writeValue( String(min_angle));
            Characteristic_UP.writeValue( String(top_time));
            Characteristic_CYCLE.writeValue( String(cycle_time));
            Serial.println(String(mean_angle));
            Serial.println(String(max_angle));
            Serial.println(String(min_angle));
            Serial.println(String(top_time));
            Serial.println(String(cycle_time));
            
          }
        }    
      }
    }
  }
}
