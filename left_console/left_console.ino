
#define DCSBIOS_DEFAULT_SERIAL

#include <DcsBios.h>
#include <Joystick.h>
#include "../GpioExpansion.h"

const uint8_t hidReportId = 100;
const uint8_t buttonCount = 80;
const uint8_t hatSwitchCount = 0;
Joystick_ joystick(hidReportId,
                   JOYSTICK_TYPE_JOYSTICK,
                   buttonCount,
                   hatSwitchCount,
                   false, // include axes
                   false,
                   false,
                   false,
                   false,
                   false,
                   false,
                   false,
                   false,
                   false,
                   false);

// Arduino Pin Configuration

int btn_index = 0;

enum GpioExpanderNames
{
    LDG_GEAR_1 = 0,
    LDG_GEAR_2,
    FLT_CONTROL,
    ENG_START,
    REFUEL,
    FUEL_1,
    FUEL_2,
    AFCS,
    NUM_GPIO_EXPANDERS // Maximum value of enum
}

GpioExpander io[] = {
    // LDG_GEAR
    GpioExpander(0, 0, 0, {L, L, L, L, X, X, X, X}, btn_index),
    GpioExpander(1, 0, 0, {D, D, S, S, S, S, S, S}, btn_index += 8),
    // FLT_CONTROL
    GpioExpander(0, 1, 1, {L, S, S, S, S, S, S, S}, btn_index += 8),
    // ENG_START
    GpioExpander(0, 1, 0, {D, D, D, D, S, X, X, L}, btn_index += 8),
    // REFUEL
    GpioExpander(0, 0, 1, {D, D, S, S, S, S, S, X}, btn_index += 8),
    // FUEL
    GpioExpander(1, 0, 1, {S, S, S, S, D, D, D, D}, btn_index += 8),
    GpioExpander(1, 1, 0, {D, D, S, D, D, S, X, X}, btn_index += 8),
    // AFCS
    GpioExpander(1, 1, 1, {D, D, D, D, S, D, D, X}, btn_index += 8),
};

// Callback functions for specific LEDs.
void set_ldg_gear_handle_led(int value) { io[LDG_GEAR_1].set_led(0, value); }
void set_ldg_gear_left_led(int value) { io[LDG_GEAR_1].set_led(1, value); }
void set_ldg_gear_right_led(int value) { io[LDG_GEAR_1].set_led(2, value); }
void set_ldg_gear_nose_led(int value) { io[LDG_GEAR_1].set_led(3, value); }

void set_flt_control_to_trim_led(int value) { io[FLT_CONTROL].set_led(0, value); }
void set_eng_start_apu_led(int value) { io[ENG_START].set_led(7, value); }

void turn_off_all_leds()
{
    for (int i = 0; i < NUM_GPIO_EXPANDERS)
    {
        io[i].set_all_led(0);
    }
}

// DCS BIOS Configuration

void onAcftNameChange(char *newValue)
{
    turn_off_all_leds();
}
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

DcsBios::IntegerBuffer A10LdgGearHandle(0x747e, 0x0800, 99, set_ldg_gear_handle_led);
DcsBios::IntegerBuffer A10LdgGearLeft(0x7430, 0x1000, 99, set_ldg_gear_left_led);
DcsBios::IntegerBuffer A10LdgGearRight(0x7430, 0x2000, 99, set_ldg_gear_right_led);
DcsBios::IntegerBuffer A10LdgGearNose(0x7430, 0x0800, 99, set_ldg_gear_nose_led);


// Arduino Runtime

void setup() {
    DcsBios::setup();
    constexpr bool autoSendState = false;
    joystick.begin(autoSendState);
}

constexpr int LOOP_FRAME_TIME_MS = 10;
void loop() {
    unsigned long frame_start_time_ms = millis();

    for (int i=0; i<NUM_GPIO_EXPANDERS; i++) {
        io[i].loop(&joystick);
    }


    joystick.sendState();

    while (millis() - frame_start_time_ms < LOOP_FRAME_TIME_MS) {
        // Wait.
    }
}
