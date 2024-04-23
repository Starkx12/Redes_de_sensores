#include <Arduino_LSM9DS1.h>

// Settings
#define LED_R_PIN           22        // Red LED pin
#define DELAY_COLLECTION    1000      // Delay (ms) between collections

// Constants
#define CONVERT_G_TO_MS2    1.0f  // Used to convert G to m/s^2
#define SAMPLING_FREQ_HZ    50       // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms)
#define NUM_SAMPLES         100       // 100 samples at 100 Hz is 1 sec window
bool flag_send_data = false;
void setup() {

  // Enable LED pin (RGB LEDs are active low)
  pinMode(LED_R_PIN, OUTPUT);
  digitalWrite(LED_R_PIN, HIGH);

  // Start serial
  Serial.begin(115200);

  // Start accelerometer (part of IMU)
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {
  
  float acc_x;
  float acc_y;
  float acc_z;
  float gyr_x;
  float gyr_y;
  float gyr_z;
  unsigned long timestamp;
  unsigned long start_timestamp;

  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'k'){
      flag_send_data = true;
    }
  }

  if(flag_send_data){

    pinMode(LED_R_PIN, OUTPUT);
    digitalWrite(LED_R_PIN, LOW);

    // Print header
    Serial.println("timestamp,accX,accY,accZ,gyrX,gyrY,gyrZ");

    // Record samples in buffer
    start_timestamp = millis();

    while(flag_send_data){

      if (Serial.available() > 0) {
        char command = Serial.read();
        if (command == 'w'){
          flag_send_data = false;
        }
      }

      if(flag_send_data){
        // Take timestamp so we can hit our target frequency
        timestamp = millis();
        
        // Read and convert accelerometer data to m/s^2
        IMU.readAcceleration(acc_x, acc_y, acc_z);
        acc_x *= -1*CONVERT_G_TO_MS2;
        acc_y *= -1*CONVERT_G_TO_MS2;
        acc_z *= -1*CONVERT_G_TO_MS2;

        // Read gyroscope data (in degrees/sec)
        IMU.readGyroscope(gyr_x, gyr_y, gyr_z);

        // Print CSV data with timestamp
        Serial.print(timestamp - start_timestamp);
        Serial.print(",");
        Serial.print(acc_x);
        Serial.print(",");
        Serial.print(acc_y);
        Serial.print(",");
        Serial.print(acc_z);
        Serial.print(",");
        Serial.print(-1*gyr_x);
        Serial.print(",");
        Serial.print(1*gyr_y);
        Serial.print(",");
        Serial.println(-1*gyr_z);

        // Wait just long enough for our sampling period
        while (millis() < timestamp + SAMPLING_PERIOD_MS);
      }else{
        // Print empty line to transmit termination of recording
        Serial.println();

        // Turn off LED to show we're done
        digitalWrite(LED_R_PIN, HIGH);
        flag_send_data = false;
      }

    }
  }
  // Wait before repeating the collection process
  delay(DELAY_COLLECTION);
}