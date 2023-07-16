#include "display.h"

// global variables and functions for the timer interrupt
static int volatile displayedNum; //is an integer. To diplay a float like 5.37, displayNum = 537 and set atDecimalPt = TWO_DECIMAL_PLACE
static volatile FlashingState flashingDigitSelect;
static void display_digit(int digitValue, int whichDigit, bool hasDecimalPt);
static volatile FlashingState atDecimalPt;

Display::Display() {
  pinMode(SERIAL_CLK, OUTPUT);
  pinMode(PARALLEL_CLK, OUTPUT);
  pinMode(SERIAL_IN, OUTPUT);
  displayedNum = 0;
  flashingDigitSelect = HUNDREDS; //Timer class also has a flashingDigitSelect.
  atDecimalPt = TWO_DECIMAL_PLACE; //defaults to showing 2 decimal places. To change, see Display::setDecimalPlace()
  this->lastUpdateTime = millis();
  setup_timer(250);
}

void Display::setDecimalPlace(FlashingState decimalPlace) {
  atDecimalPt = decimalPlace;
}

void Display::setup_timer(int frequency) { 
  //https://www.instructables.com/Arduino-Timer-Interrupts/ has more examples
  //Atmega328P datasheet: page 49, 108-112
  //frequency HAS TO be in a certain range, see OCR1A register below
  cli(); //stop interrupts

  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; //initialize counter value to 0
  // turn on CTC mode, WGM[13:10] = 0b0100
  // Set CS[12:10] = 0b001 for a prescaler of 1 (I couldn't make it work with other prescalers for some reason)
  TCCR1B |= (1 << CS10) | (1 << WGM12); 
  // set compare match register for the desired frequency
  OCR1A = (16 * 10^6) / (frequency * 1) - 1; //(OCR1A MUST be <256)
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  Serial.begin(BAUD_RATE);
  Serial.println("the timer is set");
  sei(); //enables interrupts
}

void display_digit(int digitValue, int whichDigit, bool hasDecimalPt) {
  // whichDigit: ONES, TENS, HUNDREDS, THOUSANDS
  if (digitValue < 0 || digitValue > 9 + 2) {
    print_var_full("digitValue", digitValue, "ERROR in display.c display_digit");
    return;
  }
  if (whichDigit < ONES || whichDigit > MOST_SIGNIFICANT) {
    print_var_full("whichDigit", whichDigit, "ERROR in display.c display_digit");
    return;
  }

  //Select which digit (e.g. ONES, TENS) to display
  uint8_t select = ~(1 << (whichDigit + 4));
  digitalWrite(PARALLEL_CLK, LOW);
  shiftOut(SERIAL_IN, SERIAL_CLK, LSBFIRST, select);

  //Select the digit value
  uint8_t binary[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110,
  0b10111110, 0b11100000, 0b11111110, 0b11110110, 0b0, 0b10}; 
  //thus, binary[DISPLAY_DIGIT_OFF] = 0b0, binary[NEGATIVE_SIGN] = 0b10
  uint8_t temp = binary[digitValue] + hasDecimalPt;
  shiftOut(SERIAL_IN, SERIAL_CLK, LSBFIRST, temp);
  digitalWrite(PARALLEL_CLK, HIGH);
}

// displays a number with 2 decimal places
void Display::display_number(int number) {
  if (number >= -999 && number <= 9999) { //since the 7 segment display can only show 4 digits/symbols
    if (displayedNum != number) {
      lastUpdateTime = millis();
      displayedNum = number;
      print_var_full("displayedNum", displayedNum, "update in Display::display_number");
    }
  } else {
    print_var_full("number", number, "ERROR in Display::display_number");
  }
}

// make the selected digit to blink OFF
void Display::flash_digit(FlashingState whichDigit) {
    if (flashingDigitSelect != whichDigit) {
      flashingDigitSelect = whichDigit;
      print_var_full("flashingDigitSelect", flashingDigitSelect, "update in Display::flash_digit");
    }
}

ISR(TIMER1_COMPA_vect){
  cli();
  //blinking
  static volatile unsigned long lastBlinkTime = millis();
  static volatile bool blinkOn = true;
  if (millis() - lastBlinkTime > BLINK_TIME) {
    lastBlinkTime = millis();
    blinkOn = !blinkOn;
  }

  static int volatile digitSelect = ONES; //selects which digit to diplay. The 7 segment display can ONLY diplay one digit at a time.
  //THe ISR cycles through the 4 digits really quickly (at 250Hz), so to the naked eye, it appears all four digits are display simultaneously.
  volatile bool hasDecimal = digitSelect == atDecimalPt;
  
  volatile int digitValue;
  if (flashingDigitSelect == ALL_OFF || (flashingDigitSelect == digitSelect && !blinkOn)) {
    digitValue = DISPLAY_DIGIT_OFF;
    hasDecimal = false;
  } else if (displayedNum < 0 && digitSelect == MOST_SIGNIFICANT) {
    digitValue = NEGATIVE_SIGN;
  } else {
    volatile long temp = long_pow(10, digitSelect);
    digitValue = (abs(displayedNum) % (temp * 10)) / (temp);
  }
  
  display_digit(digitValue, digitSelect, hasDecimal);
  CYCLICAL_NEXT(&digitSelect, MOST_SIGNIFICANT);
  sei();
}

/////Debugging
String Display::flashing_state_string(FlashingState flashingState) {
  return flashing_state_string_helper(flashingState) + " (" + String(flashingState) + ")";
}

String Display::flashing_state_string_helper(FlashingState flashingState) {
  switch (flashingState) {
    case ALL_ON:
    return "ALL_ON";
    case ALL_OFF:
    return "ALL_OFF";
    case ONES:
    return "ONES";
    case TENS:
    return "TENS";
    case HUNDREDS:
    return "HUNDREDS";
    case THOUSANDS:
    return "THOUSANDS";
  }
  return "unknown";
}

void Display::print_info() {
  print_class_name("display");
  PRINT_VAR("displayedNum", displayedNum);
  PRINT_VAR("flashingDigitSelect", flashing_state_string(flashingDigitSelect));
  PRINT_VAR("lastUpdateTime", lastUpdateTime);
}
