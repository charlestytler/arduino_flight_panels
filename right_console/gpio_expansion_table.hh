
enum GpioType { NONE = 0, BUTTON_2, BUTTON_3, LED };

typedef struct {
    uint8_t pcf_id;
    GpioType io[8];
} GpioExpander;

// PCF address is 0b100<A2><A1><A0>
GpioExpander engine_start_panel = {0x22, {BUTTON_2, BUTTON_3, BUTTON_2, BUTTON_3, BUTTON_2, NONE, NONE, LED}};
GpioExpander flt_ctrl_panel = {0x20, {LED, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2, BUTTON_2}};
