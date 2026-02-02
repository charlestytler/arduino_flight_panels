#pragma once

#include <Joystick.h>
#include <PCF8575.h>
#include <stdint.h>

typedef struct {
  uint8_t pcf_address; // Expected as binary of form 0b<A0><A1><A2>
  uint16_t num_leds;
  uint8_t joystick_start_index;
} GpioConfig;

class DigitalIOExpander {
public:
  DigitalIOExpander() : gpio_board_() {}

  void set_configuration(GpioConfig &config);

  void set_led(const uint8_t pin_id, const uint8_t value);

  void set_all_led(const uint16_t value);

  void setup(GpioConfig &config);

  void loop(Joystick_ &joystick);

private:
  PCF8575 gpio_board_;
  int joystick_start_index_;
};
