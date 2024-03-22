/*
In this program we’ll use Arduino’s built-in Hardware Timer, as an interrupt. 
*/

#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint16_t timer_divider = 80; // Clock ticks at 1 Mhz now
static const uint64_t timer_max_count = 1000000;

// Pins
static const int led_pin = 14;

// Globals, using a timer handle. For this we’ll use the “Hal” timer that comes with the arduino package library
static hw_timer_t *timer = NULL;

// ***************************************************************
// Interrupt Service Routines (ISRs)

void IRAM_ATTR onTimer()
{

 // Toggle LED
 int pin_state = digitalRead(led_pin);
 digitalWrite(led_pin, !pin_state);

}


void setup()
{
 // Configure LED pin
 pinMode(led_pin, OUTPUT);

 // Create and start timer (num, divider, countUp) . we initialize hardware Timer 0
 timer = timerBegin(0, timer_divider, true);


 // Provider ISR to timer (timer, function, edge)
 timerAttachInterrupt(timer, &onTimer, true);


 // At what count should ISR trigger (timer, count, autoreload)
 timerAlarmWrite(timer, timer_max_count, true);


  // Allow ISR to trigger
 timerAlarmEnable(timer);
}

void loop()
{

}
