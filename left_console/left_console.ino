#define DCSBIOS_DEFAULT_SERIAL

// #define DEBUG

#include "DcsBios.h"
#include "GpioExpander.h"
#include <Joystick.h>

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

enum GpioExpanderNames {
    LDG_GEAR_1 = 0,
    LDG_GEAR_2,
    FLT_CONTROL,
    ENG_START,
    REFUEL,
    FUEL_1,
    FUEL_2,
    AFCS,
    NUM_GPIO_EXPANDERS // Maximum value of enum
};

GpioExpander io[NUM_GPIO_EXPANDERS];
GpioConfig io_configs[NUM_GPIO_EXPANDERS] = { //
    [LDG_GEAR_1] = {0b000, {L, L, L, L, X, X, X, X}, btn_index},
    [LDG_GEAR_2] = {0b100, {D, D, S, S, S, S, S, S}, btn_index += 8},
    [FLT_CONTROL] = {0b011, {L, S, S, S, S, S, S, S}, btn_index += 8},
    [ENG_START] = {0b010, {D, D, D, D, S, L, D, D}, btn_index += 8},
    [REFUEL] = {0b001, {D, D, S, S, S, S, S, X}, btn_index += 8},
    [FUEL_1] = {0b101, {S, S, S, S, D, D, D, D}, btn_index += 8},
    [FUEL_2] = {0b110, {D, D, S, D, D, S, X, X}, btn_index += 8},
    [AFCS] = {0b111, {D, D, D, D, S, D, D, X}, btn_index += 8}};

// Callback functions for specific LEDs.

void set_ldg_gear_handle_led(unsigned int value) { io[LDG_GEAR_1].set_led(0, value); }
void set_ldg_gear_left_led(unsigned int value) { io[LDG_GEAR_1].set_led(1, value); }
void set_ldg_gear_right_led(unsigned int value) { io[LDG_GEAR_1].set_led(2, value); }
void set_ldg_gear_nose_led(unsigned int value) { io[LDG_GEAR_1].set_led(3, value); }

void set_flt_control_to_trim_led(unsigned int value) { io[FLT_CONTROL].set_led(0, value); }
void set_eng_start_apu_led(unsigned int value) { io[ENG_START].set_led(7, value); }

void turn_off_all_leds()
{
    for (int i = 0; i < NUM_GPIO_EXPANDERS; i++) {
        io[i].set_all_led(0);
    }
}

// DCS BIOS Configuration

void onAcftNameChange(char *newValue) { turn_off_all_leds(); }
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

DcsBios::IntegerBuffer A10LdgGearHandle(0x1026, 0x4000, 14, set_ldg_gear_handle_led);
DcsBios::IntegerBuffer A10LdgGearLeft(0x1026, 0x1000, 12, set_ldg_gear_left_led);
DcsBios::IntegerBuffer A10LdgGearRight(0x1026, 0x2000, 13, set_ldg_gear_right_led);
DcsBios::IntegerBuffer A10LdgGearNose(0x1026, 0x0800, 11, set_ldg_gear_nose_led);

DcsBios::IntegerBuffer F18LdgGearHandle(0x747e, 0x0800, 11, set_ldg_gear_handle_led);
DcsBios::IntegerBuffer F18LdgGearLeft(0x7430, 0x1000, 12, set_ldg_gear_left_led);
DcsBios::IntegerBuffer F18LdgGearRight(0x7430, 0x2000, 13, set_ldg_gear_right_led);
DcsBios::IntegerBuffer F18LdgGearNose(0x7430, 0x0800, 11, set_ldg_gear_nose_led);
//
DcsBios::IntegerBuffer F18ApuReady(0x74c2, 0x0800, 11, set_eng_start_apu_led);

DcsBios::IntegerBuffer F16LdgGearHandle(0x447c, 0x0002, 1, set_ldg_gear_handle_led);
DcsBios::IntegerBuffer F16LdgGearLeft(0x447a, 0x8000, 15, set_ldg_gear_left_led);
DcsBios::IntegerBuffer F16LdgGearRight(0x447c, 0x0001, 0, set_ldg_gear_right_led);
DcsBios::IntegerBuffer F16LdgGearNose(0x447a, 0x4000, 14, set_ldg_gear_nose_led);

DcsBios::IntegerBuffer F15LgHndLBuffer(F_15E_F_LG_HND_L, set_ldg_gear_handle_led);
DcsBios::IntegerBuffer F15LgLeftGLBuffer(F_15E_F_LG_LEFT_G_L, set_ldg_gear_left_led);
DcsBios::IntegerBuffer F15LgRightGLBuffer(F_15E_F_LG_RIGHT_G_L, set_ldg_gear_right_led);
DcsBios::IntegerBuffer F15LgNoseGLBuffer(F_15E_F_LG_NOSE_G_L, set_ldg_gear_nose_led);
DcsBios::IntegerBuffer F15CasTrimLBuffer(F_15E_F_CAS_TRIM_L, set_flt_control_to_trim_led);
DcsBios::IntegerBuffer F15GenStarterLBuffer(F_15E_F_GEN_STARTER_L, set_eng_start_apu_led);

// Arduino Runtime

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
    delay(5000);
    Serial.println("START");
#endif

    DcsBios::setup();
    constexpr bool autoSendState = false;
    joystick.begin(autoSendState);

    Wire.begin();
    for (int i = 0; i < NUM_GPIO_EXPANDERS; i++) {
        io[i].setup(io_configs[i]);
    }
    turn_off_all_leds();
}

constexpr int LOOP_FRAME_TIME_MS = 10; // Run at 100Hz.
void loop()
{
    unsigned long frame_start_time_ms = millis();

    DcsBios::loop();

    for (int i = 0; i < NUM_GPIO_EXPANDERS; i++) {
        io[i].loop(joystick);
    }
#ifdef DEBUG
    Serial.println(" ");
#endif

    joystick.sendState();

    while (millis() - frame_start_time_ms < LOOP_FRAME_TIME_MS) {
        // Wait.
    }
}
