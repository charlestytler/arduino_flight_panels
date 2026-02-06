#pragma once

#include "subcomponents/AnalogMultiplexer.h"
#include "subcomponents/DigitalIOExpander.h"
#include <Joystick.h>

#define MAX_NUM_DIGITAL_IO_EXPANDERS 8 // PCF8575 has max 8 I2C addresses

struct DigitalIOExpandersConfig {
  uint8_t expander_i2c_addresses[8];
  int num_expanders;
  int num_leds;
};

struct FlightSimBreakoutBoardConfig {
  DigitalIOExpandersConfig digital_io_expanders_config;
  AnalogMultiplexerConfig analog_multiplexer_config;
};

class FlightSimBreakoutBoard {
public:
  explicit FlightSimBreakoutBoard(const FlightSimBreakoutBoardConfig &config);

  // Set the value of a single LED
  void setLED(int led_id, uint8_t value);

  // Set the value of all LEDs
  void turnOffAllLEDs();
  void turnOnAllLEDs();

  // Assumptions:
  // num_leds < number of digital IO expanders * NUM_PINS_PER_IO_EXPANDER (16)
  void setup(const FlightSimBreakoutBoardConfig &config);

  void loop();

private:
  void setupDigitalIOExpanders(const DigitalIOExpandersConfig &config);
  void setupAnalogMultiplexer();

  Joystick_ joystick_;
  DigitalIOExpander digital_io_expanders_[MAX_NUM_DIGITAL_IO_EXPANDERS];
  int num_digital_io_expanders_;
  AnalogMultiplexer analog_multiplexer_;
};
