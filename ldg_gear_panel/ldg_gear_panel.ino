
#include <Joystick.h>

/// Global frame time per loop() call.
constexpr int kFrameTime_ms = 10;

/// Pin assignments.
constexpr int kPinLEngFire_Led = 1;
constexpr int kPinApuFire_Led = 1;
constexpr int kPinNoseGear_Led = 1;
constexpr int kPinLHGear_Led = 1;
constexpr int kPinRHGear_Led = 1;

constexpr int kPinLEngFireSw = 1;

int buttonPins[15] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18, 19, 20};

Joystick_ Joystick;

class Switch3way
{
public:
    explicit Switch3way(int up, int down, int middle) : upPin(up), downPin(down), middleButtonID(middle)
    {
    }

    updateButtons()
    {
        const bool up_pressed = !digitalRead(upPin);
        const bool down_pressed = !digitalRead(downPin);
        Joystick.setButton(upPin - 1, up_pressed);
        Joystick.setButton(downPin - 1, down_pressed);
        Joystick.setButton(middleButtonID - 1, (!up_pressed && !down_pressed) ? true : false);
    }

private:
    int upPin;
    int downPin;
    int middleButtonID;
};

Switch3way flir(6, 7, 21);
Switch3way brake(14, 16, 32);
Switch3way lights(15, 18, 31);
Switch3way strut(19, 20, 26);

int switch2way[] = {2, 3, 4, 5, 8, 9, 10};

void setup()
{
    Joystick.begin();

    for (int i = 0; i < 15; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

void loop()
{
    unsigned long frame_start_time_ms = millis();

    // DcsBios::loop();

    // Read all 2 way switches.
    for (int i = 0; i < 7; i++)
    {
        const bool button_is_pressed = !digitalRead(switch2way[i]);
        Joystick.setButton(switch2way[i] - 1, button_is_pressed);
        Joystick.setButton(switch2way[i] - 1 + 20, !button_is_pressed);
    }
    flir.updateButtons();
    brake.updateButtons();
    lights.updateButtons();
    strut.updateButtons();

    while (millis() - frame_start_time_ms < kFrameTime_ms)
    {
        // Wait.
    }
}
