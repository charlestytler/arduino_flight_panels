#include <PCF8574.h>

PCF8574 gpio22(0b100000 | 0b010);
PCF8574 gpio20(0b100000 | 0b000);

void setup()
{
    Serial.begin(115200);
    Serial.println("START");

    gpio22.begin();
    gpio22.setButtonMask(0b01111111);
    gpio22.write(7, 0);

    gpio20.begin();
    gpio20.setButtonMask(0b11111110);
    gpio20.write(0, 0);
}

void loop()
{
    // unsigned long frame_start_time_ms = millis();

    uint8_t pins = gpio22.readButton8();
    uint8_t pins_20 = gpio20.readButton8();
    if (pins & 0b00010000) {
        gpio22.write(7, 1);
    } else {
        gpio22.write(7, 0);
    }

    if (pins_20 & 0b00000010) {
        gpio20.write(0, 0);
    } else {
        gpio20.write(0, 1);
    }
    Serial.print(pins, BIN);
    Serial.print("  ");
    Serial.println(pins_20, BIN);
    delay(10);
}