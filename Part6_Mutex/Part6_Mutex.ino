/*
The program is simple, we have two tasks, and a shared variable integer. 
Both tasks are accessing and working on the critical section, which consists of increasing the shared_var counter. 
Each Task has a local variable, it increases the this local variable and then copies the increased value to the global shared_var.
We use a mutex so that there’s no overlap and the counter is increased by one Task at a time.
*/

// You'll likely need this on vanilla FreeRTOS
//#include semphr.h

// Use only core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Globals
static int shared_var = 0;
static SemaphoreHandle_t mutex;  //declare our global mutex. Note that FreeRTOs generalizes mutex and semaphroes as just semaphores

// Increment shared variable (the wrong way)
void incTask(void *parameters)
{
 int local_var;
  while (1)
 {
   // Take mutex prior to critcal section (if mutex is available (no one has took it, it’ll take it)
   if (xSemaphoreTake(mutex, 0) == pdTRUE)
   {

     //**** CRITICAL SECTION to work on
     // Roundabout way to do "shared_var++" randomly and poorly
     local_var = shared_var;
     local_var++;
     vTaskDelay(random(100,500) / portTICK_PERIOD_MS);
     shared_var = local_var;
     //**** CRITICAL SECTION to work on

     
     // Give mutex  after critical section
     xSemaphoreGive(mutex);

     // Print out new shared variable
     Serial.println(shared_var);
   }
   else
   {
     // Do some other thing if you like, while waiting for the mutex
   }
 }
}

void setup() {

 // Hack to kinda get randomness
 randomSeed(analogRead(0));

 // Configure Serial
 Serial.begin(115200);


 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Heap Demo---");


 // Create mutex before starting tasks
 mutex = xSemaphoreCreateMutex();

 // Start Serial receive task
 xTaskCreatePinnedToCore(incTask,
                         "Increment Task 1",
                         1024,
                         NULL,
                         1,
                         NULL,
                         app_cpu);

 // Start Serial receive task
 xTaskCreatePinnedToCore(incTask,
                         "Increment Task 2",
                         1024,
                         NULL,
                         1,
                         NULL,
                         app_cpu);

 // Delete "setup and loop" task
 vTaskDelete(NULL);
}


void loop()
{
}
