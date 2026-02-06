#include <FlightSimBreakoutBoard.h>

constexpr FlightSimBreakoutBoardConfig config{
    .digital_io_expanders_config =
        {
            .expander_i2c_addresses = {0x20},
            .num_expanders = 1, // num_expanders
            .num_leds = 0,      // num_leds
        },
    .analog_multiplexer_config = {
        .pins = {.s0 = 15, .s1 = 14, .s2 = 16, .s3 = 10}, // Channel Select pins
        .num_mux_inputs = 4,
    }};

// const FlightSimBreakoutBoardConfig config{
//     {
//         // digital_io_expanders_config
//         {0x20},
//         1, // num_expanders
//         0, // num_leds
//     },
//     {
//         // analog_multiplexer_config
//         {A0, A1, A2, A3}, // Channel Select pins
//         4,                // num_mux_inputs
//     }};

FlightSimBreakoutBoard flightSimBreakoutBoard(config);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  flightSimBreakoutBoard.setup(config);
}

void loop() { flightSimBreakoutBoard.loop(); }

//
//
// Checks on configuration.

static_assert(config.digital_io_expanders_config.num_leds <=
                  NUM_PINS_PER_IO_EXPANDER *
                      config.digital_io_expanders_config.num_expanders,
              "More LEDs than available pins.");

static_assert(config.digital_io_expanders_config.num_expanders <=
                  MAX_NUM_DIGITAL_IO_EXPANDERS,
              "More expanders than number of I2C addresses available.");

static_assert(config.analog_multiplexer_config.num_mux_inputs <=
                  MAX_NUM_MULTIPLEXERS,
              "More mux inputs than max configuration supports.");
