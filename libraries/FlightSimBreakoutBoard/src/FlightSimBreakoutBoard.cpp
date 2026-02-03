#include "FlightSimBreakoutBoard.h"

FlightSimBreakoutBoard::FlightSimBreakoutBoard(
    const FlightSimBreakoutBoardConfig &config)
    : joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                config.digital_io_expanders_config.num_expanders * 16 -
                    config.digital_io_expanders_config.num_leds,
                0, false, false, false, false, false, false, false, false,
                false, false, false) {}

void FlightSimBreakoutBoard::setLED(int led_id, uint8_t value) {
  constexpr int NUM_PINS = 16;
  const int expander_number = led_id / NUM_PINS;
  const int pin_number = led_id % NUM_PINS;
  // LEDs are ordered from last pin back, so reverse order for indexing.
  const int expander_id = num_digital_io_expanders_ - 1 - expander_number;
  const int pin_id = NUM_PINS - 1 - pin_number;

  digital_io_expanders_[expander_id].set_led(pin_id, value);
}

void FlightSimBreakoutBoard::turnOffAllLEDs() {
  for (int i = 0; i < num_digital_io_expanders_; i++) {
    digital_io_expanders_[i].set_all_led(0);
  }
}

void FlightSimBreakoutBoard::turnOnAllLEDs() {
  for (int i = 0; i < num_digital_io_expanders_; i++) {
    digital_io_expanders_[i].set_all_led(1);
  }
}

void FlightSimBreakoutBoard::setup(const FlightSimBreakoutBoardConfig &config) {
  constexpr bool AUTO_SEND_STATE = false;
  joystick_.begin(AUTO_SEND_STATE);

  Wire.begin();
  setupDigitalIOExpanders(config.digital_io_expanders_config);

  // Send intial state of all buttons, and turn off all LEDs.
  joystick_.sendState();
  turnOffAllLEDs();
}

void FlightSimBreakoutBoard::loop() {
  for (int i = 0; i < num_digital_io_expanders_; i++) {
    digital_io_expanders_[i].sendStateUpdateOnButtonChange(joystick_);
  }
}

void FlightSimBreakoutBoard::setupDigitalIOExpanders(
    const DigitalIOExpandersConfig &config) {
  constexpr int NUM_PINS = 16;
  int joystick_start_index_for_current_expander = 0;

  // Iterate backwards through the expanders count to allocate the number of
  // LEDs on each expander.
  int num_leds_per_expander[8] = {0};
  int num_total_leds_to_allocate = config.num_leds;
  int backwards_idx = config.num_expanders - 1;
  while (num_total_leds_to_allocate > 0) {
    num_leds_per_expander[backwards_idx] =
        min(NUM_PINS, num_total_leds_to_allocate);
    num_total_leds_to_allocate -= num_leds_per_expander[backwards_idx];
    backwards_idx--;
  }

  for (int i = 0; i < config.num_expanders; i++) {
    digital_io_expanders_[i] = DigitalIOExpander();
    digital_io_expanders_[i].setup(config.expander_i2c_addresses[i],
                                   num_leds_per_expander[i],
                                   joystick_start_index_for_current_expander);

    joystick_start_index_for_current_expander += NUM_PINS;

    digital_io_expanders_[i].readAndSetAllButtons(joystick_);
  }
}
