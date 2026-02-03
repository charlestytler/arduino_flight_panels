#include <FlightSimBreakoutBoard.h>

constexpr int NUM_GPIO_EXPANDERS = 1;
constexpr int NUM_LEDS = 0;
const DigitalIOExpandersConfig digital_io_expanders_config = {
    {0x20}, // expander_i2c_addresses
    NUM_GPIO_EXPANDERS,
    NUM_LEDS};

const FlightSimBreakoutBoardConfig config = {digital_io_expanders_config};

FlightSimBreakoutBoard flightSimBreakoutBoard(config);

void setup() { flightSimBreakoutBoard.setup(config); }

void loop() { flightSimBreakoutBoard.loop(); }
