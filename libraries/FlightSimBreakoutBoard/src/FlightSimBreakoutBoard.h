#pragma once

#include "subcomponents/DigitalIOExpander.h"
#include <Joystick.h>
#include <vector>

class FlightSimBreakoutBoard {
public:
  FlightSimBreakoutBoard();

  // Set the value of a single LED
  void setLED(int led_id, uint8_t value);

  // Set the value of all LEDs
  void turnOffAllLEDs();
  void turnOnAllLEDs();

  // Assumptions:
  // num_leds < number of digital IO expanders * NUM_PINS (16)
  void setup(const std::vector<uint8_t> &expander_i2c_addresses, int num_leds);

  void loop();

private:
  Joystick_ joystick_;
  std::vector<DigitalIOExpander> digital_io_expanders_;
};
