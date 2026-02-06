#include "DigitalIOExpander.h"

DigitalIOExpander::DigitalIOExpander() : gpio_board_() {}

void DigitalIOExpander::setup(uint8_t address, int num_leds,
                              int joystick_start_index) {
  gpio_board_.setAddress(address);

  num_led_outputs_ = num_leds;
  joystick_start_index_ = joystick_start_index;

  /*
  Button mask for PCF8574 library:
  Bits set to 1 will set those pins HIGH then LOW to read button switch
  status. Bits set to 0 will be unaffected during the buttonRead8() call (good
  for LED outputs).
  */
  uint16_t button_mask = 0U;
  for (int i = 0; i < NUM_PINS_PER_IO_EXPANDER; i++) {
    // LED pins are assigned to the end of the pin mask.
    bool is_led = NUM_PINS_PER_IO_EXPANDER - i <= num_led_outputs_;
    bitWrite(button_mask, i, is_led ? 0 : 1);
  }
  gpio_board_.setButtonMask(button_mask);

  // Can run board after address and button_mask are set.
  gpio_board_.begin();
}

// ******* LED OUTPUT FUNCTIONS *******

void DigitalIOExpander::set_led(uint8_t pin_id, uint8_t value) {
  gpio_board_.write(pin_id, value);
}

void DigitalIOExpander::set_all_led(uint8_t value) {
  gpio_board_.write16(value);
}

// ******* BUTTON INPUT FUNCTIONS *******

void DigitalIOExpander::readAndSetAllButtons(Joystick_ &joystick) {
  const uint16_t pin_values = gpio_board_.readButton16();

  for (int i = 0; i < NUM_PINS_PER_IO_EXPANDER - num_led_outputs_; i++) {
    last_button_state_[i] = bitRead(pin_values, i);
    joystick.setButton(joystick_start_index_ + i, last_button_state_[i]);
  }
}

void DigitalIOExpander::sendStateUpdateOnButtonChange(Joystick_ &joystick) {
  const uint16_t pin_values = gpio_board_.readButton16();

  for (int i = 0; i < NUM_PINS_PER_IO_EXPANDER - num_led_outputs_; i++) {
    const int current_button_state = bitRead(pin_values, i);
    // Only send update if button state has changed.
    if (current_button_state != last_button_state_[i]) {
      // Send update immediately upon button state change.
      joystick.setButton(joystick_start_index_ + i, current_button_state);
      joystick.sendState();

      // Also send update over serial.
      Serial.println("BTN" + String(i) + "," + String(current_button_state));

      last_button_state_[i] = current_button_state;
    }
  }
}
