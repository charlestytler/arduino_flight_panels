#pragma once

#include <FastLED.h>

#define LED_PIN 3
#define NUM_LEDS 96

enum COLOR {
    GREEN = 0,
    RED,
    YELLOW,
    ORANGE,
};

CRGB leds[NUM_LEDS];

unsigned int get_led_id(unsigned int row, unsigned int col) { return row * 8 + col; }

auto get_led_color(enum COLOR color)
{
    switch (color) {
    case GREEN:
        return CRGB(0, 150, 0);
    case RED:
        return CRGB(255, 0, 0);
    case YELLOW:
        return CRGB(255, 191, 0);
    case ORANGE:
        return CRGB(255, 120, 0);
    }
}

void set_caution_panel_led(unsigned int row, unsigned int col, COLOR color)
{
    leds[get_led_id(row, col)] = get_led_color(color);
}
