#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif


// Settings
static const uint16_t timer_divider = 8; // Clock ticks at 1 Mhz now
static const uint64_t timer_max_count = 1000000;
static const TickType_t task_delay = 2000 / portTICK_PERIOD_MS;

// Pins
static const int led_pin = 14;

// Globals
static hw_timer_t *timer = NULL;
static volatile int isr_counter;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED; // special kind of mutex, this prevents tasks in the other core from entering a criticla section

// ************************************************************************************************************************
// Our Interrupt Service Routines (ISRs)


// This function executes when timer reaches max (and reset), it'll just toggle the LED
// For this we want our ISR to reside on the internal RAM intead of flash, so that it can be accessed faster
void IRAM_ATTR onTimer()        // IRAM_ATTR = IRAM attribute qualifier
{

 // ESP-IDF version of a critical section (in an ISR)
 portENTER_CRITICAL_ISR(&spinlock);
 isr_counter++;
 portEXIT_CRITICAL_ISR(&spinlock);

  // Vanilla FreeRTOS version of a critical section (in an ISR)
 //UBaseType_t saved int_status:
 //saved_int_status = taskENTER_CRITICAL_FROM_ISR();
 //isr_counter++;
 //taskEXIT_CRITICAL_FROM_ISR(saved_int_status);

}

// ************************************************************************************************************************
// Tasks

// Wait for semaphore and print out ADC values when received
void printValues(void *parameters)
{
  
 // Loop forever
 while (1)
 {
   // Count down and print out counter value
   while (isr_counter > 0)
   {
     // Print value of counter
     Serial.println(isr_counter);

     // ESP-IDF version of a critical section (in a task)
       portENTER_CRITICAL_ISR(&spinlock);
       isr_counter--;
       portEXIT_CRITICAL_ISR(&spinlock);

     // Vanilla FreeRTOS version of a critical section (in a task)
     //taskENTER_CRITICAL();
     //isr_counter--;
     //taskEXIT_CRITICAL();
   }

   // Wait 2 seconds while ISR increments counter a few times
   vTaskDelay(task_delay);
 }
}


// ************************************************************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup()
{

 // Configure LED pin
 pinMode(led_pin, OUTPUT);

 // Configure Serial
 Serial.begin(115200);

 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Timer Demo---");

 xTaskCreatePinnedToCore(printValues,
                           "Print values",
                           1024,
                           NULL,
                           1,
                           NULL,
                           app_cpu);


 // Create and start timer (num, divider, countUp)
 timer = timerBegin(0, timer_divider, true);

 // Provider ISR to timer (timer, function, edge)
 timerAttachInterrupt(timer, &onTimer, true);

 // At what count should ISR trigger (timer, count, autoreload)
 timerAlarmWrite(timer, timer_max_count, true);

 // Allow ISR to trigger
 timerAlarmEnable(timer);

 // Delete "setup and loop" task
 vTaskDelete(NULL);
}


void loop()
{

}
