
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

void setup() {
    pinMode(kMasterCautionLedPin, OUTPUT);
    pinMode(kAA_LedPin, OUTPUT);
    pinMode(kNAV_LedPin, OUTPUT);
    pinMode(kAG_LedPin, OUTPUT);
}

void loop() {
    const int led_order[4] = {kMasterCautionLedPin, kAA_LedPin, kNAV_LedPin, kAG_LedPin};
    for (int i=0; i<4; i++)
    {
        digitalWrite(led_order[i], HIGH);
        delay(1000);
        digitalWrite(led_order[i], LOW);
    }
}
