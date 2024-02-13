#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

#define BLE_UUID_ACCELERATION_X   "1"
#define BLE_UUID_ACCELERATION_Y   "2"
#define BLE_UUID_ACCELERATION_Z   "3"
#define BLE_UUID_ENABLE "4"
#define BLE_UUID_TEST_SERVICE   "9A48ECBA-2E92-082F-C079-9E75AAE428B1"
float x, y, z;
bool enable = false;
BLEService testService( BLE_UUID_TEST_SERVICE );
// BLE LED Switch Characteristic 
BLEFloatCharacteristic accelerationCharacteristic_X( BLE_UUID_ACCELERATION_X, BLERead | BLENotify );
BLEFloatCharacteristic accelerationCharacteristic_Y( BLE_UUID_ACCELERATION_Y, BLERead | BLENotify );
BLEFloatCharacteristic accelerationCharacteristic_Z( BLE_UUID_ACCELERATION_Z, BLERead | BLENotify );
BLEByteCharacteristic switchCharacteristic(BLE_UUID_ENABLE, BLERead | BLEWrite);

void setup() {

  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  // set LED's pin to output mode 
  pinMode(LED_BUILTIN, OUTPUT);  
  digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED


  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Ignacio");
  BLE.setAdvertisedService(testService);

  // add the characteristic to the service
  testService.addCharacteristic( accelerationCharacteristic_X);
  testService.addCharacteristic( accelerationCharacteristic_Y);
  testService.addCharacteristic( accelerationCharacteristic_Z);
  testService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(testService);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);            // turn on the LED to indicate the connection

    // while the central is still connected to peripheral:
    while (central.connected()) {

      if(switchCharacteristic.written()) {
        enable =  (switchCharacteristic.value()>0) ? true : false;
      }


       if (IMU.accelerationAvailable() and enable) {
      IMU.readAcceleration(x, y, z);
      Serial.println(x);
      accelerationCharacteristic_X.writeValue( x );
      Serial.println(y);
      accelerationCharacteristic_Y.writeValue( y );
      Serial.println(z);
      accelerationCharacteristic_Z.writeValue( z );
     }   

    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    
  }
}