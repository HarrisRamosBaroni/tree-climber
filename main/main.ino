#include "src/actions.h"
#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

int process_running = 0;
int cooldownms = 0;

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

void processGamepad(ControllerPtr ctl) {
    // Query controller data by getting the buttons() function.
    //dumpGamepad(ctl);

    // Y button relates to top gripper
    // press left trigger simultaneously to open gripper
    // press right trigger simultaneously to close gripper
    if (ctl->buttons() == 0x0018) {
        Serial.println("open top gripper");
        cooldownms = 10000;
        gripper(TOP_NUM, OPEN, cooldownms);
    } else if (ctl->buttons() == 0x0028) {
        Serial.println("close top gripper");
        cooldownms = 10000;
        gripper(TOP_NUM, CLOSE, cooldownms);
    }

    // A button relates to bottom gripper
    // press left trigger simultaneously to open gripper
    // press right trigger simultaneously to close gripper
    else if (ctl->buttons() == 0x0011) {
        Serial.println("open bottom gripper");
        cooldownms = 10000;
        gripper(BOTTOM_NUM, OPEN, cooldownms);
    } else if (ctl->buttons() == 0x0021) {
        Serial.println("close bottom gripper");
        cooldownms = 10000;
        gripper(BOTTOM_NUM, CLOSE, cooldownms);
    }

    // B button relates to rack and pinion
    // press left trigger simultaneously to move rack and pinion up
    // press right trigger simultaneously to move rack and pinion down
    else if (ctl->buttons() == 0x0014) {
        Serial.println("move rack and pinion up");
        cooldownms = 10000;
        vertical_move(UP, cooldownms);
    } else if (ctl->buttons() == 0x0024) {
        Serial.println("move rack and pinion down");
        cooldownms = 10000;
        vertical_move(DOWN, cooldownms);
    }

    // X button relates to wheels
    // press left trigger simultaneously to move robot left
    // press right trigger simultaneously to move robot right
    // or alternatively, move left joystick left or right
    else if (ctl->buttons() == 0x0012 || ctl->axisX() < -200) {
        Serial.println("move robot left");
        cooldownms = 10000;
        horizontal_move(LEFT, cooldownms);
    } else if (ctl->buttons() == 0x0022 || ctl->axisX() > 200) {
        Serial.println("move robot right");
        cooldownms = 10000;
        horizontal_move(RIGHT, cooldownms);
    }

    // Press joystick to initiate noboru
    else if (ctl->buttons() == 0x0100) {
        Serial.println("initiate noboru");
        init_noboru();
    }

    // Press Y and joystick to step up
    else if (ctl->buttons() == 0x0110) {
        Serial.println("step up");
        stepup();
    }

    // Press A and joystick to step down
    else if (ctl->buttons() == 0x0120) {
        Serial.println("step down");
        stepdown();
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

    //     vTaskDelay(1);
    delay(150);
}