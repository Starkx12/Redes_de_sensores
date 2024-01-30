
//Includes
#include <Recognition_gyroscope_3_inferencing.h>
#include <Arduino_LSM9DS1.h> 

//Constant defines 
#define RED 22     
#define BLUE 24     
#define GREEN 23
#define LED_PWR 25


//Variables
static bool debug_nn = false; 
static uint32_t run_inference_every_ms = 20;
static rtos::Thread inference_thread(osPriorityLow);
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };
static float inference_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

/* Forward declaration */
void run_inference_background();

void setup()
{
    
    Serial.begin(115200);    
    while (!Serial);    

    while(!IMU.begin());      

    inference_thread.start(mbed::callback(&run_inference_background));
    
    pinMode(RED, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(LED_PWR, OUTPUT);
}

float ei_get_sign(float number) {
    return (number >= 0.0) ? 1.0 : -1.0;
}

void run_inference_background()
{
    // wait until we have a full buffer
    delay((EI_CLASSIFIER_INTERVAL_MS * EI_CLASSIFIER_RAW_SAMPLE_COUNT));

    // This is a structure that smoothens the output result
    // With the default settings 70% of readings should be the same before classifying.
    ei_classifier_smooth_t smooth;
    ei_classifier_smooth_init(&smooth, 10 /* no. of readings */, 10 /* min. readings the same */, 0.6 /* min. confidence */, 0.5 /* max anomaly */);

    while (1) {
        // copy the buffer
        memcpy(inference_buffer, buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE * sizeof(float));

        // Turn the raw buffer in a signal which we can the classify
        signal_t signal;
        int err = numpy::signal_from_buffer(inference_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
            ei_printf("Failed to create signal from buffer (%d)\n", err);
            return;
        }

        // Run the classifier
        ei_impulse_result_t result = { 0 };

        err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", err);
            return;
        }        
        
        // ei_classifier_smooth_update yields the predicted label
        const char *prediction2 = ei_classifier_smooth_update(&smooth, &result);
        String prediction = String(prediction2);
          
          

          if(prediction.equals("Pitch_neg")){
            digitalWrite(RED, HIGH);
            digitalWrite(BLUE, LOW);
            digitalWrite(GREEN, HIGH);
            digitalWrite(LED_PWR, HIGH);
            Serial.println("Pitch_neg");

          } else if(prediction.equals("Pitch_pos")){
            digitalWrite(RED, LOW);
            digitalWrite(BLUE, LOW);
            digitalWrite(GREEN, HIGH);
            digitalWrite(LED_PWR, LOW);
            Serial.println("Pitch_pos");

          } else if(prediction.equals("Roll_neg")){

            digitalWrite(RED, HIGH);
            digitalWrite(BLUE, LOW);
            digitalWrite(GREEN, LOW);
            digitalWrite(LED_PWR, HIGH);
            Serial.println("Roll_neg");

          } else if(prediction.equals("Roll_pos")){

            digitalWrite(RED, HIGH);
            digitalWrite(BLUE, LOW);
            digitalWrite(GREEN, LOW);
            digitalWrite(LED_PWR, LOW);
            Serial.println("Roll_pos");

          } else if(prediction.equals("Yaw_neg")){

            digitalWrite(RED, HIGH);
            digitalWrite(BLUE, HIGH);
            digitalWrite(GREEN, LOW);
            digitalWrite(LED_PWR, HIGH);
            Serial.println("Yaw_neg");

          } else if(prediction.equals("Yaw_pos")){

            digitalWrite(RED, LOW);
            digitalWrite(BLUE, HIGH);
            digitalWrite(GREEN, LOW);
            digitalWrite(LED_PWR, LOW);
            Serial.println("Yaw_pos");

          } else {
            digitalWrite(RED, HIGH);
            digitalWrite(BLUE, HIGH);
            digitalWrite(GREEN, HIGH);
            digitalWrite(LED_PWR, HIGH);
            Serial.println("UNKNOWN");
          }
          

        delay(run_inference_every_ms);
    }

   
}


void loop()
{
    while (1) {
        // Determine the next tick (and then sleep later)
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        // roll the buffer -3 points so we can overwrite the last one
        numpy::roll(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, -3);

        // read to the end of the buffer
        IMU.readGyroscope(
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 3],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 2],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1]
        );       

        // and wait for next tick
        uint64_t time_to_wait = next_tick - micros();
        delay((int)floor((float)time_to_wait / 1000.0f));
        delayMicroseconds(time_to_wait % 1000);
    }
}
