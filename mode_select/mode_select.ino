
constexpr int kMasterCautionLedPin = 15;
constexpr int kAA_LedPin = 14;
constexpr int kNAV_LedPin = 16;
constexpr int kAG_LedPin = 10;

constexpr int kMasterCautionSwPin = 5;
constexpr int kAA_SwPin = 6;
constexpr int kNAV_SwPin = 7;
constexpr int kAG_SwPin = 8;

constexpr int kRotaryClkPin = 2;
constexpr int kRotaryDtPin = 3;
constexpr int kRotarySwPin = 4;

static int i = 0;
const int led_order[4] = {kMasterCautionLedPin, kAA_LedPin, kNAV_LedPin, kAG_LedPin};

void setup()
{
    pinMode(kMasterCautionLedPin, OUTPUT);
    pinMode(kAA_LedPin, OUTPUT);
    pinMode(kNAV_LedPin, OUTPUT);
    pinMode(kAG_LedPin, OUTPUT);

    pinMode(kMasterCautionSwPin, INPUT_PULLUP);
    pinMode(kAA_SwPin, INPUT_PULLUP);
    pinMode(kNAV_SwPin, INPUT_PULLUP);
    pinMode(kAG_SwPin, INPUT_PULLUP);
    pinMode(kRotarySwPin, INPUT_PULLUP);

    digitalWrite(led_order[i], HIGH);
}

void advance_led()
{
    digitalWrite(led_order[i], LOW);
    if (i++ >= 4)
    {
        i = 0;
    }
    digitalWrite(led_order[i], HIGH);
    delay(1000); // Delay for dumb debounce.
}

void loop()
{
    if (!digitalRead(kMasterCautionSwPin) ||
        !digitalRead(kAA_SwPin) ||
        !digitalRead(kNAV_SwPin) ||
        !digitalRead(kAG_SwPin) ||
        !digitalRead(kRotarySwPin))
    {
        advance_led();
    }
}
