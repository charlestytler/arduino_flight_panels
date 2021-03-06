
#define DCSBIOS_DEFAULT_SERIAL
#include <DcsBios.h>
#include <Joystick.h>

#include "JoystickEncoder.hh"

/// Global frame time per loop() call.
constexpr int kFrameTime_ms = 10;

/// Pin assignments.
constexpr int kPinMasterCautionLed = 15;
constexpr int kPinAA_Led = 14;
constexpr int kPinNAV_Led = 16;
constexpr int kPinAG_Led = 10;

constexpr int kPinMasterCautionSw = 5;
constexpr int kPinAA_Sw = 6;
constexpr int kPinNAV_Sw = 7;
constexpr int kPinAG_Sw = 8;

constexpr int kPinRotaryClk = 2;
constexpr int kPinRotaryDt = 3;
constexpr int kPinRotarySw = 4;

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

// A-10C
DcsBios::LED masterCaution(0x1012, 0x0800, kPinMasterCautionLed);
DcsBios::LED gunReady(0x1026, 0x8000, kPinAG_Led);

// AV8BNA
DcsBios::LED mcLight(0x7860, 0x1000, kPinMasterCautionLed);
DcsBios::LED vstolLight(0x7884, 0x0010, kPinAA_Led);
DcsBios::LED navLight(0x7884, 0x0008, kPinNAV_Led);
DcsBios::LED agLight(0x7884, 0x0020, kPinAG_Led);

// F/A-18C
DcsBios::LED masterCautionLt(0x7408, 0x0200, kPinMasterCautionLed);
DcsBios::LED masterModeAaLt(0x740c, 0x0200, kPinAA_Led);
DcsBios::LED masterModeAgLt(0x740c, 0x0400, kPinAG_Led);

// F-16
DcsBios::LED lightMasterCaution(0x4472, 0x0800, kPinMasterCautionLed);
DcsBios::LED icpAaModeBtn(0x4426, 0x0100, kPinAA_Led);
DcsBios::LED icpAgModeBtn(0x4426, 0x0200, kPinAG_Led);

// F-5E
//DcsBios::LED mcLight(0x7602, 0x0020, kPinMasterCautionLed);
//DcsBios::LED leftLight(0x760e, 0x0800, kPinNAV_Led);
//DcsBios::LED noseLight(0x760e, 0x0400, kPinAA_Led);
//DcsBios::LED rightLight(0x760e, 0x1000, kPinAG_Led);

// KA-50
DcsBios::LED scMasterCautionLed(0x1814, 0x0800, kPinMasterCautionLed);

// L-39
DcsBios::LED frontMasterCautionLamp(0x3336, 0x0002, kPinMasterCautionLed);

// M-2000
DcsBios::LED apMasterAmbre(0x7200, 0x8000, kPinMasterCautionLed);

// UH-1H
DcsBios::LED masterCautionInd(0x1416, 0x0100, kPinMasterCautionLed);
DcsBios::LED armedInd(0x1416, 0x0010, kPinAG_Led);

// VIGGEN
DcsBios::LED hojdLamp(0x4618, 0x1000, kPinAA_Led);
DcsBios::LED attLamp(0x4618, 0x0800, kPinNAV_Led);
DcsBios::LED spakLamp(0x4618, 0x0400, kPinAG_Led);

Joystick_ Joystick;
JoystickEncoder rotaryEnc(kPinRotaryDt, kPinRotaryClk);

void setup()
{
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
    }

    while (millis() - frame_start_time_ms < kFrameTime_ms)
    {
        // Wait.
    }
}
