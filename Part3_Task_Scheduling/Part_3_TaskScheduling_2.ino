/*
This programs  uses two Tasks to control the blinking rate of an LED. 
One task listens for input in the serial terminal, when you enter the number the delay time on the blinking LED should be updated to that time. 
This allows us to create a simple user interface that runs independently of the hardware being controlled.
*/

// Needed for atoi() converts string to integer
#include <stdlib.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 20;

// Pins
static const int led_pin = 14;

// Globals
static int led_delay = 500;   // ms

//****************************************************************************
// Tasks

// Task: Blink LED at rate set by global variable
void toggleLED(void *parameter) {
 while (1) {
   digitalWrite(led_pin, HIGH);
   vTaskDelay(led_delay / portTICK_PERIOD_MS);
   digitalWrite(led_pin, LOW);
   vTaskDelay(led_delay / portTICK_PERIOD_MS);
 }
}

void readSerial(void *parameters) {

 char c;
 char buf[buf_len];
 uint8_t idx = 0;

 // Clear whole buffer
 memset(buf, 0, buf_len);

 // Loop forever
 while (1) {

   if (Serial.available() > 0) {
     	c = Serial.read();


     	// Update delay variable and reset buffer if we get a newline character
     	if (c == '\n') {
       	led_delay = atoi(buf);   //this is an update to the global variable for delay
       	Serial.print("Updated LED delay to: ");
       	Serial.println(led_delay);
       	memset(buf, 0, buf_len);
       	idx = 0;
     } 
    else {
       // Only append if index is not over message limit
       if (idx < buf_len - 1) {
         buf[idx] = c;
         idx++;
       }
     }
   }
 }
}

//*****************************************************************************
// Main


void setup() {
 // Configure pin
 pinMode(led_pin, OUTPUT);

 // Configure serial and wait a second
 Serial.begin(115200);
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println("Multi-task LED Demo");
 Serial.println("Enter a number in milliseconds to change the LED delay.");

 // Start blink task
 xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
           toggleLED,      // Function to be called
           "Toggle LED",   // Name of task
           1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
           NULL,           // Parameter to pass
           1,              // Task priority
           NULL,           // Task handle
           app_cpu);       // Run on one core for demo purposes (ESP32 only)
          
 // Start serial read task
 xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
           readSerial,     // Function to be called
           "Read Serial",  // Name of task
           1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
           NULL,           // Parameter to pass
           1,              // Task priority (must be same to prevent lockup)
           NULL,           // Task handle
           app_cpu);       // Run on one core for demo purposes (ESP32 only)

 // Delete "setup and loop" task
 vTaskDelete(NULL);
}

void loop() {
}
