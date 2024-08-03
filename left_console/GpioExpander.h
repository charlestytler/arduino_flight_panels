#pragma once

#include <Joystick.h>
#include <PCF8574.h>

enum GpioType {
    S, // Single pole input
    D, // Double pole input
    M, // Multiple pole (e.g. rotary switch) input
    L, // LED output
    X  // Unused
};

typedef struct {
    uint8_t pcf_address; // Expected as binary of form 0b<A0><A1><A2>
    GpioType pins[8];
    int joystick_start_index;
} GpioConfig;

class GpioExpander
{
  public:
    GpioExpander() : gpio_board_() {}

    void set_configuration(GpioConfig &config)
    {
        uint8_t pcf_id = 0b100000 | config.pcf_address;
#ifdef DEBUG
        Serial.print("PCF ID: ");
        Serial.print(pcf_id, BIN);
        Serial.print("\t");
        Serial.println(pcf_id, HEX);
#endif
        gpio_board_.setAddress(pcf_id);

        /*
        Button mask for PCF8574 library:
        Bits set to 1 will set those pins HIGH then LOW to read button switch status.
        Bits set to 0 will be unaffected during the buttonRead8() call (good for LED outputs).
        */
        constexpr int NUM_PINS = 8;
        uint8_t button_mask = 0U;
        for (int i = 0; i < NUM_PINS; i++) {
            if (config.pins[i] == S || config.pins[i] == D || config.pins[i] == M) {
                bitWrite(button_mask, i, 1);
            }
        }
        gpio_board_.setButtonMask(button_mask);
#ifdef DEBUG
        Serial.print("Mask: ");
        Serial.println(button_mask, BIN);
#endif

        joystick_start_index_ = config.joystick_start_index;
    }

    void set_led(const uint8_t pin_id, const uint8_t value) { gpio_board_.write(pin_id, value); }

    void set_all_led(const uint8_t value) { gpio_board_.write8(0); }

    void setup(GpioConfig &config)
    {
        set_configuration(config);
        gpio_board_.begin();
    }

    void loop(Joystick_ &joystick)
    {
        uint8_t pin_values = gpio_board_.readButton8();
#ifdef DEBUG
        Serial.print("Board ");
        Serial.print(gpio_board_.getAddress(), HEX);
        Serial.print("  Values: ");
        Serial.println(pin_values, BIN);
#endif

        for (int i = 0; i < 8; i++) {
            if (bitRead(gpio_board_.getButtonMask(), i)) {
                joystick.setButton(joystick_start_index_ + i, !(bitRead(pin_values, i)));
            }
        }
    }

  private:
    PCF8574 gpio_board_;
    int joystick_start_index_;
};
