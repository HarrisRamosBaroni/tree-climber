#include "src/actions.h"
#include <Bluepad32.h>
#include <Arduino.h>

#define BTN_Y 0x0008
#define BTN_LFT_TRGR 0x0010
#define BTN_RGHT_TRGR 0x0020
#define BTN_A 0x0001
#define BTN_B 0x0004
#define BTN_X 0x0002
#define BTN_JYSTK 0x0100
const int LED_PIN = 2; // Pin number for the built-in LED on the ESP32 board

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

int action_executing = 0;
int action = -1;
TaskHandle_t actionTask;

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        //"idx=%d, dpad: 0x%02x, buttons: 0x%04x, misc: 0x%02x\n",
        "Lx: %4d, Ly: %4d\n",
        //"buttons: 0x%05x\n",
        //"Lx: %4d, Ly: %4d, Rx: %4d, Ry: %4d, brake: %4d, throttle: %4d\n",
        //ctl->index(),        // Controller Index
        //ctl->dpad(),         // D-pad
        //ctl->buttons()//,      // bitmask of pressed buttons
        //ctl->miscButtons(),   // bitmask of pressed "misc" buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY()//,        // (-511 - 512) left Y axis
        //ctl->axisRX(),       // (-511 - 512) right X axis
        //ctl->axisRY(),       // (-511 - 512) right Y axis
        //ctl->brake(),        // (0 - 1023): brake button
        //ctl->throttle()      // (0 - 1023): throttle (AKA gas) button
    );
}

void executeAction(void *param) {
    // Setup the PWM controller
    board1.begin();
    board1.setPWMFreq(50); //(60);  // Analog servos run at ~50 Hz updates
    // Setup LED pin
    pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output

    while (1){
        if (action != -1){
            // Serial.print("action beginning in core ");
            // Serial.println(xPortGetCoreID());
            action_executing = 1;
            digitalWrite(LED_PIN, HIGH); // Turn on the LED
            // actions_taken[actions_taken_idx] = action;
            // actions_taken_idx++;
        }
        switch (action){
            case 0:
                // Serial.println("open top gripper in core 0");
                gripper(TOP_NUM, ACTION_OPEN, shortcooldownms);
                break;
            case 1:
                // Serial.println("close top gripper in core 0");
                gripper(TOP_NUM, ACTION_CLOSE, shortcooldownms);
                break;
            case 2:
                // Serial.println("open bottom gripper in core 0");
                gripper(BOTTOM_NUM, ACTION_OPEN, shortcooldownms);
                break;
            case 3:
                // Serial.println("close bottom gripper in core 0");
                gripper(BOTTOM_NUM, ACTION_CLOSE, shortcooldownms);
                break;
            case 4:
                // Serial.println("move rack and pinion up in core 0");
                vertical_move(DIR_UP, cooldownms);
                break;
            case 5:
                // Serial.println("move rack and pinion down in core 0");
                vertical_move(DIR_DOWN, cooldownms);
                break;
            case 6:
                // Serial.println("move robot left in core 0");
                horizontal_move(DIR_LEFT, cooldownms);
                break;
            case 7:
                // Serial.println("move robot right in core 0");
                horizontal_move(DIR_RIGHT, cooldownms);
                break;
            case 8:
                // Serial.println("initiate noboru in core 0");
                init_noboru();
                break;
            case 9:
                // Serial.println("step up in core 0");
                stepup();
                break;
            case 10:
                // Serial.println("step down in core 0");
                stepdown();
                break;
            case 11:
                // Serial.println("big both grab in core 0");
                gripper(BOTH_NUM, ACTION_OPEN, cooldownms);
                break;
            case 12:
                // Serial.println("big both ungrab in core 0");
                gripper(BOTH_NUM, ACTION_CLOSE, cooldownms);
                break;
            default:
                // Serial.println("default in core 0");
                // delay(2000);
                break;
        }
        if (action != -1){
            // Serial.println("action ended in core 0");
            action_executing = 0;
            digitalWrite(LED_PIN, LOW); // Turn off the LED
        }
        // action = -1; // reset action after execution of current action
        // delay(2000);
        vTaskDelay(4000 / portTICK_PERIOD_MS);  // Yield control for 4000 ms
    }
}

void processGamepad(ControllerPtr ctl) {
    if (!action_executing) {
        int buttons_pressed = ctl->buttons();
        // Query controller data by getting the buttons() function.
        //dumpGamepad(ctl);

        // int joyX = ctl->axisX();
        // if (joyX < -200) {
        //     buttons_pressed = BTN_X + BTN_LFT_TRGR;
        // } else if (joyX > 200) {
        //     buttons_pressed = BTN_X + BTN_RGHT_TRGR;
        // }
        action_executing = 1;
        switch (buttons_pressed) {
            case BTN_Y + BTN_LFT_TRGR:
                // Serial.println("core 1 cmd: open top gripper");
                action = 0;
                break;
            case BTN_Y + BTN_RGHT_TRGR:
                // Serial.println("core 1 cmd: close top gripper");
                action = 1;
                break;
            case BTN_A + BTN_LFT_TRGR:
                // Serial.println("core 1 cmd: open bottom gripper");
                action = 2;
                break;
            case BTN_A + BTN_RGHT_TRGR:
                // Serial.println("core 1 cmd: close bottom gripper");
                action = 3;
                break;
            case BTN_B + BTN_LFT_TRGR:
                // Serial.println("core 1 cmd: move rack and pinion up");
                action = 4;
                break;
            case BTN_B + BTN_RGHT_TRGR:
                // Serial.println("core 1 cmd: move rack and pinion down");
                action = 5;
                break;
            case BTN_X + BTN_LFT_TRGR:
                // Serial.println("core 1 cmd: move robot left");
                action = 6;
                break;
            case BTN_X + BTN_RGHT_TRGR:
                // Serial.println("core 1 cmd: move robot right");
                action = 7;
                break;
            case BTN_JYSTK:
                // Serial.println("core 1 cmd: initiate noboru");
                action = 8;
                break;
            case BTN_Y + BTN_JYSTK:
                // Serial.println("core 1 cmd: step up");
                action = 9;
                break;
            case BTN_A + BTN_JYSTK:
                // Serial.println("core 1 cmd: step down");
                action = 10;
                break;
            case BTN_B + BTN_JYSTK:
                // Serial.println("core 1 cmd: big both grab");
                action = 11;
                break;
            case BTN_X + BTN_JYSTK:
                // Serial.println("core 1 cmd: big both ungrab");
                action = 12;
                break;
            default:
                // Serial.println("default in core 1");
                action = -1;
                action_executing = 0;
                break;
        }
    }
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But it might also fix some connection / re-connection issues.
    BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);

    // Core 0 task to execute the actions with PWM controller
    xTaskCreatePinnedToCore(executeAction, "executeAction", 4096, NULL, 1, &actionTask, 0); 

    // Setup the ultrasonic sensor
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    // Fetch the data from all controllers connected to the ESP32
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    vTaskDelay(1); // yield to lower priority task
    delay(150);
}