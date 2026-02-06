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
};

class AnalogMultiplexer {
public:
  AnalogMultiplexer(const AnalogMultiplexerConfig &config);

  void readAnalogInputs();

  void sendStateUpdateOnAnalogInputChange();

private:
  int analog_inputs_filtered_[MAX_NUM_MULTIPLEXERS * NUM_PINS_PER_MULTIPLEXER] =
      {0};
  int analog_inputs_last_sent_[MAX_NUM_MULTIPLEXERS *
                               NUM_PINS_PER_MULTIPLEXER] = {0};
  int num_analog_inputs_;
  HC4067 multiplexer_driver_;
  int mux_input_pins_[MAX_NUM_MULTIPLEXERS];
  int num_mux_inputs_;
};

// namespace AnalogMultiplexer {
//
// void init(int analog_inputs_raw[], int analog_inputs_filtered[],
//           const int num_inputs);
//
// void readAnalogInputs(HC4067 &multiplexer, int &analog_inputs_raw,
//                       const int &mux_input_pins, int num_mux_inputs);
//
// void filterAnalogInputs(int &analog_inputs_raw, int &analog_inputs_filtered,
//                         int num_inputs);
// } // namespace AnalogMultiplexer
