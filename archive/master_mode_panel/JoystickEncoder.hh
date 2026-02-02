#ifndef H_JoystickEncoder
#define H_JoystickEncoder

#include <Encoder.h>

// Describes a rotary knob turn direction.
enum class TurnDirection
{
    kNone,
    kLeft,
    kRight
};

class JoystickEncoder
{
public:
    explicit JoystickEncoder(int pin1, int pin2);

    TurnDirection process_encoder();

private:
    bool debounce_signal(long new_reading, int delta_position);

    bool debounce_has_settled(int delta_position);

    TurnDirection determine_button_press(int delta_position);

    TurnDirection detect_fast_turns_during_debounce(int delta_position);

    void reset_fast_turns_detector_variables();

    Encoder encoder_monitor_;               /// Instance of Encoder.
    long debounced_encoder_position_{0};    /// Stored position of encoder.
    long prev_encoder_position_reading_{0}; /// Undebounced position of encoder.

    // Internal varialbes for debouncing.
    int debounce_counter_{0}; /// Count-down counter used to debounce and suppress joystick output.

    // Internal varialbes for detecting fast turns.
    int num_fast_turns_detected_{0};                           /// Keeps track of number of whole turns detected while debouncing.
    TurnDirection last_turn_direction_ = TurnDirection::kNone; /// Keeps track of last turn direction in case of change of direction during debounce.

    // Constant parameters.
    static constexpr int kDebounceFrames_{3};         /// Parameter for number of frames the signal must be steady before considered debounced.
    static constexpr int kPositionTicksPerDetent_{4}; /// Parameter for number of position ticks that represent one detent of rotary knob.
};

#endif
