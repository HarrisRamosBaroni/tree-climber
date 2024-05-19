#ifndef ACTIONS_H
#define ACTIONS_H

// Original library sourse: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

extern Adafruit_PWMServoDriver board1;

#define RACKNPIN_NUM 0
#define TOP_NUM 1
#define BOTTOM_NUM 2
#define BOTH_NUM -1
#define WHEEL1_NUM 3
#define WHEEL2_NUM 4
#define STOP 0
#define OPEN 1
#define CLOSE 2
#define UP 1
#define DOWN 2
#define LEFT 1
#define RIGHT 2
#define WINCHDEADZONE 1700
#define RACKDEADZONE 1130
#define WHEELDEADZONE 1700 // TODO: Find the correct deadzone for the wheels

void gripper(int servo_num, int action, int ms);
void vertical_move(int direction, int ms);
void horizontal_move(int direction, int ms);
void init_noboru();
void stepup();
void stepdown();
void scan();
void avoid_branch();

#endif