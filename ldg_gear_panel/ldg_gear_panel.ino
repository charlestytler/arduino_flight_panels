#define DCSBIOS_DEFAULT_SERIAL
#include <DcsBios.h>
#include "ModuleLED.h"
#include <Joystick.h>
#include <PCF8574.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

/// Global frame time per loop() call.
constexpr int kFrameTime_ms = 10;

PCF8574 ioExpanderPins[] = {PCF8574(0x20), PCF8574(0x21), PCF8574(0x24)};
const int numExpanderPinsUsed[] = {8, 8, 8};
const int numExpanders = NELEMS(numExpanderPinsUsed);

Joystick_ Joystick;

/// Pin assignments.
constexpr int kPinLEngFire_Led = 18;
constexpr int kPinApuFire_Led = 14;
constexpr int kPinNoseGear_Led = 5;
constexpr int kPinLHGear_Led = 6;
constexpr int kPinRHGear_Led = 4;
constexpr int kPinEmerGear_Led = 7;

const int buttonPins[] = {8, 9, 10, 16, 15, 19, 20, 21};
// The following buttons (using Joystick ID) are identified as toggles which will add an additional button for their OFF condition.
const int toggle2way[] = {21, 22, 23, 24};
const int toggle3way[][2] = {{1, 2}, {19, 20}, {13, 14}, {29, 30}, {31, 32}};

/// DCS-Bios connections.
DcsBios::ModuleLED leds[] = {
    DcsBios::ModuleLED("FA-18C_hornet", 0x7408, 0x0040, kPinLEngFire_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x740c, 0x0004, kPinApuFire_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x7430, 0x1000, kPinLHGear_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x7430, 0x0800, kPinNoseGear_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x7430, 0x2000, kPinRHGear_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x7478, 0x0800, kPinEmerGear_Led),
    DcsBios::ModuleLED("A-10C", 0x10da, 0x0008, kPinLEngFire_Led),
    DcsBios::ModuleLED("A-10C", 0x10da, 0x0010, kPinApuFire_Led),
    DcsBios::ModuleLED("A-10C", 0x1026, 0x1000, kPinLHGear_Led),
    DcsBios::ModuleLED("A-10C", 0x1026, 0x0800, kPinNoseGear_Led),
    DcsBios::ModuleLED("A-10C", 0x1026, 0x2000, kPinRHGear_Led),
    DcsBios::ModuleLED("A-10C", 0x1026, 0x4000, kPinEmerGear_Led),
    DcsBios::ModuleLED("A-10C_2", 0x10da, 0x0008, kPinLEngFire_Led),
    DcsBios::ModuleLED("A-10C_2", 0x10da, 0x0010, kPinApuFire_Led),
    DcsBios::ModuleLED("A-10C_2", 0x1026, 0x1000, kPinLHGear_Led),
    DcsBios::ModuleLED("A-10C_2", 0x1026, 0x0800, kPinNoseGear_Led),
    DcsBios::ModuleLED("A-10C_2", 0x1026, 0x2000, kPinRHGear_Led),
    DcsBios::ModuleLED("A-10C_2", 0x1026, 0x4000, kPinEmerGear_Led)};

void onAcftNameChange(char *newValue)
{
    for (size_t i = 0; i < NELEMS(leds); i++)
    {
        leds[i].set_active_according_to_module(newValue);
    }
}
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

void setup()
{
    DcsBios::setup();
    Joystick.begin();

    for (int i = 0; i < numExpanders; i++)
    {
        ioExpanderPins[i].begin();
    }

    for (size_t i = 0; i < NELEMS(buttonPins); i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

void loop()
{
    unsigned long frame_start_time_ms = millis();

    DcsBios::loop();

    // Read all button pins.
    int numInputPins = NELEMS(buttonPins) + NELEMS(toggle2way) + NELEMS(toggle3way);
    for (int i = 0; i < numExpanders; i++)
    {
        numInputPins += numExpanderPinsUsed[i];
    }
    bool inputPinStatus[numInputPins];

    int inputCount = 0;
    for (size_t i = 0; i < NELEMS(buttonPins); i++)
    {
        inputPinStatus[inputCount++] = !digitalRead(buttonPins[i]);
    }
    for (int ioExpander = 0; ioExpander < numExpanders; ioExpander++)
    {
        uint8_t expanderPinStates = ioExpanderPins[ioExpander].read8();
        for (int pin = 0; pin < numExpanderPinsUsed[ioExpander]; pin++)
        {
            inputPinStatus[inputCount++] = !(expanderPinStates & (1 << pin));
        }
    }

    // Set button values for OFF states of toggle switches.
    for (size_t i = 0; i < NELEMS(toggle2way); i++)
    {
        const bool toggleInactive = !inputPinStatus[toggle2way[i]];
        inputPinStatus[inputCount++] = toggleInactive ? true : false;
    }
    for (size_t i = 0; i < NELEMS(toggle3way); i++)
    {
        const bool toggleInactive = (!inputPinStatus[toggle3way[i][0]] && !inputPinStatus[toggle3way[i][1]]);
        inputPinStatus[inputCount++] = toggleInactive ? true : false;
    }

    // Set Joystick button states.
    for (int i = 0; i < numInputPins; i++)
    {
        Joystick.setButton(i, inputPinStatus[i]);
    }

    while (millis() - frame_start_time_ms < kFrameTime_ms)
    {
        // Wait.
    }
}

void set_all_led_off()
{
    digitalWrite(kPinLEngFire_Led, LOW);
    digitalWrite(kPinApuFire_Led, LOW);
    digitalWrite(kPinNoseGear_Led, LOW);
    digitalWrite(kPinLHGear_Led, LOW);
    digitalWrite(kPinRHGear_Led, LOW);
    digitalWrite(kPinEmerGear_Led, LOW);
}