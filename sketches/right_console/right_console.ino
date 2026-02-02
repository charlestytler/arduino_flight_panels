#define DCSBIOS_DEFAULT_SERIAL

// #define DEBUG

#include "DcsBios.h"
#include "GpioExpander.h"
#include <Joystick.h>

const uint8_t hidReportId = 101;
const uint8_t buttonCount = 64;
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
    ENGINE_PWR_1 = 0,
    ENGINE_PWR_2,
    NUM_GPIO_EXPANDERS // Maximum value of enum
};

GpioExpander io[NUM_GPIO_EXPANDERS];
GpioConfig io_configs[NUM_GPIO_EXPANDERS] = {
    //
    [ENGINE_PWR_1] = {0b000, {X, X, X, X, X, X, X, X}, btn_index},
    [ENGINE_PWR_2] = {0b100, {X, X, X, X, X, X, X, X}, btn_index += 8},
};

void turn_off_all_leds()
{
    for (int i = 0; i < NUM_GPIO_EXPANDERS; i++) {
        io[i].set_all_led(0);
    }
}

// DCS BIOS Configuration

void onAcftNameChange(char *newValue) { turn_off_all_leds(); }
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

DcsBios::IntegerBuffer A10LdgGearHandle(0x1026, 0x4000, 14, set_caution_panel_led(1, 4, COLOR::GREEN));

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
