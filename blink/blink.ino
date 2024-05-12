#include <Arduino.h>

const int LED_PIN = 2; // Pin number for the built-in LED on the ESP32 board

void setup() {
    pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output
}

void loop() {
    digitalWrite(LED_PIN, HIGH); // Turn on the LED
    delay(1000); // Wait for 1 second
    digitalWrite(LED_PIN, LOW); // Turn off the LED
    delay(1000); // Wait for 1 second
}