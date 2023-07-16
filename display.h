#ifndef DISPLAY_H // this is a header guard
#define DISPLAY_H

#include "commonlib.h"
#include <Arduino.h>

// pin 7-segment display pins
#define PARALLEL_CLK 8 //RCLK
#define SERIAL_CLK 7 //SRCLK
#define SERIAL_IN 6 //SER

typedef enum FlashingState {
  ALL_ON = -2,
  ALL_OFF = -1,
  ONES = 0, // the digit in ones place flashes/blinks, while other digits are ON
  TENS = 1,
  HUNDREDS = 2,
  THOUSANDS = 3,
} FlashingState;

#define MOST_SIGNIFICANT THOUSANDS
//determines the number of decimal places shown:
//since for 1 decimal place, the decimal point is on the right of TENS place
#define ONE_DECIMAL_PLACE TENS
#define TWO_DECIMAL_PLACE HUNDREDS
#define THREE_DECIMAL_PLACE THOUSANDS
#define ZERO_DECIMAL_PLACE ONES
#define NO_DECIMAL_PLACE ALL_OFF

#define DISPLAY_DIGIT_OFF 10
#define NEGATIVE_SIGN 11
#define SHIFT_REGISTER_SIZE 8
#define BLINK_TIME 1200

class Display {
  public:
    Display();
    void Display::setDecimalPlace(FlashingState decimalPlace);
    void display_number(int number);
    void flash_digit(FlashingState whichDigit);
    void print_info();
    String Display::flashing_state_string(FlashingState flashingState);
    
  private:
    void setup_timer(int frequency);
    unsigned long lastUpdateTime;
    String Display::flashing_state_string_helper(FlashingState flashingState);
};

#endif
