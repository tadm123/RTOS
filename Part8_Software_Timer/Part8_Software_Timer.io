/*
We’ll create a one-shot timer, a one-shot timer will call an arbitrary function after some time period, 
but will only call it once. All we’ll do un the function is to say that the timer expired in the serial terminal.
*/

// You'll likely need this in vanilla FreeRTOS
//#include timers.h

// only use core 1 for this demo
#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif


// Globals
static TimerHandle_t one_shot_timer = NULL;

// Called when one of the timers expires
void myTimerCallback(TimerHandle_t xTimer)
{
 Serial.println("Timer expired");
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
                           pdFALSE,                   // Auto-reload
                           (void *)0,                 // Timer ID
                           myTimerCallback);          // Callback function


 // Check to make sure timers were created
 if (one_shot_timer == NULL)
 {
   Serial.println("Could not create one of the timers");
 }
 else
 {
   // Wait and then print out a message that we're starting the timer
   vTaskDelay(1000 / portTICK_PERIOD_MS);
   Serial.println("Starting timers...");


   // Start timer (max block time if command queue is full)
   xTimerStart(one_shot_timer, portMAX_DELAY);
 }

 // Delete self task to show the timers will work with no user tasks
 vTaskDelete(NULL);
}


void loop()
{
}
