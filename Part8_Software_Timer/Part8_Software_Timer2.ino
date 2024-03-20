/*
We’ll add an auto-reload timer to see how it compares to the first program.
*/

// only use core 1 for this demo
#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif

// Globals
static TimerHandle_t one_shot_timer = NULL;      //handle to a one shot timer
static TimerHandle_t auto_reload_timer = NULL;   //handle to a auto reload timer

// Called when one of the timers expires
void myTimerCallback(TimerHandle_t xTimer)
{
  // Print message if timer 0 expired
 if ((uint32_t)pvTimerGetTimerID(xTimer) == 0) //since TaskID was casted as pointer to conform to the syntax of the TimerCreate function, you’ll have to cast it to an integer to see the number.
   Serial.println("One-shot timer expired");


 if ((uint32_t)pvTimerGetTimerID(xTimer) == 1)
   Serial.println("Auto reload timer expired");

}

void setup()
{

 // Configure Serial
 Serial.begin(115200);

 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Timer Demo---");

 one_shot_timer = xTimerCreate(
                           "One-shot timer",          // Name of timer
                           2000 / portTICK_PERIOD_MS,  // Period of timer (in ticks)
                           pdFALSE,                   // Auto-reload (for one-shot timer is set as pdFALSE)
                           (void *)0,                 // Timer ID
                           myTimerCallback);          // Callback function

 auto_reload_timer = xTimerCreate(
                           "Auto-reload timer",          // Name of timer
                           1000 / portTICK_PERIOD_MS,  // Period of timer (in ticks)
                           pdTRUE,                    // Auto-reload 
                           (void *)1,                 // Timer ID
                           myTimerCallback);          // Callback function


 // Check to make sure timers were created
 if (one_shot_timer == NULL || auto_reload_timer == NULL)
 {
   Serial.println("Could not create one of the timers");
 }
 else
 {
   // Wait and then print out a message that we're starting the timer
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   Serial.println("Starting timers...");


   // START TIMER (max block time if command queue is full)
   xTimerStart(one_shot_timer, portMAX_DELAY);
   xTimerStart(auto_reload_timer, portMAX_DELAY);
 }

 // Delete self task to show the timers will work with no user tasks
 vTaskDelete(NULL);
}

void loop()
{
}




