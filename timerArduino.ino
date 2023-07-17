/*To understand the context and purpose of this code, visit:
 * https://www.instructables.com/How-to-Display-and-Set-Numbers-With-7-Segment-Disp/
 * This code makes references to steps on this Instructables website
 * written by square1a on 17th July 2023
 * 
 * The code is free and open to be used and modified.
*/

#include "timerArduino.h"

void setup() {
  // there is NO loop() function like in other Arduino code
  Serial.begin(BAUD_RATE); // activates the serial communication
  Serial.println("start of program42");
  Timer timer;
   
  while(1) {
    char c = Serial.read();
    if (c == 'i') { //prints out debugging informations
      timer.print_info();
      timer.joystick.print_info();
      timer.myDisplay.print_info();
    }
    
    timer.cmd = timer.joystick.get_command();
    if (timer.cmd != NO_ACTION) {
      PRINT_VAR("time.cmd", timer.joystick.command_string(timer.cmd));
    }
    timer.finite_state_machine();
    
    delay(CYCLE_TIME);
  }
}

Timer::Timer() {
  pinMode(BUZZER, OUTPUT);
  this->state = SET1; //finite state machine state
  this->enterStateTime = millis(); //when the last change of state happened
  this->setValue = 15; //in seconds
  this->remainingTime = 0; //in seconds
  this->remainingTimeAtStartOfState = this->remainingTime; //updates only at change of state
  this->cmd = NO_ACTION; //commands from the joystick. NO_ACTION == CENTRE.
  this->flashingDigitSelect = HUNDREDS; //which digit flashes/blinks
  this->myDisplay.setDecimalPlace(TWO_DECIMAL_PLACE);
}

void Timer::set1_state() {
  const int digitInSeconds[] = {1, 10, 60, 600}; //ONES is 1 seconds, HUNDREDS is a minute/60 seconds
  switch (this->cmd) {
    case UP + HOLDING: //if the joystick is pushed or held UP
    case UP:
    bounded_addition(&this->setValue, MAX_TIME, digitInSeconds[this->flashingDigitSelect], 0, true);
    break;
    case DOWN + HOLDING:
    case DOWN:
    bounded_addition(&this->setValue, MAX_TIME, -digitInSeconds[this->flashingDigitSelect], 0, true);
    break;
    case LEFT + HOLDING:
    case LEFT:
    CYCLICAL_NEXT(&this->flashingDigitSelect, MOST_SIGNIFICANT);
    break;
    case RIGHT + HOLDING:
    case RIGHT:
    CYCLICAL_PREV(&this->flashingDigitSelect, MOST_SIGNIFICANT);
    break;
  }

  //updates the Display class
  this->myDisplay.display_number(secondsToTime(this->setValue));
  this->myDisplay.flash_digit(this->flashingDigitSelect);

  //state transitions
  if (this->cmd == BUTTON_CLICK) {
    this->state = SET2;
  } else if (this->cmd != NO_ACTION) {
    this->enterStateTime = millis(); //so it re-enters the SET1 state, to avoid WAIT1 from triggering
  } else if (millis() - this->enterStateTime > SLEEP_TIME) {
    this->state = WAIT1;
  }
}

void Timer::set2_state() {
  this->remainingTime = this->setValue;

  //state transitions
  this->state = START;
}

void Timer::start_state() {
  unsigned long inMillisec = (unsigned long)((unsigned long)this->remainingTimeAtStartOfState * 1000 - millis() + this->enterStateTime + 0.5);
  this->remainingTime = (int)(inMillisec / 1000);
  this->myDisplay.display_number(secondsToTime(this->remainingTime));
  
  //state transitions
  if (this->cmd == BUTTON_CLICK) {
    this->state = PAUSE;
  } else if (this->remainingTime <= 0) {
    this->state = ALERT;
  }
}

void Timer::pause_state() {
  //only waiting for state transitions
  if (this->cmd == BUTTON_CLICK) {
    this->state = START;
  } else if (millis() - this->enterStateTime > SLEEP_TIME) {
    this->state = WAIT2;
  } else if (this->cmd == HOLDING + LEFT || this->cmd == HOLDING + RIGHT) {
    this->state = SET2;
  } else if (this->cmd != NO_ACTION) {
    this->state = SET1;
  }
}

void Timer::wait1_state() {
  this->myDisplay.flash_digit(ALL_OFF); //the timer sleeps
  if (this->cmd != NO_ACTION) {
    this->state = SET1;
  }
}

void Timer::wait2_state() {
  this->myDisplay.flash_digit(ALL_OFF); //the timer sleeps
  if (this->cmd != NO_ACTION) {
    this->state = PAUSE;
  }
}

void Timer::alert_state() {
  static bool select = true;
  static unsigned long prevTime = millis();
  if (millis() - prevTime > 1000) {
    prevTime = millis();
    select = !select;
  }

  tone(BUZZER, select ? 2000 : 4000);
  
  //only waiting for state transitions
  if (this->cmd != NO_ACTION || millis() - this->enterStateTime > BUZZER_TIME) {
    this->state = SET1;
  }
}

//See diagrams under documentation folder.
void Timer::finite_state_machine() {
  State prev_state = this->state;
  switch (this->state) {
    case WAIT1:
    wait1_state();
    break;
    case WAIT2:
    wait2_state();
    break;
    case ALERT:
    alert_state();
    break;
    case PAUSE:
    pause_state();
    break;
    case SET1:
    set1_state();
    break;
    case SET2:
    set2_state();
    break;
    case START:
    start_state();
    break;
    default:
    print_var_full("state", this->state, "ERROR in Timer::finite_state_machine");
    break;
  }

  if (prev_state != this->state) {
    prev_state = this->state;
    print_var_full("state", state_string(this->state), "state changed in Timer::finite_state_machine");
    this->myDisplay.flash_digit(ALL_ON);
    this->enterStateTime = millis();
    noTone(BUZZER);
    this->remainingTimeAtStartOfState = this->remainingTime;
  }
}

int Timer::secondsToTime(int seconds) {
  int timeSeconds = seconds % 60;
  int timeMinutes = seconds / 60;
  return 100 * timeMinutes + timeSeconds; //in the form of mmss (m for minute, s for seconds)
}

int Timer::timeToSeconds(int aTime) {
  int seconds = aTime % 100;
  int minutes = aTime / 100;
  return seconds + 60 * minutes;
}

/////debugging
String Timer::state_string(State state) {
  return state_string_helper(state) + " (" + String(state) + ")";
}
String Timer::state_string_helper(State state) {
  switch (state) {
    case WAIT1:
    return "WAIT1";
    case WAIT2:
    return "WAIT2";
    case ALERT:
    return "ALERT";
    case PAUSE:
    return "PAUSE";
    case SET1:
    return "SET1";
    case SET2:
    return "SET2";
    case START:
    return "START";
  }
  return "unknown";
}

void Timer::print_info() {
  print_class_name("timer");
  PRINT_VAR("state", state_string(this->state));
  PRINT_VAR("enterStateTime", this->enterStateTime);
  PRINT_VAR("setValue", this->setValue);
  PRINT_VAR("remainingTime", this->remainingTime);
  PRINT_VAR("remainingTimeAtStartOfState", this->remainingTimeAtStartOfState);
  PRINT_VAR("cmd", this->joystick.command_string(this->cmd));
  PRINT_VAR("flashingDigitSelect", this->flashingDigitSelect);
}

void test_display(Timer* timer) {
  //passed the test already
  Serial.println("Start of display testing");
  #define END 4200
  float num[] = {12, -3.6, -39.3, 100, 99.99, -9.991, -9.99, -0.54, 0, -0.01, 0.001, -4.56543, 5000, END};

  for (int i = 0; i < 30; i++) {
    if (num[i] == END) {
      break;
    }
    Serial.println(num[i]);
    timer->myDisplay.display_number(num[i]);
    delay(2000);
  }
}

void analogTest () {
  const int ADC_PIN = A2;
  const int PWM_PIN = 10;
  pinMode(ADC_PIN, INPUT);
  pinMode(PWM_PIN, OUTPUT);

  bool toggle = true;
  while(1) {
    delay(1000);
    if (toggle) {
      analogWrite(PWM_PIN, 255);
    } else {
      analogWrite(PWM_PIN, 125);
    }
    toggle = !toggle;

    int val = analogRead(ADC_PIN);
    PRINT_VAR("val", val);
  }
}
