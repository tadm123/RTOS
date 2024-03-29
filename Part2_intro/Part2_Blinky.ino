/* 
Simple Blinky project. Practicing with creating Tasks on the microcontroller.
*/

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins 
static const int led_pin = 13;   //13 is the inbuilt LED pin

// Our task: Blink an LED
void toggleLED(void *parameter)    // for FreeRTOS you pass a void pointer as parameter. for not lets keep it simple and dont pass any arguments
{                                 
  while(true)
  {
    digitalWrite(led_pin, HIGH);          //turn LED on
    vTaskDelay(500 / portTICK_PERIOD_MS); //wait for 500 ms (miliseconds)
    digitalWrite(led_pin, LOW);           //turn LED off
    vTaskDelay(500 / portTICK_PERIOD_MS); //wait for 500 ms
  }
}


void setup() {
  // Configure pin
  pinMode(led_pin, OUTPUT);

  // Task to run forever
  xTaskCreatePinnedToCore(    // Use xTaskCreate() in vanilla FreeRTOS
            toggleLED,        // Function to be called (The one we just created above)
            "Toggle LED",     // Name of task
            1024,             // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,             // Paremeters to pass to function
            1,                // Task priority (0 to configMAX_PRIORITIES - 1)
            NULL,             // Task to handle
            app_cpu);

  // If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler() in
  // main after setting up your tasks.

}

void loop() {
  // put your main code here, to run repeatedly:

}
