/*
Making the LED blink at different rates, that means creating another thread or tasks just like that one we 
had on the first Blinky program and setting the delay to something different:
For this I just used two different LEDs instead (pin 14 and pin 4)
*/

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif


// Pins
static const int led_pin14 = 14;  
static const int led_pin4 = 4;  

// Our task: Blink an LED
void toggleLED500(void *parameter)    // for FreeRTOS you pass a void pointer as parameter. for not lets keep it simple and dont pass any arguments
{                                
 while(true)
 {
   digitalWrite(led_pin14, HIGH);          //turn LED on
   vTaskDelay(500 / portTICK_PERIOD_MS); //wait for 500 ms (miliseconds)
   digitalWrite(led_pin14, LOW);           //turn LED off
   vTaskDelay(500 / portTICK_PERIOD_MS); //wait for 500 ms
 }
}


void toggleLED100(void *parameter)    // for FreeRTOS you pass a void pointer as parameter. for not lets keep it simple and dont pass any arguments
{                                
 while(true)
 {
   digitalWrite(led_pin4, HIGH);          //turn LED on
   vTaskDelay(100 / portTICK_PERIOD_MS); //wait for 100 ms (miliseconds)
   digitalWrite(led_pin4, LOW);           //turn LED off
   vTaskDelay(100 / portTICK_PERIOD_MS); //wait for 100 ms
 }
}


void setup() {
 // Configure pin
 pinMode(led_pin14, OUTPUT);
 pinMode(led_pin4, OUTPUT);

 // Task to run forever
 xTaskCreatePinnedToCore(    // Use xTaskCreate() in vanilla FreeRTOS
           toggleLED500,        // Function to be called (The one we just created above)
           "Toggle LED 500ms",     // Name of task
           1024,             // Stack size (bytes in ESP32, words in FreeRTOS)
           NULL,             // Paremeters to pass to function
           1,                // Task priority (0 to configMAX_PRIORITIES - 1)
           NULL,             // Task to handle
           app_cpu);

// Task to run forever
 xTaskCreatePinnedToCore(    // Use xTaskCreate() in vanilla FreeRTOS
           toggleLED100,        // Function to be called (The one we just created above)
           "Toggle LED 100ms",     // Name of task
           1024,             // Stack size (bytes in ESP32, words in FreeRTOS)
           NULL,             // Paremeters to pass to function
           1,                // Task priority (0 to configMAX_PRIORITIES - 1)
           NULL,             // Task to handle
           app_cpu);
}

void loop() {
  Serial.begin(9600);
  Serial.println("Printing...");
  vTaskDelay(500 / portTICK_PERIOD_MS);
}
