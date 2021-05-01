
#define DCSBIOS_DEFAULT_SERIAL
#include <DcsBios.h>
#include <Joystick.h>

#include "JoystickEncoder.hh"
#include "ModuleLED.h"

#include "UFC_OLED_Display.h"

/// Global frame time per loop() call.
constexpr int kFrameTime_ms = 10;

/// Pin assignments.
constexpr int kPinMasterCautionLed = 15;
constexpr int kPinAA_Led = 14;
constexpr int kPinNAV_Led = 16;
constexpr int kPinAG_Led = 10;

constexpr int kPinMasterCautionSw = 6;
constexpr int kPinAA_Sw = 7;
constexpr int kPinNAV_Sw = 8;
constexpr int kPinAG_Sw = 9;

constexpr int kPinRotaryClk = 1;
constexpr int kPinRotaryDt = 0;
constexpr int kPinRotarySw = 5;

constexpr int kOLEDVcc = 4;
constexpr int kOLEDSDA = 2;
constexpr int kOLEDSCL = 3;

/// Joystick button assignments.
enum ButtonID : uint8_t
{
    kButtonMasterCaution = 0,
    kButtonAA,
    kButtonNAV,
    kButtonAG,
    kButtonEncoderPress,
    kButtonEncoderLeft,
    kButtonEncoderRight
};

/// DCS-Bios connections.

// Define all LEDs in an array to easily loop through them later.
DcsBios::ModuleLED leds[] = {
    DcsBios::ModuleLED("A-10C", 0x1012, 0x0800, kPinMasterCautionLed),
    DcsBios::ModuleLED("A-10C", 0x1026, 0x8000, kPinAG_Led),
    DcsBios::ModuleLED("A-10C_2", 0x1012, 0x0800, kPinMasterCautionLed),
    DcsBios::ModuleLED("A-10C_2", 0x1026, 0x8000, kPinAG_Led),
    DcsBios::ModuleLED("AV8BNA", 0x7860, 0x1000, kPinMasterCautionLed),
    DcsBios::ModuleLED("AV8BNA", 0x7884, 0x0010, kPinAA_Led),
    DcsBios::ModuleLED("AV8BNA", 0x7884, 0x0008, kPinNAV_Led),
    DcsBios::ModuleLED("AV8BNA", 0x7884, 0x0020, kPinAG_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x7408, 0x0200, kPinMasterCautionLed),
    DcsBios::ModuleLED("FA-18C_hornet", 0x740c, 0x0200, kPinAA_Led),
    DcsBios::ModuleLED("FA-18C_hornet", 0x740c, 0x0400, kPinAG_Led),
    DcsBios::ModuleLED("F-14B", 0x12d4, 0x0080, kPinMasterCautionLed),
    DcsBios::ModuleLED("F-14B", 0x127e, 0x0004, kPinAA_Led),
    DcsBios::ModuleLED("F-14B", 0x127e, 0x0002, kPinNAV_Led),
    DcsBios::ModuleLED("F-14B", 0x127e, 0x0008, kPinAG_Led),
    DcsBios::ModuleLED("F-16C_50", 0x4472, 0x0800, kPinMasterCautionLed),
    DcsBios::ModuleLED("F-16C_50", 0x4426, 0x0100, kPinAA_Led),
    DcsBios::ModuleLED("F-16C_50", 0x4426, 0x0200, kPinAG_Led),
    DcsBios::ModuleLED("F-5E-3", 0x7602, 0x0020, kPinMasterCautionLed),
    DcsBios::ModuleLED("F-5E-3", 0x760e, 0x0800, kPinNAV_Led),
    DcsBios::ModuleLED("F-5E-3", 0x760e, 0x0400, kPinAA_Led),
    DcsBios::ModuleLED("F-5E-3", 0x760e, 0x1000, kPinAG_Led),
    DcsBios::ModuleLED("Ka-50", 0x1814, 0x0800, kPinMasterCautionLed),
    DcsBios::ModuleLED("L-39C", 0x3336, 0x0002, kPinMasterCautionLed),
    DcsBios::ModuleLED("L-39ZA", 0x3336, 0x0002, kPinMasterCautionLed),
    DcsBios::ModuleLED("M-2000C", 0x7200, 0x8000, kPinMasterCautionLed),
    DcsBios::ModuleLED("UH-1H", 0x1416, 0x0100, kPinMasterCautionLed),
    DcsBios::ModuleLED("UH-1H", 0x1416, 0x0010, kPinAG_Led),
    DcsBios::ModuleLED("AJS37", 0x4618, 0x1000, kPinAA_Led),
    DcsBios::ModuleLED("AJS37", 0x4618, 0x0800, kPinNAV_Led),
    DcsBios::ModuleLED("AJS37", 0x4618, 0x0400, kPinAG_Led),
};

static char current_module[32];

// This callback function is called every aircraft change with the module name
// as newValue.
void onAcftNameChange(char *newValue)
{
    strcpy(current_module, newValue);
    const size_t number_of_leds = sizeof(leds) / sizeof((leds)[0]);
    for (size_t i = 0; i < number_of_leds; i++)
    {
        leds[i].set_active_according_to_module(newValue);
    }
    OLED.clear();
    OLED.powerMode(SSD1311_LCD_ON);
}
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

Joystick_ Joystick;
JoystickEncoder rotaryEnc(kPinRotaryDt, kPinRotaryClk);

void setup()
{
    pinMode(kOLEDVcc, OUTPUT);
    digitalWrite(kOLEDVcc, HIGH);
    OLED_20x2_setup();
    DcsBios::setup();
    Joystick.begin();

    pinMode(kPinMasterCautionLed, OUTPUT);
    pinMode(kPinAA_Led, OUTPUT);
    pinMode(kPinNAV_Led, OUTPUT);
    pinMode(kPinAG_Led, OUTPUT);

    pinMode(kPinMasterCautionSw, INPUT_PULLUP);
    pinMode(kPinAA_Sw, INPUT_PULLUP);
    pinMode(kPinNAV_Sw, INPUT_PULLUP);
    pinMode(kPinAG_Sw, INPUT_PULLUP);
    pinMode(kPinRotarySw, INPUT_PULLUP);
}

void loop()
{
    unsigned long frame_start_time_ms = millis();

    DcsBios::loop();

    const TurnDirection rotary_output = rotaryEnc.process_encoder();
    const bool turn_left = (rotary_output == TurnDirection::kLeft) ? true : false;
    const bool turn_right = (rotary_output == TurnDirection::kRight) ? true : false;
    Joystick.setButton(kButtonEncoderLeft, turn_left);
    Joystick.setButton(kButtonEncoderRight, turn_right);

    // Read all button switches.
    const bool master_caution_is_pressed = !digitalRead(kPinMasterCautionSw);
    const bool aa_is_pressed = !digitalRead(kPinAA_Sw);
    const bool nav_is_pressed = !digitalRead(kPinNAV_Sw);
    const bool ag_is_pressed = !digitalRead(kPinAG_Sw);
    const bool rotary_is_pressed = !digitalRead(kPinRotarySw);
    // Set values in joystick.
    Joystick.setButton(kButtonMasterCaution, master_caution_is_pressed);
    Joystick.setButton(kButtonAA, aa_is_pressed);
    Joystick.setButton(kButtonNAV, nav_is_pressed);
    Joystick.setButton(kButtonAG, ag_is_pressed);
    Joystick.setButton(kButtonEncoderPress, rotary_is_pressed);

    // Clear LEDs combo:
    if (aa_is_pressed && ag_is_pressed)
    {
        digitalWrite(kPinMasterCautionLed, LOW);
        digitalWrite(kPinAA_Led, LOW);
        digitalWrite(kPinNAV_Led, LOW);
        digitalWrite(kPinAG_Led, LOW);
        OLED.powerMode(SSD1311_LCD_OFF);
    }

    while (millis() - frame_start_time_ms < kFrameTime_ms)
    {
        // Wait.
    }
}
