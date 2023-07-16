#include "joystick.h"

Joystick::Joystick() {
  pinMode(BUTTON, INPUT);
  pinMode(X_VAL, INPUT);
  pinMode(Y_VAL, INPUT);

  this->now = CENTRE; //current position of the joystick
  this->prev = CENTRE; //previous position of the joystick
  this->changePosTime = millis();
  this->lastUpdateTime = millis();
  this->wasHolding = false;
  this->prevButton = true;
}

Position Joystick::get_position() { 
  //returns the current position of the joystick
  //expressing as CENTRE, LEFT, RIGHT, UP or DOWN
  int x = analogRead(X_VAL);
  int y = analogRead(Y_VAL);

  if (abs(x - 500) < 100 && abs(y - 500) < 150) {
    return CENTRE;
  }

  if (y > x) {
    return (y + x < 1000) ? LEFT : DOWN;
  } else {
    return (y + x < 1000) ? UP : RIGHT;
  }
}

int Joystick::get_command() {
  if (millis() - this->lastUpdateTime > DEBOUNCING_DELAY) {
    this->lastUpdateTime = millis();
    ///// button press detection ////
    if (this->prevButton && !digitalRead(BUTTON)) { //falling edge trigger, since button is active LOW
      this->prevButton = false;
      return BUTTON_CLICK;
    }
    this->prevButton = digitalRead(BUTTON);

    ////// update joystick position info /////
    this->now = get_position();
    if (this->now != this->prev) {
      this->changePosTime = millis();
    }
    
    if (millis() - this->changePosTime > HOLDING_TIME) { // holding down the joystick for longer than HOLDING_TIME
      this->changePosTime = millis();
      this->wasHolding = true;
      this->prev = this->now;
      if (this->now != CENTRE) {
        return this->now + HOLDING;
      } else {
        return NO_ACTION;
      }
    }

    if (this->now == CENTRE) { //holding down the joystick for shorter than HOLDING_TIME
      if (this->prev != CENTRE && !this->wasHolding) {
        //triggers when the joystick returns to the CENTRE, after being positioned elsewhere
        //it is detecting the falling edge of the joystick movement
        int temp = this->prev;
        this->prev = this->now;
        return temp;
      }
      this->wasHolding = false;
    }
  }
  
  this->prev = this->now;
  return NO_ACTION;
}

/////debugging
String Joystick::command_string(int command) {
  return command_string_helper(command) + " (" + String(command) + ")";
}

String Joystick::command_string_helper(int command) {
  switch (command) {
    case UP:
    return "UP";
    case DOWN:
    return "DOWN";
    case LEFT:
    return "LEFT";
    case RIGHT:
    return "RIGHT";
    case CENTRE:
    return "CENTRE";
    case UP + HOLDING:
    return "HOLDING UP";
    case DOWN + HOLDING:
    return "HOLDING DOWN";
    case LEFT + HOLDING:
    return "HOLDING LEFT";
    case RIGHT + HOLDING:
    return "HOLDING RIGHT";
    case BUTTON_CLICK:
    return "button clicked";
  }
  return "unknown";
}

void Joystick::print_info() {
  print_class_name("JOYSTICK");
  PRINT_VAR("x-value", analogRead(X_VAL));
  PRINT_VAR("y-value", analogRead(Y_VAL));
  
  PRINT_VAR("now", this->command_string(this->now));
  PRINT_VAR("prev", this->command_string(this->prev));
  
  PRINT_VAR("wasHolding", this->wasHolding);
  PRINT_VAR("lastUpdateTime", this->lastUpdateTime);
  PRINT_VAR("changePosTime", this->changePosTime);
  PRINT_VAR("millis()", millis());
  PRINT_VAR("button", digitalRead(BUTTON));
}
