#include "JoystickEncoder.hh"

JoystickEncoder::JoystickEncoder(int pin1, int pin2) : encoder_monitor_(pin1, pin2)
{
    encoder_monitor_.write(0);
}

TurnDirection JoystickEncoder::process_encoder()
{
    const long encoder_position_reading = encoder_monitor_.read();
    const int delta_position = encoder_position_reading - debounced_encoder_position_;
    const bool debounce_has_settled = debounce_signal(encoder_position_reading, delta_position);

    TurnDirection button_press_output = TurnDirection::kNone;
    if (debounce_has_settled)
    {
        const int rounding_offset = kPositionTicksPerDetent_ / 2; // Better approximates rounding in integer division.
        const int num_detents_turned = (abs(delta_position) + rounding_offset) / kPositionTicksPerDetent_;
        if (num_detents_turned > num_fast_turns_detected_)
        {
            button_press_output = determine_button_press(delta_position);
        }
        reset_fast_turns_detector_variables();
    }
    else
    {
        button_press_output = detect_fast_turns_during_debounce(delta_position);
    }

    return button_press_output;
}

bool JoystickEncoder::debounce_signal(long new_reading, int delta_position)
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

bool JoystickEncoder::debounce_has_settled(int delta_position)
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

TurnDirection JoystickEncoder::determine_button_press(int delta_position)
{
    if (delta_position > 0)
    {
        return TurnDirection::kRight;
    }
    else if (delta_position < 0)
    {
        return TurnDirection::kLeft;
    }
}

TurnDirection JoystickEncoder::detect_fast_turns_during_debounce(int delta_position)
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

    TurnDirection button_press_output = TurnDirection::kNone;
    const int next_fast_turn_threshold = (num_fast_turns_detected_ + 1) * kPositionTicksPerDetent_ + 1;
    if (abs(delta_position) > next_fast_turn_threshold)
    {
        button_press_output = determine_button_press(delta_position);
        num_fast_turns_detected_++;
    }

    return button_press_output;
}

void JoystickEncoder::reset_fast_turns_detector_variables()
{
    num_fast_turns_detected_ = 0;
    last_turn_direction_ = TurnDirection::kNone;
}
