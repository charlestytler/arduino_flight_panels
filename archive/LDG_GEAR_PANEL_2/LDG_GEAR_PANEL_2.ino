#define DCSBIOS_DEFAULT_SERIAL

#include <DcsBios.h>
#include <Joystick.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

/// Global frame time per loop() call.
constexpr int kFrameTime_ms = 10;

Joystick_ Joystick;

/// Pin assignments.
constexpr int PIN_NOSE_GEAR_LED = 6;
constexpr int PIN_LH_GEAR_LED = 7;
constexpr int PIN_RH_GEAR_LED = 8;
constexpr int PIN_LDG_GEAR_LED = 9;

const int buttonPins[] = {4, 5};
// The following buttons (using Joystick ID) are identified as toggles which will add an additional button for their OFF
// condition.
const int toggle2way[] = {2, 3};
const int toggle3way[][2] = {{14, 15}};

/// DCS-Bios connections.
// DcsBios::LED flpLgLeftGearLt(FA_18C_hornet_FLP_LG_LEFT_GEAR_LT_AM, PIN_LH_GEAR_LED);
// DcsBios::LED flpLgNoseGearLt(FA_18C_hornet_FLP_LG_NOSE_GEAR_LT_AM, PIN_NOSE_GEAR_LED);
// DcsBios::LED flpLgRightGearLt(FA_18C_hornet_FLP_LG_RIGHT_GEAR_LT_AM, PIN_RH_GEAR_LED);
// DcsBios::LED landingGearHandleLt(FA_18C_hornet_LANDING_GEAR_HANDLE_LT_AM, PIN_LDG_GEAR_LED);

DcsBios::LED flpLgLeftGearLt(0x7430, 0x1000, PIN_LH_GEAR_LED);
DcsBios::LED flpLgNoseGearLt(0x7430, 0x0800, PIN_NOSE_GEAR_LED);
DcsBios::LED flpLgRightGearLt(0x7430, 0x2000, PIN_RH_GEAR_LED);
DcsBios::LED landingGearHandleLt(0x747e, 0x0800, PIN_LDG_GEAR_LED);
/*

DcsBios::LED gearLSafe(A_10C_GEAR_L_SAFE_AM, PIN_LH_GEAR_LED);
DcsBios::LED gearNSafe(A_10C_GEAR_N_SAFE_AM, PIN_NOSE_GEAR_LED);
DcsBios::LED gearRSafe(A_10C_GEAR_R_SAFE_AM, PIN_RH_GEAR_LED);
DcsBios::LED handleGearWarning(A_10C_HANDLE_GEAR_WARNING_AM, PIN_LDG_GEAR_LED);

DcsBios::LED lightGearL(F_16C_50_LIGHT_GEAR_L_AM, PIN_LH_GEAR_LED);
DcsBios::LED lightGearN(F_16C_50_LIGHT_GEAR_N_AM, PIN_NOSE_GEAR_LED);
DcsBios::LED lightGearR(F_16C_50_LIGHT_GEAR_R_AM, PIN_RH_GEAR_LED);
DcsBios::LED lightGearWarn(F_16C_50_LIGHT_GEAR_WARN_AM, PIN_LDG_GEAR_LED);

DcsBios::LED fLgLeftGL(F_15E_F_LG_LEFT_G_L_AM, PIN_LH_GEAR_LED);
DcsBios::LED fLgNoseGL(F_15E_F_LG_NOSE_G_L_AM, PIN_NOSE_GEAR_LED);
DcsBios::LED fLgRightGL(F_15E_F_LG_RIGHT_G_L_AM, PIN_RH);
DcsBios::LED fLgHndL(F_15E_F_LG_HND_L_AM, PIN_LDG_GEAR_LED);

DcsBios::LED leftGearG(AV8BNA_LEFT_GEAR_G_AM, PIN_LH_GEAR_LED);
// DcsBios::LED mainGearG(AV8BNA_MAIN_GEAR_G_AM, PIN);
DcsBios::LED noseGearG(AV8BNA_NOSE_GEAR_G_AM, PIN_NOSE_GEAR_LED);
DcsBios::LED rightGearG(AV8BNA_RIGHT_GEAR_G_AM, PIN_RH_GEAR_LED);
DcsBios::LED gearLeverLight(AV8BNA_GEAR_LEVER_LIGHT_AM, PIN_LDG_GEAR_LED);

void onAcftNameChange(char *newValue) { set_all_led_off(); }
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);
*/

void setup()
{
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);

    DcsBios::setup();
    Joystick.begin();
}

// Constant that maps the phyical pin to the joystick button.
const int pinToButtonMap = 2;
// Last state of the button
int lastButtonState[2] = {0,0};

void loop()
{
    DcsBios::loop();

    // Read pin values
    for (int index = 0; index < 2; index++)
    {
      int currentButtonState = !digitalRead(index + pinToButtonMap);
      if (currentButtonState != lastButtonState[index])
      {
        Joystick.setButton(index, currentButtonState);
        lastButtonState[index] = currentButtonState;
      }
    }

    delay(50);
}
