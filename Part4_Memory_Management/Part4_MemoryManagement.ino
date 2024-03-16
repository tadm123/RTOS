/* 
This program does nothing but print numbers in an array and store those numbers in an array and then print one of those numbers. 
We'll us dynamic memory in the heap for this.

Let’s start with a pretty big array of 100 elements. Because these are ints (4 bytes). This should be 100 * 4 bytes = 400 bytes.
Well give the task 1500 bytes of stack
*/


// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif


// Task: Perform some mundane task
void testTask(void *parameter)
{
 while(1){
   int a = 1;
   int b[100];

   // Do something with array so it's not optmized out by the compiler
   for (int i = 0; i < 100; i++){
     b[i] = a + 1;
   }
   Serial.println(b[0]);

   // Print out remaining stack memory (words)
   Serial.print("High water mark (words): ");
   Serial.println(uxTaskGetStackHighWaterMark(NULL));

   // Print out number of free heap memory bytes before malloc
   Serial.print("Heap before malloc (bytes): ");
   Serial.println(xPortGetFreeHeapSize() );

   // in FreeRTOS we use pvPortMalloc (regular malloc is not thread safe)
   int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

   // One way to prevent heap overflow is to check the malloc output
   if (ptr == NULL)
     Serial.println("Not enough heap.");

   else
     // Do something with the memory so it's not optimized out by the compiler
     for (int i = 0; i < 1024; i++)
       ptr[i] = 3;
  

   // Print out number of free heap memory bytes after malloc
   Serial.print("Heap after malloc (bytes): ");
   Serial.println(xPortGetFreeHeapSize() );


   // Free up our allocated memory
   //vPortFree(ptr);    //commented out to see what happens

   // Wait for a while
   vTaskDelay(100 / portTICK_PERIOD_MS);

 }
}


void setup(){
  
 // Configure Serial
 Serial.begin(115200);

 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Memory Demo---");

 // Start the only other task
 xTaskCreatePinnedToCore(testTask,
                         "Test Task",
                         1500,
                         NULL,
                         1,
                         NULL,
                         app_cpu);

 // Delete task
 vTaskDelete(NULL);
}

void loop(){
}

