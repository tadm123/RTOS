/*
This is a binary semaphore demo.
In here we’ll type a number as input, it’ll pass a number to the function, and then write it on the LED. 
The semaphore will be used so that the function can run and finish it, and then the setup() can finish.
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
//static int shared_var = 0;
static SemaphoreHandle_t bin_sem;  //declare our global mutex. Note that FreeRTOs generalizes mutex and semaphroes as just semaphores

static const int led_pin = 14;



// Blink LED based on rate passed by parameter
void blinkLED(void *parameters) {
  
 // Copy the parameter into a local variable
 int num = *(int *)parameters;


 // Release the binary semaphore so that the creating function can finish. Using Give (producer) we add one to the Semaphore (semaphore now at 1)
 xSemaphoreGive(bin_sem);


 // Print the parameter
 Serial.print("Received: ");
 Serial.println(num);


 // Configure the LED pin
 pinMode(led_pin, OUTPUT);


 // Blink forever and ever
 while (1) {
   digitalWrite(led_pin, HIGH);
   vTaskDelay(num / portTICK_PERIOD_MS);
   digitalWrite(led_pin, LOW);
   vTaskDelay(num / portTICK_PERIOD_MS);
 }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {


long int delay_arg;


 // Configure Serial
 Serial.begin(115200);


 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Semaphore Solution---");
 Serial.println("Enter a number for delay (milliseconds)");


 // Wait for input from Serial
 while (Serial.available() <= 0);


 // Read integer value
 delay_arg = Serial.parseInt();
 Serial.print("Sending: ");
 Serial.println(delay_arg);


  // Create mutex before starting tasks
 bin_sem = xSemaphoreCreateBinary();  //initialized to 0


// Note we don’t need to add a SemaphoreTake(), in this place, since the semaphore already initializes it to 0 


 // Start task 1
 xTaskCreatePinnedToCore(blinkLED,
                         "Blink LED",
                         1024,
                         (void *)&delay_arg,
                         1,
                         NULL,
                         app_cpu);

 // Do nothing until mutex has been returned (maximum delay)
 xSemaphoreTake(bin_sem, portMAX_DELAY);

 // Show that we accomplished our task of passing the stack-based argument
 Serial.println("Done!");
}


void loop() {
  // Do nothing but allow yielding to lower-priority tasks
 vTaskDelay(1000 / portTICK_PERIOD_MS);
}
