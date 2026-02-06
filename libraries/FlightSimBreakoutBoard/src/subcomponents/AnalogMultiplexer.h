#include <HC4067.h>

#define MAX_NUM_MULTIPLEXERS 5 // Board has 5 analog input pins available
#define NUM_PINS_PER_MULTIPLEXER 16

struct MuxChannelSelectPins {
  uint8_t s0;
  uint8_t s1;
  uint8_t s2;
  uint8_t s3;
};

struct AnalogMultiplexerConfig {
  MuxChannelSelectPins pins;
  int num_mux_inputs;
  int mux_input_pins[MAX_NUM_MULTIPLEXERS];
};

class AnalogMultiplexer {
public:
  AnalogMultiplexer(const AnalogMultiplexerConfig &config);

  void readAnalogInputs(const AnalogMultiplexerConfig &config);

  void
  sendStateUpdateOnAnalogInputChange(const AnalogMultiplexerConfig &config);

private:
  HC4067 multiplexer_driver_;
  int analog_inputs_filtered_[MAX_NUM_MULTIPLEXERS * NUM_PINS_PER_MULTIPLEXER] =
      {0};
  int analog_inputs_last_sent_[MAX_NUM_MULTIPLEXERS *
                               NUM_PINS_PER_MULTIPLEXER] = {0};
};
