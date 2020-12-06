#include <Encoder.h>

/// Global frame time per loop() call.
constexpr int kFrameTime_ms = 10;

/// Pin assignments.
enum PinID : byte
{
    kMasterCautionLed = 15,
    kAA_Led = 14,
    kNAV_Led = 16,
    kAG_Led = 10,

    kMasterCautionSw = 5,
    kAA_Sw = 6,
    kNAV_Sw = 7,
    kAG_Sw = 8,

    kRotaryClk = 2,
    kRotaryDt = 3,
    kRotarySw = 4,
};

/// Joystick button assignments.
enum class ButtonID
{
    kMasterCaution = 1,
    kAA,
    kNAV,
    kAG,
    kEncoderPress,
    kEncoderLeft,
    kEncoderRight
};

static int i = 0;
const int led_order[4] = {kMasterCautionLed, kAA_Led, kNAV_Led, kAG_Led};
long rotaryEncPosition = 0;
static long prevEncPosition = 0;

void advance_led()
{
    digitalWrite(led_order[i], LOW);
    if (++i >= 4)
    {
        i = 0;
    }
    digitalWrite(led_order[i], HIGH);
    //delay(50); // Delay for dumb debounce.
}

void unadvance_led()
{
    digitalWrite(led_order[i], LOW);
    if (--i < 0)
    {
        i = 3;
    }
    digitalWrite(led_order[i], HIGH);
    //delay(50); // Delay for dumb debounce.
}

class JoystickEncoder
{
public:
    explicit JoystickEncoder(PinID pin1, PinID pin2, ButtonID joyLeft, ButtonID joyRight) : encoder_monitor_(pin1, pin2), joy_left_{joyLeft}, joy_right_{joyRight}
    {
        encoder_monitor_.write(0);
    }

    void process_encoder()
    {
        const long encoder_position_reading = encoder_monitor_.read();
        const int delta_position = encoder_position_reading - debounced_encoder_position_;
        const bool debounce_has_settled = debounce_signal(encoder_position_reading, delta_position);

        Serial.print("   Debounced: ");
        Serial.print(debounced_encoder_position_);
        Serial.print("   Delta: ");
        Serial.print(delta_position);
        Serial.print("   Settled: ");
        Serial.print(debounce_has_settled);
        Serial.print("   DebounceCounter: ");
        Serial.print(debounce_counter_);
        Serial.print("   NumFastTurns: ");
        Serial.print(num_fast_turns_detected_);

        if (debounce_has_settled)
        {
            const int rounding_offset = kPositionTicksPerDetent_ / 2; // Better approximates rounding in integer division.
            const int num_detents_turned = (abs(delta_position) + rounding_offset) / kPositionTicksPerDetent_;
            if (num_detents_turned > num_fast_turns_detected_)
            {
                send_button_press(delta_position);
            }
            reset_fast_turns_detector_variables();
        }
        else
        {
            count_fast_turns_during_debounce(delta_position);
        }
        Serial.println();
    }

private:
    bool debounce_signal(long new_reading, int delta_position)
    {
        bool debounce_has_settled = false;
        if (new_reading != prev_encoder_position_reading_)
        {
            // Reset debounce counter.
            debounce_counter_ = kDebounceFrames_;
        }

        else
        {
            if (debounce_counter_ > 0)
            {
                debounce_counter_--;
            }
            else
            {
                // This is used to not trigger an output early, wait for more than one tick of change.
                if (abs(delta_position) > 1)
                {
                    debounced_encoder_position_ = new_reading;
                }
                debounce_has_settled = true;
            }
        }
        prev_encoder_position_reading_ = new_reading;
        return debounce_has_settled;
    }

    bool debounce_has_settled(int delta_position)
    {
        const bool min_delta_threshold_is_met = abs(delta_position) > 1;
        const bool debounce_threshold_is_met = debounce_counter_ >= 3;
        if (min_delta_threshold_is_met && debounce_threshold_is_met)
        {
            debounce_counter_ = 0;
            return true;
        }
        return false;
    }

    void send_button_press(int delta_position)
    {
        if (delta_position > 0)
        {
            Serial.print("     RIGHT TURN");
            advance_led();
        }
        else if (delta_position < 0)
        {
            Serial.print("     LEFT TURN");
            unadvance_led();
        }
    }

    void count_fast_turns_during_debounce(int delta_position)
    {
        TurnDirection current_turn_direction;
        if (delta_position > 0)
        {
            current_turn_direction = TurnDirection::kRight;
        }
        else if (delta_position < 0)
        {
            current_turn_direction = TurnDirection::kLeft;
        }

        // Reset fast turns count if turn direction (with respect to debounced position) changes.
        if (current_turn_direction != last_turn_direction_)
        {
            num_fast_turns_detected_ = 0;
        }
        last_turn_direction_ = current_turn_direction;

        const int next_fast_turn_threshold = (num_fast_turns_detected_ + 1) * kPositionTicksPerDetent_ + 1;
        if (abs(delta_position) > next_fast_turn_threshold)
        {
            send_button_press(delta_position);
            num_fast_turns_detected_++;
        }
    }

    void reset_fast_turns_detector_variables()
    {
        num_fast_turns_detected_ = 0;
        last_turn_direction_ = TurnDirection::kNone;
    }

    Encoder encoder_monitor_;               /// Instance of Encoder.
    long debounced_encoder_position_{0};    /// Stored position of encoder.
    long prev_encoder_position_reading_{0}; /// Undebounced position of encoder.
    ButtonID joy_left_;                     /// Joystick button number to send as pressed for a left turn.
    ButtonID joy_right_;                    /// Joystick button number to send as pressed for a right turn.

    // Internal varialbes for debouncing.
    int debounce_counter_{0}; /// Count-down counter used to debounce and suppress joystick output.

    // Internal varialbes for detecting fast turns.
    enum class TurnDirection
    {
        kNone,
        kLeft,
        kRight
    };
    int num_fast_turns_detected_{0};                           /// Keeps track of number of whole turns detected while debouncing.
    TurnDirection last_turn_direction_ = TurnDirection::kNone; /// Keeps track of last turn direction in case of change of direction during debounce.

    // Constant parameters.
    static constexpr int kDebounceFrames_{3};         /// Parameter for number of frames the signal must be steady before considered debounced.
    static constexpr int kPositionTicksPerDetent_{4}; /// Parameter for number of position ticks that represent one detent of rotary knob.
};

JoystickEncoder rotaryEnc(kRotaryDt, kRotaryClk, ButtonID::kEncoderLeft, ButtonID::kEncoderRight);

void setup()
{
    pinMode(kMasterCautionLed, OUTPUT);
    pinMode(kAA_Led, OUTPUT);
    pinMode(kNAV_Led, OUTPUT);
    pinMode(kAG_Led, OUTPUT);

    pinMode(kMasterCautionSw, INPUT_PULLUP);
    pinMode(kAA_Sw, INPUT_PULLUP);
    pinMode(kNAV_Sw, INPUT_PULLUP);
    pinMode(kAG_Sw, INPUT_PULLUP);
    pinMode(kRotarySw, INPUT_PULLUP);

    digitalWrite(led_order[i], HIGH);

    Serial.begin(9600);
}

void loop()
{
    unsigned long frame_start_time_ms = millis();

    rotaryEnc.process_encoder();

    if (!digitalRead(kMasterCautionSw) ||
        !digitalRead(kAA_Sw) ||
        !digitalRead(kNAV_Sw) ||
        !digitalRead(kAG_Sw) ||
        !digitalRead(kRotarySw))
    {
        advance_led();
        delay(100);
    }

    while (millis() - frame_start_time_ms < kFrameTime_ms)
    {
        // Wait.
    }
}
