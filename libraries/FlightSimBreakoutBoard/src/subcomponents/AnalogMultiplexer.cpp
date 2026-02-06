#include "AnalogMultiplexer.h"

AnalogMultiplexer::AnalogMultiplexer(const AnalogMultiplexerConfig &config)
    : multiplexer_driver_{config.pins.s0, config.pins.s1, config.pins.s2,
                          config.pins.s3} {}

void AnalogMultiplexer::readAnalogInputs(
    const AnalogMultiplexerConfig &config) {
  for (int i = 0; i < NUM_PINS_PER_MULTIPLEXER; i++) {
    // Set multiplexer to select pin i
    multiplexer_driver_.setChannel(i);

    // Read from each mux input and filter.
    constexpr float alpha = 0.8;

    for (int j = 0; j < config.num_mux_inputs; j++) {
      int curr = i + j * NUM_PINS_PER_MULTIPLEXER;
      analog_inputs_filtered_[curr] =
          static_cast<int>((1 - alpha) * analog_inputs_filtered_[curr] +
                           alpha * analogRead(config.mux_input_pins[j]));
    }
  }
}

void AnalogMultiplexer::sendStateUpdateOnAnalogInputChange(
    const AnalogMultiplexerConfig &config) {
  // This is a constant as most analog inputs are the same or similar B10K
  // potentiometers. If a variety of anoalog inputs are used, it may be worth
  // reviewing making this configurable.
  constexpr int ANALOG_INPUT_CHANGE_DEADBAND = 4;

  for (int i = 0; i < NUM_PINS_PER_MULTIPLEXER * config.num_mux_inputs; i++) {
    if (abs(analog_inputs_filtered_[i] - analog_inputs_last_sent_[i]) >
        ANALOG_INPUT_CHANGE_DEADBAND) {

      // TODO: This will be replaced with a passed in messaging protocol to use.
      Serial.println("POT,#" + String(i) + "," +
                     String(analog_inputs_filtered_[i]));

      analog_inputs_last_sent_[i] = analog_inputs_filtered_[i];
    }
  }
}
