#ifndef JOYSTICK_H // this is a header guard
#define JOYSTICK_H

#include <Arduino.h>
#include "commonlib.h"

#define X_VAL A3
#define Y_VAL A4
#define BUTTON A5
#define HOLDING_TIME 300 //in ms, above this time threshold, it is considered a LONG hold
#define DEBOUNCING_DELAY 30 //in ms

typedef enum Position {CENTRE, UP, DOWN, LEFT, RIGHT} Position;
#define HOLDING 10
#define NO_ACTION CENTRE
#define BUTTON_CLICK -1

class Joystick {
  public:
    Joystick();
    String command_string(int command);
    void print_info();
    int get_command();
  private:
    Position get_position();
    String Joystick::command_string_helper(int command);
    Position now;
    Position prev;
    unsigned long changePosTime;
    unsigned long lastUpdateTime;
    boolean wasHolding;
    boolean prevButton;
};

#endif
