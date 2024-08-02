#pragma once

#include <vector>

#include <PCF8574.h>
#include <Joystick.h>

enum GpioType
{
    S, // Single pole input
    D, // Double pole input
    M, // Multiple pole (e.g. rotary switch) input
    L, // LED output
    X  // Unused
};

class GpioExpander
{
public:
    GpioExpander(const int A2, const int A1, const int A0, const std::vector<GpioType> io_pins, const int joystick_start_index);

    void set_led(const uint8_t pin_id, const uint8_t value);
    void set_all_led(const uint8_t value);

    void setup();
    void loop(const Joystick_ &joystick);

private:
    PCF8574 gpio_board_;
    int joystick_start_index_;

    /* May be needed for tracking multi-pole switches
    std::vector<GpioType> io_pins_;
    std::vector<int> pin_values_;
    std::vector<int> input_pins_;
    std::vector<int> led_output_pins_;
    GpioType prev_pin_type_;
    int pole_count_;
    */
};

// PCF address is 0b100<A2><A1><A0>
GpioExpander engine_start_panel = {0x22, {BUTTON_2, BUTTON_3, BUTTON_2, BUTTON_3, BUTTON_2, NONE, NONE, LED}};
GpioExpander flt_ctrl_panel = {0x20, {LED, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2}};
