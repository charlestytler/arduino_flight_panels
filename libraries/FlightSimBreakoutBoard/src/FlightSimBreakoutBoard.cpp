#include "FlightSimBreakoutBoard.h"

namespace {

void configureJoystick(int num_digital_io_expanders, int num_leds,
                       Joystick_ &joystick) {
  const uint8_t buttonCount = num_digital_io_expanders * 16 - num_leds;
  const uint8_t hatSwitchCount = 0;
  Joystick_ joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                     buttonCount, hatSwitchCount,
                     false, // include axes
                     false, false, false, false, false, false, false, false,
                     false, false);
}

void setupDigitalIOExpanders(const std::vector<uint8_t> &expander_i2c_addresses,
                             int num_leds) {
  constexpr int NUM_PINS = 16;
  const num_expanders = expander_i2c_addresses.size();
  int joystick_start_index_for_current_expander = 0;

  // Iterate backwards through the expanders count to allocate the number of
  // LEDs on each expander.
  std::vector<int> num_leds_per_expander(num_expanders, 0);
  int num_total_leds_to_allocate = num_leds;
  int backwards_idx = num_expanders - 1;
  while (num_total_leds_to_allocate > 0) {
    num_leds_per_expander[backwards_idx] =
        std::min(NUM_PINS, num_total_leds_to_allocate);
    num_total_leds_to_allocate -= num_leds_per_expander[backwards_idx];
    backwards_idx--;
  }

  for (int i = 0; i < num_expanders; i++) {
    digital_io_expanders_.push_back(DigitalIOExpander());
    digital_io_expanders_.back().setup(
        pcf_addresses[i], num_leds_per_expander[i],
        joystick_start_index_for_current_expander);

    joystick_start_index_for_current_expander += NUM_PINS;

    digital_io_expanders_.back().readAndSetAllButtons(joystick_);
  }
}
} // namespace

FlightSimBreakoutBoard::FlightSimBreakoutBoard() {}

void FlightSimBreakoutBoard::setLED(int led_id, uint8_t value) {
  constexpr int NUM_PINS = 16;
  const int expander_number = led_id / NUM_PINS;
  const int pin_number = led_id % NUM_PINS;
  // LEDs are ordered from last pin back, so reverse order for indexing.
  const int expander_id = digital_io_expanders_.size() - 1 - expander_number;
  const int pin_id = NUM_PINS - 1 - pin_number;

  digital_io_expanders_[expander_id].set_led(pin_id, value);
}

void FlightSimBreakoutBoard::turnOffAllLEDs() {
  for (int i = 0; i < digital_io_expanders_.size(); i++) {
    digital_io_expanders_[i].set_all_led(0);
  }
}

void FlightSimBreakoutBoard::turnOnAllLEDs() {
  for (int i = 0; i < digital_io_expanders_.size(); i++) {
    digital_io_expanders_[i].set_all_led(1);
  }
}

void FlightSimBreakoutBoard::setup(
    const std::vector<uint8_t> &expander_i2c_addresses, int num_leds) {
  constexpr bool AUTO_SEND_STATE = false;
  joystick_.begin(AUTO_SEND_STATE);

  Wire.begin();
  setupDigitalIOExpanders(expander_i2c_addresses, num_leds);

  // Send intial state of all buttons, and turn off all LEDs.
  joystick_.sendState();
  turnOffAllLEDs();
}

void FlightSimBreakoutBoard::loop() {
  for (int i = 0; i < digital_io_expanders_.size(); i++) {
    digital_io_expanders_[i].sendStateUpdateOnButtonChange(joystick_);
  }
}
