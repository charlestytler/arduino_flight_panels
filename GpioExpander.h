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
    GpioExpander(const int A2, const int A1, const int A0, const std::vector<GpioType> io_pins, const int joystick_start_index)
    {

        uint8_t pcf_id = 0U;
        pcf_id << A2 << A1 << A0;
        gpio_board_.setAddress(pcf_id);

        io_pins_ = io_pins;

        const int NUM_PINS = 8;
        uint8_t button_mask = 0U;
        for (int i = 0; i < NUM_PINS; i++)
        {
            if (io_pins[i] == L || io_pins[i] == X)
            {
                button_mask <<= 0;
            }
            else
            {
                button_mask <<= 1;
            }
        }
        gpio_board_.setButtonMask(button_mask);

        joystick_start_index_ = joystick_start_index;
    }

    void set_led(const uint8_t pin_id, const uint8_t value) {
        gpio_board_.write(pin_id, value);
    }

    void set_all_led(const uint8_t value) {
        for (int i=0; i<output_pins_.length(); i++) {
            set_led(i, value);
        }

    }

    void setup() {
        gpio_board_.begin();
    }

    void loop(Joystick_ &joystick){
        uint8_t pin_values = gpio_board_.readButton8();
        int bit_of_interest = 1;
        for (int i=0; i<8; i++) {
            if (gpio_board_.getButtonMask() & bit_of_interest){
                joystick.setButton(joystick_start_index_ + i, pin_values & bit_of_interest);
            }
            bit_of_interest <<= 0;
        }
    }

private:
    PCF8574 gpio_board_;
    int joystick_start_index_;
};
