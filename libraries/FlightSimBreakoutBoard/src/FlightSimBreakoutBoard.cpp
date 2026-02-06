#include "FlightSimBreakoutBoard.h"

FlightSimBreakoutBoard::FlightSimBreakoutBoard(
    const FlightSimBreakoutBoardConfig &config)
    : joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                config.digital_io_expanders_config.num_expanders *
                        NUM_PINS_PER_IO_EXPANDER -
                    config.digital_io_expanders_config.num_leds,
                0, false, false, false, false, false, false, false, false,
                false, false, false),
      analog_multiplexer_(config.analog_multiplexer_config) {}

void FlightSimBreakoutBoard::setup(const FlightSimBreakoutBoardConfig &config) {
  constexpr bool AUTO_SEND_STATE = false;
  joystick_.begin(AUTO_SEND_STATE);

  Wire.begin();
  setupDigitalIOExpanders(config.digital_io_expanders_config);
  // Analog multiplexer needs no setup.

  // Send intial state of all buttons, and turn off all LEDs.
  delay(5000); // Wait for all USB handshakes to complete.
  joystick_.sendState();
  turnOffAllLEDs();
}

void FlightSimBreakoutBoard::setLED(int led_id, uint8_t value) {
  const int expander_number = led_id / NUM_PINS_PER_IO_EXPANDER;
  const int pin_number = led_id % NUM_PINS_PER_IO_EXPANDER;
  // LEDs are ordered from last pin back, so reverse order for indexing.
  const int expander_id = num_digital_io_expanders_ - 1 - expander_number;
  const int pin_id = NUM_PINS_PER_IO_EXPANDER - 1 - pin_number;

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

void FlightSimBreakoutBoard::loop(const FlightSimBreakoutBoardConfig &config) {
  for (int i = 0; i < num_digital_io_expanders_; i++) {
    digital_io_expanders_[i].sendStateUpdateOnButtonChange(joystick_);
  }
  analog_multiplexer_.sendStateUpdateOnAnalogInputChange(
      config.analog_multiplexer_config);
}

void FlightSimBreakoutBoard::setupDigitalIOExpanders(
    const DigitalIOExpandersConfig &config) {
  num_digital_io_expanders_ = config.num_expanders;
  int joystick_start_index_for_current_expander = 0;

  // Iterate backwards through the expanders count to allocate the number of
  // LEDs on each expander.
  int num_leds_per_expander[8] = {0};
  int num_total_leds_to_allocate = config.num_leds;
  int backwards_idx = num_digital_io_expanders_ - 1;
  while (num_total_leds_to_allocate > 0) {
    num_leds_per_expander[backwards_idx] =
        min(NUM_PINS_PER_IO_EXPANDER, num_total_leds_to_allocate);
    num_total_leds_to_allocate -= num_leds_per_expander[backwards_idx];
    backwards_idx--;
  }

  for (int i = 0; i < num_digital_io_expanders_; i++) {
    digital_io_expanders_[i] = DigitalIOExpander();
    digital_io_expanders_[i].setup(config.expander_i2c_addresses[i],
                                   num_leds_per_expander[i],
                                   joystick_start_index_for_current_expander);

    joystick_start_index_for_current_expander += NUM_PINS_PER_IO_EXPANDER;

    digital_io_expanders_[i].readAndSetAllButtons(joystick_);
  }
}
