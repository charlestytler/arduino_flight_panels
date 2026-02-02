#pragma once

#include "DigitalIOExpander.h"

class DigitalIOExpander {
public:
  DigitalIOExpander() : gpio_board_() {}

  void set_configuration(GpioConfig &config) {
    uint8_t pcf_id = 0b100000 | config.pcf_address;
    gpio_board_.setAddress(pcf_id);

#ifdef DEBUG
    Serial.print("PCF ID: ");
    Serial.print(pcf_id, BIN);
    Serial.print("\t");
    Serial.println(pcf_id, HEX);
#endif

    /*
    Button mask for PCF8574 library:
    Bits set to 1 will set those pins HIGH then LOW to read button switch
    status. Bits set to 0 will be unaffected during the buttonRead8() call (good
    for LED outputs).
    */
    constexpr int NUM_PINS = 16;
    uint16_t button_mask = 0U;
    for (int i = 0; i < NUM_PINS; i++) {
      // LED pins are assigned to the end of the pin mask.
      bool is_led = NUM_PINS - i <= config.num_leds;
      bitWrite(button_mask, i, is_led ? 0 : 1);
    }

    gpio_board_.setButtonMask(button_mask);

#ifdef DEBUG
    Serial.print("Mask: ");
    Serial.println(button_mask, BIN);
#endif

    joystick_start_index_ = config.joystick_start_index;
  }

  void set_led(const uint8_t pin_id, const uint8_t value) {
    gpio_board_.write(pin_id, value);
  }

  void set_all_led(const uint16_t value) { gpio_board_.write16(value); }

  void setup(GpioConfig &config) {
    set_configuration(config);
    gpio_board_.begin();
  }

  void loop(Joystick_ &joystick) {
    uint16_t pin_values = gpio_board_.readButton16();
#ifdef DEBUG
    Serial.print("B");
    Serial.print(gpio_board_.getAddress(), BIN);
    Serial.print(": ");
    Serial.print(pin_values, BIN);
    Serial.print(" ");
#endif

    for (int i = 0; i < 16; i++) {
      if (bitRead(gpio_board_.getButtonMask(), i)) {
        joystick.setButton(joystick_start_index_ + i,
                           !(bitRead(pin_values, i)));
      }
    }
  }

private:
  PCF8575 gpio_board_;
  int joystick_start_index_;
};
