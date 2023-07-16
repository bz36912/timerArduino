#ifndef TIMER_ARDUINO_H // this is a header guard
#define TIMER_ARDUINO_H

#include "display.h"
#include "joystick.h"
#include "commonlib.h"

#define CYCLE_TIME 20
#define MAX_TIME (99 * 60 + 59) //99 minutes and 59 seconds
#define SLEEP_TIME 30000 //sleeps after inactive for 30 seconds
#define BUZZER_TIME 10000 //buzzer sounds for at most 10 seconds

#define BUZZER 9 //buzzer is connected to digital pin 9 (PWM)

// finite state machine state. See diagrams under documentation folder.
typedef enum State {WAIT1, WAIT2, ALERT, PAUSE, SET1, SET2, START} State;

class Timer {
  public:
    int setValue; //in seconds
    int remainingTime; //in seconds
    int cmd; //commands from the joystick
    FlashingState flashingDigitSelect; //which digit flashes/blinks
    Display myDisplay;
    Joystick joystick;

    Timer();
    void print_info();
    void Timer::finite_state_machine();
    String Timer::state_string(State state);
    
  private:
    State state; //finite state machine state
    unsigned long enterStateTime; //when the last change of state happened
    int remainingTimeAtStartOfState; //updates only at change of state

    //states
    void set1_state();
    void Timer::set2_state();
    void Timer::start_state();
    void Timer::pause_state();
    void Timer::wait1_state();
    void Timer::wait2_state();
    void Timer::alert_state();

    int secondsToTime(int seconds);
    int timeToSeconds(int aTime);
    String Timer::state_string_helper(State state);
};

#endif
