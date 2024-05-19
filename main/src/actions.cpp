#include "src/actions.h"

void gripper(int servo_num, int action, int ms) {
  /*
    action = 0 -> stop, action = 1 -> open, action = 2 -> close
    servo_num = 1 -> top, servo_num = 2 -> bottom
    servo_num = -1 -> both top and bottom
  */

  //int pulse = action ? 1650 : 1750; // deadzone at 1700
  int pulse = !direction ? WINCHDEADZONE : ((direction == 1) ? 1650 : 1750); 
  if (servo_num == BOTH_NUM) {
    board1.setPWM(TOP_NUM, 0, pulse);
    board1.setPWM(BOTTOM_NUM, 0, pulse);
  } else {
    board1.setPWM(servo_num, 0, pulse);
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
  int pulse = !direction ? RACKDEADZONE : ((direction == 1) ? 1100 : 1160); 
  board1.setPWM(RACKNPIN_NUM, 0, pulse);
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
  int pulse = !direction ? WHEELDEADZONE : ((direction == 1) ? 1650 : 1750); 
  board1.setPWM(WHEEL1_NUM, 0, pulse);
  board1.setPWM(WHEEL2_NUM, 0, pulse);
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