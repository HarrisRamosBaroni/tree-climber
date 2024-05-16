# 1 "/Users/harris/Documents/y1/final_project/ultrasonic_distance/ultrasonic_distance.ino"
/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-hc-sr04-ultrasonic-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS



long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, 0x03); // Sets the trigPin as an Output
  pinMode(echoPin, 0x01); // Sets the echoPin as an Input
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, 0x0);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, 0x1);
  delayMicroseconds(10);
  digitalWrite(trigPin, 0x0);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, 0x1);

  // Calculate the distance
  distanceCm = duration * 0.034/2;

  // Convert to inches
  distanceInch = distanceCm * 0.393701;

  // Prints the distance in the Serial Monitor
  //Serial.print("Distance (cm): ");
  //Serial.println(distanceCm);
  // Serial.print("Distance (inch): ");
  // Serial.println(distanceInch);

  delay(10);
}
