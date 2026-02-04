#pragma once

#include <Joystick.h>
#include <PCF8575.h>
#include <stdint.h>

#define NUM_PINS_PER_IO_EXPANDER 16

class DigitalIOExpander {
public:
  DigitalIOExpander();

  void setup(uint8_t address, int num_leds, int joystick_start_index);

  // ******* LED OUTPUT FUNCTIONS *******

  // Set the value of a single LED
  void set_led(uint8_t pin_id, uint8_t value);

  // Set the value of all LEDs
  void set_all_led(uint8_t value);

  // ******* BUTTON INPUT FUNCTIONS *******

  // Reads all button inputs and sets them on the joystick,
  // however requires an external call to joystick.sendState().
  void readAndSetAllButtons(Joystick_ &joystick);

  // Loop function that monitors button inputs and sends a joystick
  // state update upon a change.
  void sendStateUpdateOnButtonChange(Joystick_ &joystick);

private:
  PCF8575 gpio_board_;
  int num_led_outputs_;
  int joystick_start_index_;
  int last_button_state_[16] = {0};
};
