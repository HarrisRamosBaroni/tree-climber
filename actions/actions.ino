// Original library sourse: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// The default I2C address of the PCA9685 board is 0x40
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

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
#define WINCHDEADZONE 1700 + 20
#define RACKDEADZONE 1130
#define WHEELDEADZONE 1700 // TODO: Find the correct deadzone for the wheels

void setup() {
  Serial.begin(9600);
  Serial.println("32 channel Servo test!");

  board1.begin();
  board1.setPWMFreq(50); //(60);  // Analog servos run at ~60 Hz updates
  //yield();
}

void gripper(int servo_num, int action, int ms) {
  /*
    action = 0 -> stop, action = 1 -> open, action = 2 -> close
    servo_num = 1 -> top, servo_num = 2 -> bottom
    servo_num = -1 -> both top and bottom
  */

  //int pulse = action ? 1650 : 1750; // deadzone at 1700
  int pulse = !action ? WINCHDEADZONE : ((action == 1) ? (WINCHDEADZONE - 50) : (WINCHDEADZONE + 50)); 
  if (servo_num == BOTH_NUM) {
    board1.writeMicroseconds(TOP_NUM, pulse);
    board1.writeMicroseconds(BOTTOM_NUM, pulse);
  } else {
    board1.writeMicroseconds(servo_num, pulse);
  }
  delay(ms);
}

void vertical_move(int direction, int ms) {
  /*
    direction = 0 -> stop
    direction = 1 -> bottom gripper moves up w.r.t top gripper
    direction = 2 -> bottom gripper moves down w.r.t top gripper
  */
 
  //float pulse = direction ? 1100 : 1160; // deadzone at 1130
  int pulse = !direction ? RACKDEADZONE : ((direction == 1) ? (RACKDEADZONE - 30) : (RACKDEADZONE + 30)); 
  board1.writeMicroseconds(RACKNPIN_NUM, pulse);
  delay(ms);
}

// TODO: Obtain the correct pulse width for horizontal movement
void horizontal_move(int direction, int ms) {
  /*
    direction = 0 -> stop, direction = 1 -> left, direction = 2 -> right
    ms = time in milliseconds to move in the given direction
    ms = -1 -> move indefinitely in the given direction
  */
 
  //int pulse = action ? 1650 : 1750; // deadzone at 1700
  int pulse = !direction ? WHEELDEADZONE : ((direction == 1) ? (WHEELDEADZONE - 50) : (WHEELDEADZONE + 50)); 
  board1.writeMicroseconds(WHEEL1_NUM, pulse);
  board1.writeMicroseconds(WHEEL2_NUM, pulse);
  if (ms == -1) return;
  delay(ms);
}

void init_noboru() {
  /*
    Initialise the robot noboru to grab the tree trunk.
    You have 10 seconds to grab the tree 
    trunk once both grippers are open.
  */
  gripper(BOTH_NUM, OPEN,  10000); // 10 seconds to open both grippers
  gripper(BOTH_NUM, STOP,  5000);  // 5 seconds still
  gripper(BOTH_NUM, CLOSE, 10000); // 10 seconds to close both grippers
}

void stepup() {
  /*
    A single step climb up the tree.
    Only activate after noboru is initialised.
  */
  gripper(TOP_NUM, OPEN,  5000);  
  gripper(TOP_NUM, STOP,  5000);
  vertical_move(DOWN, 8000);
  gripper(TOP_NUM, CLOSE, 5000);

  gripper(BOTTOM_NUM, OPEN,  5000);  
  gripper(BOTTOM_NUM, STOP,  5000);
  vertical_move(UP, 8000);
  gripper(BOTTOM_NUM, CLOSE, 5000);
}

void stepdown() {
  // A single step downwards on tree.
  gripper(BOTTOM_NUM, OPEN,  5000);  
  gripper(BOTTOM_NUM, STOP,  5000);
  vertical_move(DOWN, 8000);
  gripper(BOTTOM_NUM, CLOSE, 5000);

  gripper(TOP_NUM, OPEN,  5000);  
  gripper(TOP_NUM, STOP,  5000);
  vertical_move(UP, 8000);
  gripper(TOP_NUM, CLOSE, 5000);
}

void scan() {
  // Scan the tree for branches.
}

void avoid_branch() {
  // Move the robot to avoid a branch.
  //scan()
  //horizontal_move(LEFT, 5000);
  //horizontal_move(RIGHT, 5000);
}

void loop() {
}