#include "esp32-hal-ledc.h"

#define SERVO_PIN 22  // GPIO pin connected to the servo
#define SERVO_CHANNEL 1
#define SERVO_MIN_PULSE_WIDTH 500  // Min pulse width for the servo (in microseconds)
#define SERVO_MAX_PULSE_WIDTH 2400 // Max pulse width for the servo (in microseconds)
#define SERVO_FREQUENCY 50          // Servo PWM frequency (in Hz)
#define TIMER_WIDTH 16              // Width of the timer (16 bits)

void setup() {
   ledcSetup(SERVO_CHANNEL, SERVO_FREQUENCY, TIMER_WIDTH); // Setup PWM channel for the servo
   ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);   // Attach the servo to the GPIO pin
}

void loop() {
   // Sweep the servo from 0 to 360 degrees
   for (int angle = 0; angle <= 360; angle += 10) {
      int pulse_width = map(angle, 0, 360, SERVO_MIN_PULSE_WIDTH, SERVO_MAX_PULSE_WIDTH);
      ledcWrite(SERVO_CHANNEL, pulse_width); // Set the pulse width for the servo
      delay(200); // Delay to allow the servo to reach the desired position
   }
}
