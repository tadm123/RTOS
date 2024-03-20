/*
This is a counting semaphore demo.
We have five producer Tasks that add values to a shared circular buffer and two consumer Tasks that read from this circular buffer. 
Each Task write it's task's number three times, so you should see 000 111 222 333 444
We'll protect the shares resource using a mutex.
For this project we'll need two counting semaphores in addition to the mutex, one to count the number of filled slots, and the other to count 
the number of empty slots in the buffer
*/


// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif


// Settings
enum {BUF_SIZE = 5};                  // Size of buffer array
static const int num_prod_tasks = 5;  // Number of producer tasks
static const int num_cons_tasks = 2;  // Number of consumer tasks
static const int num_writes = 3;      // Num times each producer writes to buf


// Globals
static int buf[BUF_SIZE];             // Shared buffer
static int head = 0;                  // Writing index to buffer
static int tail = 0;                  // Reading index to buffer
static SemaphoreHandle_t bin_sem;     // Waits for parameter to be read
static SemaphoreHandle_t mutex;       // Lock access to buffer and Serial
static SemaphoreHandle_t sem_empty;   // Counts number of empty slots in buf (this is for the counting semaphore)
static SemaphoreHandle_t sem_filled;  // Counts number of filled slots in buf


//*****************************************************************************
// Tasks


// Producer: write a given number of times to shared buffer
void producer(void *parameters) {


 // Copy the parameters into a local variable
 int num = *(int *)parameters;


 // Release the binary semaphore
 xSemaphoreGive(bin_sem);  //bin_sem at 1 now. Since it was at 0, and you Give (producer gives into the shares resources. The bin counter will be at 1 now. And since this is the max amount, then it’s full. Afterwards we’ll need to run xSemaphoreTake(bin_sem), so that it goes back to empty 0.


 // Fill shared buffer with task number
 for (int i = 0; i < num_writes; i++) {


   // Wait for empty slot in buffer to be available
   xSemaphoreTake(sem_empty, portMAX_DELAY);  


   // Lock critical section with a MUTEX
   xSemaphoreTake(mutex, portMAX_DELAY); //TAKE MUTEX
   buf[head] = num;                      //the critical section, we filled buffer with [0,0,0]
   head = (head + 1) % BUF_SIZE;
   xSemaphoreGive(mutex);              // RELEASE MUTEX


   // Signal to consumer tasks that a slot in the buffer has been filled
   xSemaphoreGive(sem_filled);
 }


 // Delete self task
 vTaskDelete(NULL);
}


// Consumer: continuously read from shared buffer
void consumer(void *parameters) {


 int val;


 // Read from buffer
 while (1) {


   // Wait for at least one slot in buffer to be filled
   xSemaphoreTake(sem_filled, portMAX_DELAY);


   // Lock critical section with a mutex
   xSemaphoreTake(mutex, portMAX_DELAY);
   val = buf[tail];
   tail = (tail + 1) % BUF_SIZE;
   Serial.println(val);
   xSemaphoreGive(mutex);


   // Signal to producer thread that a slot in the buffer is free
   xSemaphoreGive(sem_empty);
 }
}


//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)


void setup() {


 char task_name[12]; //this is gonna be the name of each task 0, 1, 2 , 3 , 4
  // Configure Serial
 Serial.begin(115200);


 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Semaphore Solution---");


 // Create mutexes and semaphores before starting tasks
 bin_sem = xSemaphoreCreateBinary();  //intializes at 0
 mutex = xSemaphoreCreateMutex();  //mutex is initialized to 1 by default (meaning not taken)
 sem_empty = xSemaphoreCreateCounting(BUF_SIZE, BUF_SIZE);  //creates counting semaphore (5, 5). first numberi s max count of semaphore, second is where it starts, so we can only take
 sem_filled = xSemaphoreCreateCounting(BUF_SIZE, 0); // (5, 0), buf size is 5, initial value is 0, so we can only give until it's full


 // Start producer tasks (wait for each to read argument)
 for (int i = 0; i < num_prod_tasks; i++) {
   sprintf(task_name, "Producer %i", i);   //stores the string "Producer 0" in buffer array task_name (task_name[14] = "Producer 0")
   xTaskCreatePinnedToCore(producer,
                           task_name,
                           1024,
                           (void *)&i,  //passing 0 to function
                           1,
                           NULL,
                           app_cpu);
   xSemaphoreTake(bin_sem, portMAX_DELAY);  //takes binary semaphore
 }


 // Start consumer tasks
 for (int i = 0; i < num_cons_tasks; i++) {
   sprintf(task_name, "Consumer %i", i);
   xTaskCreatePinnedToCore(consumer,
                           task_name,
                           1024,
                           NULL,
                           1,
                           NULL,
                           app_cpu);
 }


 // Notify that all tasks have been created (lock Serial with mutex)
 xSemaphoreTake(mutex, portMAX_DELAY);
 Serial.println("All tasks created");
 xSemaphoreGive(mutex);
}


void loop() {


 // Do nothing but allow yielding to lower-priority tasks
 vTaskDelay(1000 / portTICK_PERIOD_MS);
}
