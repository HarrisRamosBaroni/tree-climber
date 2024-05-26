# 1 "/Users/harris/Documents/y1/final_project/main/main.ino"
# 2 "/Users/harris/Documents/y1/final_project/main/main.ino" 2
# 3 "/Users/harris/Documents/y1/final_project/main/main.ino" 2
# 4 "/Users/harris/Documents/y1/final_project/main/main.ino" 2
# 12 "/Users/harris/Documents/y1/final_project/main/main.ino"
const int LED_PIN = 2; // Pin number for the built-in LED on the ESP32 board

ControllerPtr myControllers[
# 14 "/Users/harris/Documents/y1/final_project/main/main.ino" 3 4
                           4
# 14 "/Users/harris/Documents/y1/final_project/main/main.ino"
                                            ];
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

int action_executing = 0;
int cooldownms = 10000;
int shortcooldownms = 3000;
int action = -1;
TaskHandle_t actionTask;

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < 
# 27 "/Users/harris/Documents/y1/final_project/main/main.ino" 3 4
                       4
# 27 "/Users/harris/Documents/y1/final_project/main/main.ino"
                                        ; i++) {
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

    for (int i = 0; i < 
# 48 "/Users/harris/Documents/y1/final_project/main/main.ino" 3 4
                       4
# 48 "/Users/harris/Documents/y1/final_project/main/main.ino"
                                        ; i++) {
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
        ctl->axisX(), // (-511 - 512) left X Axis
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
    board1.setPWMFreq(50); //(60);  // Analog servos run at ~60 Hz updates
    // Setup LED pin
    pinMode(LED_PIN, 0x03); // Set the LED pin as an output

    while (1){
        if (action != -1){
            Serial.print("action beginning in core ");
            Serial.println(xPortGetCoreID());
            action_executing = 1;
            digitalWrite(LED_PIN, 0x1); // Turn on the LED
        }
        switch (action){
            case 0:
                Serial.println("open top gripper in core 0");
                gripper(1, 1, shortcooldownms);
                break;
            case 1:
                Serial.println("close top gripper in core 0");
                gripper(1, 2, shortcooldownms);
                break;
            case 2:
                Serial.println("open bottom gripper in core 0");
                gripper(2, 1, shortcooldownms);
                break;
            case 3:
                Serial.println("close bottom gripper in core 0");
                gripper(2, 2, shortcooldownms);
                break;
            case 4:
                Serial.println("move rack and pinion up in core 0");
                vertical_move(2, cooldownms);
                break;
            case 5:
                Serial.println("move rack and pinion down in core 0");
                vertical_move(1, cooldownms);
                break;
            case 6:
                Serial.println("move robot left in core 0");
                horizontal_move(1, cooldownms);
                break;
            case 7:
                Serial.println("move robot right in core 0");
                horizontal_move(2, cooldownms);
                break;
            case 8:
                Serial.println("initiate noboru in core 0");
                init_noboru();
                break;
            case 9:
                Serial.println("step up in core 0");
                stepup();
                break;
            case 10:
                Serial.println("step down in core 0");
                stepdown();
                break;
            case 11:
                Serial.println("big both grab in core 0");
                gripper(-1, 1, cooldownms);
                break;
            case 12:
                Serial.println("big both ungrab in core 0");
                gripper(-1, 2, cooldownms);
                break;
            default:
                Serial.println("default in core 0");
                // delay(2000);
                break;
        }
        if (action != -1){
            Serial.println("action ended in core 0");
            action_executing = 0;
            digitalWrite(LED_PIN, 0x0); // Turn off the LED
        }
        // action = -1; // reset action after execution of current action
        // delay(2000);
        vTaskDelay(4000 / ( ( TickType_t ) 1000 / ( 
# 160 "/Users/harris/Documents/y1/final_project/main/main.ino" 3 4
                         1000 
# 160 "/Users/harris/Documents/y1/final_project/main/main.ino"
                         ) )); // Yield control for 4000 ms
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
            case 0x0008 + 0x0010:
                Serial.println("core 1 cmd: open top gripper");
                action = 0;
                break;
            case 0x0008 + 0x0020:
                Serial.println("core 1 cmd: close top gripper");
                action = 1;
                break;
            case 0x0001 + 0x0010:
                Serial.println("core 1 cmd: open bottom gripper");
                action = 2;
                break;
            case 0x0001 + 0x0020:
                Serial.println("core 1 cmd: close bottom gripper");
                action = 3;
                break;
            case 0x0004 + 0x0010:
                Serial.println("core 1 cmd: move rack and pinion up");
                action = 4;
                break;
            case 0x0004 + 0x0020:
                Serial.println("core 1 cmd: move rack and pinion down");
                action = 5;
                break;
            case 0x0002 + 0x0010:
                Serial.println("core 1 cmd: move robot left");
                action = 6;
                break;
            case 0x0002 + 0x0020:
                Serial.println("core 1 cmd: move robot right");
                action = 7;
                break;
            case 0x0100:
                Serial.println("core 1 cmd: initiate noboru");
                action = 8;
                break;
            case 0x0008 + 0x0100:
                Serial.println("core 1 cmd: step up");
                action = 9;
                break;
            case 0x0001 + 0x0100:
                Serial.println("core 1 cmd: step down");
                action = 10;
                break;
            case 0x0004 + 0x0100:
                Serial.println("core 1 cmd: big both grab");
                action = 11;
                break;
            case 0x0002 + 0x0100:
                Serial.println("core 1 cmd: big both ungrab");
                action = 12;
                break;
            default:
                Serial.println("default in core 1");
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
    //Serial.begin(9600);
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
    xTaskCreatePinnedToCore(executeAction, "executeAction", 4096, 
# 277 "/Users/harris/Documents/y1/final_project/main/main.ino" 3 4
                                                                 __null
# 277 "/Users/harris/Documents/y1/final_project/main/main.ino"
                                                                     , 1, &actionTask, 0);
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    vTaskDelay(1);
    delay(150);
}
