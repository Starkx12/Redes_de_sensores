// Tiptoe detection, measuring mean, max, min angles, up and cycle times
// Send data with BLE comunication and starting the process with a BLE characteristic
// Created by Ignacio Latre and Diego Remon
// https://github.com/DiegoRemonn/Redes-de-Sensores
// https://github.com/Starkx12/Redes_de_sensores

// Includes
#include <arduino.h>
#include "BBTimer.hpp"
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

// Designation of a BBTimer's class timer
BBTimer my_t3(BB_TIMER3);

// Variables and constants initialization
// Global variables
bool send_flag = false;
float acc_z_filtered = 0.0;
float acc_z_filtered_amplif = 0.0;
float acc_max = 0.0;
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
bool startSwitch = false;
float alpha = 0.7;

// States definition
enum State {REST, UP, TOP, DOWN};
State actual_State = REST;

// Personalized UUID for the BLE services and characteristics
const char* deviceServiceUuid = "F000AA10-0451-4000-B000-000000000000";
const char* meanAngleCharacteristicUuid = "F000AA11-0451-4000-B000-000000000000";
const char* maxAngleCharacteristicUuid = "F000AA12-0451-4000-B000-000000000000";
const char* minAngleCharacteristicUuid = "F000AA13-0451-4000-B000-000000000000";
const char* timeUpCharacteristicUuid = "F000AA14-0451-4000-B000-000000000000";
const char* timeCycleCharacteristicUuid = "F000AA15-0451-4000-B000-000000000000";
const char* startCharacteristicUuid = "F000AA16-0451-4000-B000-000000000000";

BLEService deviceService(deviceServiceUuid); // BLE Accelerometer Service

// BLE Accelerometer Characteristic - custom 128-bit UUID, read and notify by central
// Syntax: BLE<DATATYPE>Characteristic <NAME>(<UUID>,<PROPERTIES>,<DATA LENGTH>)
BLEStringCharacteristic ble_meanAngle(meanAngleCharacteristicUuid, BLERead | BLENotify, 12);
BLEStringCharacteristic ble_maxAngle(maxAngleCharacteristicUuid, BLERead | BLENotify, 12);
BLEStringCharacteristic ble_minAngle(minAngleCharacteristicUuid, BLERead | BLENotify, 12);
BLEStringCharacteristic ble_upTime(timeUpCharacteristicUuid, BLERead | BLENotify, 12);
BLEStringCharacteristic ble_cycleTime(timeCycleCharacteristicUuid, BLERead | BLENotify, 12);
BLEBoolCharacteristic ble_startSwitch("startCharacteristicUuid", BLERead | BLEWrite);

// BLE callback function for the timer
void t3Callback()
{
  send_flag = true; 
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // IMU sensor initialization (Inertial Measurement Unit)
  // Correct start verification
  if(!IMU.begin()){
    Serial.println("Error al inicializar el IMU!");
    while(1);
  }

  // Set LED's pin to output mode
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(LED_BUILTIN, LOW);    // when the central disconnects, turn off the LED

  // Begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy failed!");
    while (1);
  }

  // Set advertised local name and service UUID:
  BLE.setLocalName("Diego"); // set your own local name
  BLE.setAdvertisedService(deviceService);

  // Add the characteristic to the service
  deviceService.addCharacteristic(ble_meanAngle);
  deviceService.addCharacteristic(ble_maxAngle);
  deviceService.addCharacteristic(ble_minAngle);
  deviceService.addCharacteristic(ble_upTime);
  deviceService.addCharacteristic(ble_cycleTime);
  deviceService.addCharacteristic(ble_startSwitch);

  // Switch initialization in stop position
  ble_startSwitch.writeValue(startSwitch); 

  // Add service
  BLE.addService(deviceService);

  // Start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  Serial.println("Bluetooth device is now active, waiting for connections...");

  // Initialization and start of the interrupt timer with a 20 ms period (50 Hz)
  my_t3.setupTimer(20000, t3Callback);
  my_t3.timerStart();

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
  
  // Listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // If a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // Print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection

    // While the central is still connected to peripheral:
    while (central.connected()) { 
      if (ble_startSwitch.written()) {
        start = ble_startSwitch.value();  
        if (start) {
          Serial.println("ON");
        }else{
          Serial.println("OFF");
        }      
      }

      if (start==false) {
        
        send_flag = false;
        acc_z_filtered = 0.0;
        acc_z_filtered_amplif = 0.0;
        acc_max = 0.0;
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

        ///////////////////////////////////////////////////////////////////////////////////////////GET DATA    
        
        IMU.readGyroscope(x_dps, y_dps, z_dps); 
        IMU.readAcceleration(x_acc, y_acc, z_acc);  

        //Value adaptation
        x_dps *=-1.0;
        z_dps *=-1.0;          

        //Filtering data      
        acc_z_filtered = acc_z_filtered*alpha + x_acc*(1-alpha);
        gyr_y_filtered = gyr_y_filtered*alpha + z_dps*(1-alpha);

        //Saturate data     
        acc_z_filtered = (abs(acc_z_filtered) < 1e-4)? 0.0F : acc_z_filtered;
        gyr_y_filtered = (abs(gyr_y_filtered) < 1)? 0.0F : gyr_y_filtered;

        acc_z_filtered_amplif = acc_z_filtered*100;
        //Serial.print("Acc(z) filtered: ");
        //Serial.println(y_dps);
        
        ///////////////////////////////////////////////////////////////////////////////////////////MEF
        
        //Update the distance between  key points
        dist_min -= 1;    
        
        //From rest to up
        if ((acc_z_filtered_amplif < -2) and (abs(y_dps) < 50) and (gyr_y_filtered > 7) and (dist_min<=0) and (actual_State == REST)){

          counter_state+=1;
          if(counter_state>=5){

            dist_min=5;
            counter_state=0;
            actual_State = UP ;
            time_up = millis();
            Serial.println("UP");
          }

        //From up to top
        }else if ((gyr_y_filtered < 7) and (dist_min<=0) and (actual_State == UP)){

          counter_state+=1;
          if(counter_state>=3){

            dist_min=5;
            counter_state=0;
            actual_State = TOP ;
            time_top = millis();
            cycle_time = abs(time_top - time_up);    
            Serial.println("TOP");
            acc_max = acc_z_filtered_amplif;
          }

        //From top to up
        }else if ((gyr_y_filtered > 7) and (dist_min<=0) and (actual_State == TOP)){

          counter_state+=1;
          if(counter_state>=3){

            dist_min=5;
            counter_state=0;
            actual_State = UP;
            time_down = millis();
            top_time = abs(time_down-time_top);
            Serial.println("UP");
          }

        //From top to down
        }else if ((acc_z_filtered_amplif > acc_max + 7) and (gyr_y_filtered < -7) and (dist_min<=0) and (actual_State == TOP)){

          counter_state+=1;
          if(counter_state>=3){

            dist_min=5;
            counter_state=0;
            actual_State = DOWN;
            time_down = millis();
            top_time = abs(time_down-time_top);
            Serial.println("DOWN");
          }

        //From down to rest
        }else if ((acc_z_filtered_amplif > -2) and (gyr_y_filtered > -7) and (dist_min<=0) and (actual_State == DOWN)){

          counter_state+=1;
          if(counter_state>=10){

            dist_min=5;
            counter_state=0;
            actual_State = REST;
            Serial.println("REST");
          }  
            
        }else{
          actual_State = actual_State;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////GET ANGLE AND RESET VALUES
        
        if(actual_State != REST){

          //Get the increment in degrees each 20ms
          angle_y = gyr_y_filtered * 0.027; 
          abs_angle_y += angle_y;
          if (abs_angle_y < 0) {
            abs_angle_y = 0.0;
          }
          //Serial.print("Angulo girado: ");
          //Serial.println(abs_angle_y);
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
            
            ble_meanAngle.writeValue(String(mean_angle));
            ble_maxAngle.writeValue(String(max_angle));
            ble_minAngle.writeValue(String(min_angle));
            ble_upTime.writeValue(String(top_time));
            ble_cycleTime.writeValue(String(cycle_time));
            Serial.println(String(mean_angle));
            Serial.println(String(max_angle));
            Serial.println(String(min_angle));
            Serial.println(String(top_time));
            Serial.println(String(cycle_time));
            
          }
        }    
      }
    }
    
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);    // when the central disconnects, turn off the LED
  }
}
