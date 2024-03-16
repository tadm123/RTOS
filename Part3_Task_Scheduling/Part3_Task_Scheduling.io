/*
Task Scheduling:
Here we’ll have two Tasks that print string or characters on the serial monitor, they’ll interrupt each other. 
*/

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE	//this will return false (reading from config in freertos.h file), so we’ll set it to run on the second core app_cpu=1
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins
const char msg[] = "Barkdadeer brig Arr booty rum.";

//Task handles
static TaskHandle_t task_1 = NULL;  //TaskHandle_t is a data type in ESP32 to represent the task handle
static TaskHandle_t task_2 = NULL;

//*****************************************************************************

//Task 1: Print to Serial Terminal with lower priority
void startTask1(void *parameter){  //this has void function so it will not return anything, task functions need to be passed a void pointer parameter

 // Count number of characters in string
 int msg_len = strlen(msg);

 // Print strings to Terminal
 while (1){
   Serial.println();
   for (int i = 0; i < msg_len; i++){
     Serial.print(msg[i]);   //if we just use Serial.println it will print the whole string, prevent us from seeing interruptions from second Task
   }

   Serial.println();
   vTaskDelay(1000 / portTICK_PERIOD_MS);  //putting the Task in Blocked state for 1 second
  
 }
}


// Task 2: Print to Serial Terminal with higher priority. This task 2 (higher priority) preempts (interrupts) task 1 (lower priority)
void startTask2(void *parameter){
 while(1){
   Serial.print('*');
   vTaskDelay(100 / portTICK_PERIOD_MS);
 }
}

//************************************************
// Main (runs as its own task with priority 1 on core 1)


void setup(){

 // Configure Serial baud rate (go slow so we can watch the preemption)
 Serial.begin(300);


 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);  //delay
 Serial.println();
 Serial.println("---FreeRTOS Task Demo---");


 // Print self priority
 Serial.print("Setup and loop task running on core ");
 Serial.print(xPortGetCoreID());	//xPortGetCoreID() returns the ID of the core on which the currently executing task is running.
 Serial.print(" with priority ");
 Serial.println(uxTaskPriorityGet(NULL)); //this returns the priority of the task


 // Task to run forever
 xTaskCreatePinnedToCore(			// Use xTaskCreatE() in Vanilla FreeRTOS
    startTask1, 	// Function to be called
                         "Task 1",	// Name of task (for human readability)
                         1024,		// Stack size (byes in ESP32,words in FreeRTOS)
                         NULL,		// Parameters to pass to function, there are cases where no parameters are passed, you can simply put NULL in here. In C, NULL is a macro typically defined as ((void *)0), representing a null pointer constant. When you pass NULL as a parameter to a function, you're indicating that the function should expect no data to be passed to it.
                         1,		// Task priority (the lower number has lower priority)
                         &task_1,		// Task to handle (we’re giving the address of that task_1 handle) 
                         app_cpu);     //In the xTaskCreatePinnedToCore function call, the parameter app_cpu specifies the core to which the task will be pinned.


 // Task to run once with higher priority
 xTaskCreatePinnedToCore(startTask2,
                         "Task 2",
                         1024,
                         NULL,
                         2,
                         &task_2,
                         app_cpu);
          
}


void loop() {

 // Suspend and resume the higher proprity task every 2 seconds
 for (int i = 0; i < 3; i++){
   vTaskSuspend(task_2);
   vTaskDelay(2000 / portTICK_PERIOD_MS);
   vTaskResume(task_2);
   vTaskDelay(2000 / portTICK_PERIOD_MS);
 }

 // Delete the lower priority task
 if (task_1 != NULL) {    //checking if tasks is not NULL, and immedaitely setting it to NULL after deleting it
   vTaskDelete(task_1);
   task_1 = NULL;
 }

}
