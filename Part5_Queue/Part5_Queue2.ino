/*
One task performs basic echo on Serial. If it sees "delay" followed by a
number, it sends the number (in a queue) to the second task. If it receives
a message in a second queue, it prints it to the console. The second task
blinks an LED. When it gets a message from the first queue (number), it
updates the blink delay to that number. Whenever the LED blinks 100 times,
the second task sends a message to the first task to be printed.
*/



// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t buf_len = 255;     // Size of buffer to look for command
static const char command[] = "delay "; // Note the space
static const int delay_queue_len = 5;   // Size of delay_queue
static const int msg_queue_len = 5;     // Size of msg_queue
static const uint8_t blink_max = 100;   // Num times to blink before message

// Pins
static const int led_pin = 14;

typedef struct Message {
 char body[20];
 int count;
} Message;


// Globals
static QueueHandle_t delay_queue;  //queue for our Task 1
static QueueHandle_t msg_queue;


//*****************************************************************************
// Tasks


// Task: command line interface (CLI). This reads serial input and if "delay xxx" sends it into Queue 'delay_queue'
void doCLI(void *parameters) {

 Message rcv_msg;  //initializing struct
 char c;
 char buf[buf_len];
 uint8_t idx = 0;
 uint8_t cmd_len = strlen(command);  
 int led_delay;

 // Clear whole buffer
 memset(buf, 0, buf_len);

 // Loop forever
 while (1) {
   
   // See if there's a message in the queue (do not block)
   if (xQueueReceive(msg_queue, (void *)&rcv_msg, 0) == pdTRUE) {
     Serial.print(rcv_msg.body);
     Serial.println(rcv_msg.count);
   }

   // Read characters from serial
   if (Serial.available() > 0) {
     c = Serial.read();

     
     // Store received character to buffer if not over buffer limit
     if (idx < buf_len - 1) {
       buf[idx] = c;
       idx++;
     }


     // Print newline and check input on 'enter'
     if ((c == '\n') || (c == '\r')) {

       // Print newline to terminal
       Serial.print("\r\n");

       // Check if the first 6 characters are "delay " (remember the string command[] = "delay ")
       if (memcmp(buf, command, cmd_len) == 0) {  //comparing buf with command strings, if they're

         // Convert last part to positive integer (negative int crashes)
         char* tail = buf + cmd_len;   // initializing char pointer, pointing to number part of the buf array (ex "200"). Array decays into pointer then simple pointer arithmetic to move pointer
         led_delay = atoi(tail);    // converts string to number d_delay = 200             
         led_delay = abs(led_delay); // always will convert it to positive, despite you putting negative umbers

         // Send integer to other task via queue, sending led_delay integer number into delay_queue Queue
         if (xQueueSend(delay_queue, (void *)&led_delay, 10) != pdTRUE) {
           Serial.println("ERROR: Could not put item on delay queue.");
         }
       }


       // Reset receive buffer and index counter
       memset(buf, 0, buf_len);
       idx = 0;

     // Otherwise, echo character back to serial terminal
     } else {
       Serial.print(c);
     }
   }
 }
}

// Task: flash LED based on delay provided, notify other task every 100 blinks
void blinkLED(void *parameters) {

 Message msg;
 int led_delay = 500;
 uint8_t counter = 0;

 // Set up pin
 pinMode(led_pin, OUTPUT);

 // Loop forever
 while (1) {

   // See if there's a message in the queue (do not block)
   if (xQueueReceive(delay_queue, (void *)&led_delay, 0) == pdTRUE) {


     // Best practice: use only one task to manage serial comms
     strcpy(msg.body, "Message received ");
     msg.count = 1;
     xQueueSend(msg_queue, (void *)&msg, 10);
   }


   // Blink
   digitalWrite(led_pin, HIGH);
   vTaskDelay(led_delay / portTICK_PERIOD_MS);
   digitalWrite(led_pin, LOW);
   vTaskDelay(led_delay / portTICK_PERIOD_MS);


   // If we've blinked 100 times, send a message to the other task
   counter++;
   if (counter >= blink_max) {
    
     // Construct message and send
     strcpy(msg.body, "Blinked: ");
     msg.count = counter;
     xQueueSend(msg_queue, (void *)&msg, 10);


     // Reset counter
     counter = 0;
   }
 }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

 // Configure Serial
 Serial.begin(115200);

 // Wait a moment to start (so we don't miss Serial output)
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 Serial.println();
 Serial.println("---FreeRTOS Queue Solution---");
 Serial.println("Enter the command 'delay xxx' where xxx is your desired ");
 Serial.println("LED blink delay time in milliseconds");

 // Create queues
 delay_queue = xQueueCreate(delay_queue_len, sizeof(int));
 msg_queue = xQueueCreate(msg_queue_len, sizeof(Message));


 // Start CLI task
 xTaskCreatePinnedToCore(doCLI,
                         "CLI",
                         1200,
                         NULL,
                         1,
                         NULL,
                         app_cpu);


 // Start blink task
 xTaskCreatePinnedToCore(blinkLED,
                         "Blink LED",
                         1200,
                         NULL,
                         1,
                         NULL,
                         app_cpu);

 // Delete "setup and loop" task
 vTaskDelete(NULL);
}

void loop() {
 // Execution should never get here
}
