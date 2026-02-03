#include <FlightSimBreakoutBoard.h>


constexpr int NUM_GPIO_EXPANDERS = 1;
const uint8_t expander_i2c_addresses[NUM_GPIO_EXPANDERS] = {0x20};
constexpr int NUM_LEDS = 0;
const DigitalIOExpandersConfig digital_io_expanders_config = {expander_i2c_addresses, NUM_GPIO_EXPANDERS, NUM_LEDS};

const FlightSimBreakoutBoardConfig config = {digital_io_expanders_config};

FlightSimBreakoutBoard flightSimBreakoutBoard(config);

void setup()
{
  flightSimBreakoutBoard.setup(config);
}

void loop()
{
  flightSimBreakoutBoard.loop();
}
