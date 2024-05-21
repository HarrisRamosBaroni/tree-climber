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
#define ACTION_STOP 0
#define ACTION_OPEN 1
#define ACTION_CLOSE 2
#define DIR_UP 2
#define DIR_DOWN 1
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define WINCHDEADZONE 1700 + 20
#define RACKDEADZONE 1130 + 10
#define WHEELDEADZONE 1685 

void gripper(int servo_num, int action, int ms);
void vertical_move(int direction, int ms);
void horizontal_move(int direction, int ms);
void init_noboru();
void stepup();
void stepdown();
void scan();
void avoid_branch();

#endif