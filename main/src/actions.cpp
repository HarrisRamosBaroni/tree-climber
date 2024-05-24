#include "actions.h"

void gripper(int servo_num, int action, int ms) {
  /*
    action = 0 -> stop, action = 1 -> open, action = 2 -> close
    servo_num = 1 -> top, servo_num = 2 -> bottom
    servo_num = -1 -> both top and bottom
  */

  //int pulse = action ? 1650 : 1750; // deadzone at 1700
  int pulse = !action ? WINCHDEADZONE : ((action == ACTION_OPEN) ? (WINCHDEADZONE - 50) : (WINCHDEADZONE + 50)); 
  if (servo_num == BOTH_NUM) {
    board1.writeMicroseconds(TOP_NUM, pulse);
    board1.writeMicroseconds(BOTTOM_NUM, pulse);
    delay(ms);
    board1.writeMicroseconds(TOP_NUM, WINCHDEADZONE);
    board1.writeMicroseconds(BOTTOM_NUM, WINCHDEADZONE);
  } else {
    board1.writeMicroseconds(servo_num, pulse);
    delay(ms);
    board1.writeMicroseconds(servo_num, WINCHDEADZONE);
  }
}

void vertical_move(int direction, int ms) {
  /*
    direction = 0 -> stop
    direction = 1 -> bottom gripper moves up w.r.t top gripper
    direction = 2 -> bottom gripper moves down w.r.t top gripper
  */
 
  int pulse = !direction ? RACKDEADZONE : ((direction == DIR_DOWN) ? (RACKDEADZONE - 50) : (RACKDEADZONE + 50)); 
  board1.writeMicroseconds(RACKNPIN_NUM, pulse);
  delay(ms);
  board1.writeMicroseconds(RACKNPIN_NUM, RACKDEADZONE);
}

void horizontal_move(int direction, int ms) {
  /*
    direction = 0 -> stop, direction = 1 -> left, direction = 2 -> right
    ms = time in milliseconds to move in the given direction
    ms = -1 -> move indefinitely in the given direction
  */
 
  int pulse = !direction ? WHEELDEADZONE : ((direction == DIR_LEFT) ? (WHEELDEADZONE - 100) : (WHEELDEADZONE + 100)); 
  board1.writeMicroseconds(WHEEL1_NUM, pulse);
  board1.writeMicroseconds(WHEEL2_NUM, pulse);
  if (ms == -1) return;
  delay(ms);
  board1.writeMicroseconds(WHEEL1_NUM, WHEELDEADZONE);
  board1.writeMicroseconds(WHEEL2_NUM, WHEELDEADZONE);
}

void init_noboru() {
  /*
    Initialise the robot noboru to grab the tree trunk.
    You have 5 seconds to grab the tree 
    trunk once both grippers are open.
  */
  gripper(BOTH_NUM, ACTION_OPEN,  15000); // 15 seconds to open both grippers
  gripper(BOTH_NUM, ACTION_STOP,  5000);  // 5 seconds still
  gripper(BOTH_NUM, ACTION_CLOSE, 15000); // 15 seconds to close both grippers
}

void stepup() {
  /*
    A single step climb up the tree.
    Only activate after noboru is initialised.
  */
  gripper(TOP_NUM, ACTION_OPEN,  20000);  
  // gripper(TOP_NUM, ACTION_STOP,  5000); //get rid of this line?
  vertical_move(DIR_DOWN, 20000);
  gripper(TOP_NUM, ACTION_CLOSE, 20000);

  gripper(BOTTOM_NUM, ACTION_OPEN,  20000);  
  // gripper(BOTTOM_NUM, ACTION_STOP,  5000); //get rid of this line?
  vertical_move(DIR_UP, 20000);
  gripper(BOTTOM_NUM, ACTION_CLOSE, 20000);
}

void stepdown() {
  // A single step downwards on tree.
  gripper(BOTTOM_NUM, ACTION_OPEN,  20000);  
  // gripper(BOTTOM_NUM, ACTION_STOP,  5000); //get rid of this line?
  vertical_move(DIR_DOWN, 20000);
  gripper(BOTTOM_NUM, ACTION_CLOSE, 20000);

  gripper(TOP_NUM, ACTION_OPEN,  20000);  
  gripper(TOP_NUM, ACTION_STOP,  5000); //get rid of this line?
  // vertical_move(DIR_UP, 20000);
  gripper(TOP_NUM, ACTION_CLOSE, 20000);
}

float get_distance() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Set the trigPin on HIGH state for 10 micro seconds to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin (returns the sound wave travel time in microseconds)
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance
  distanceCm = duration * SOUND_SPEED/2;
  return distanceCm;
}

void scan() {
  // Scan the tree for branches.
}

void avoid_branch() {
  // Move the robot to avoid a branch.
  //scan()
  //horizontal_move(DIR_LEFT, 5000);
  //horizontal_move(DIR_RIGHT, 5000);
}

void climb() {
  // Climb the tree (no branch avoidance)
  // Must be called after init_noboru()
  // Simply step up until the distance is less than 15cm

  while (get_distance() > 15) {
    stepup();
    actions_taken[actions_taken_idx++] = 9; // 9 -> stepup. post increment index
  }
}

void branch_climb() {
  // Climb the tree while avoiding branches.
  // init_noboru();
  // while (get_distance() > 15) {
  //   stepup();
  //   avoid_branch();
  // }
}

void descend() {
  // Descend the tree.
  // Retrace steps taken to climb the tree.
  for (actions_taken_idx; actions_taken_idx >= 0; actions_taken_idx--) {
    switch (actions_taken[actions_taken_idx]) {
      case 6:
          horizontal_move(DIR_LEFT, cooldownms);
          break;
      case 7:
          horizontal_move(DIR_RIGHT, cooldownms);
          break;
      case 9:
          stepup();
          break;
      case 10:
          stepdown();
          break;
      default:
          // delay(2000);
          break;
    }
  }
}