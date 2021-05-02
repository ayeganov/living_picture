#include "Arduino.h"
#include "motor.hpp"

#include "person_detector.hpp"
#include "eye_controller.hpp"


enum class State
{
  INITIALIZATION,
  CLOSING_EYES,
  RUNNING
};


State current_state = State::INITIALIZATION;
const int ECHO_PIN = 3;
const int TRIGGER_PIN = 4;
const int MAX_DISTANCE_CM = 300;
const char* yes = "yes";
const char* no = "no";
bool start_routine = false;
bool greeting_played = false;
unsigned long next_eye_look = 0;
int HEAD = 0;
unsigned long update_state_step_ms = 20;
unsigned long last_update = 0;

auto eye_controller = eyes::EyeController();

const long BAUD_RATE = 115200;


void send_current_state(const char* sound = nullptr)
{
  long eye_pos = 1;
  Serial.print("eyes: ");
  Serial.print(eye_pos);
  Serial.print(" closed: ");
  Serial.print(eye_controller.eyes_closed() ? yes : no);
  Serial.print(" sound: ");
  if(sound)
  {
    Serial.print(sound);
  }
  Serial.println();
}


void setup()
{
  // setup detection objects
  Serial.begin(BAUD_RATE);

  detection::init_sonic_person_detector(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM, 70);
  detection::set_detection_time(600);
  eye_controller.init_eye_position(0);
  // read unconnected pin to get analog noise value
  randomSeed(analogRead(0));
  last_update = millis();
}


void loop()
{
  auto current_millis = millis();
  if(Serial.available())
  {
    Serial.readStringUntil('\n');
    eye_controller.init_eye_position(100);
    eye_controller.close_eyes();
    Serial.println("closing the eyes");
  }

  eye_controller.update_state();
  detection::update_person_state();

  auto update_passed_time = current_millis - last_update;

  // detect person
  // on detection
  //  1. Open the eyes
  //  2. Play sound - random 'khm', or 'hello there'
  //  3. Enter sound detection mode
  //    3a. If detected sound - play congrats sound
  //  4. When person leaves - play good bye sound

  if(detection::is_person_detected() and not start_routine)
  {
    start_routine = true;
    next_eye_look = current_millis + random(10000, 15000);
    eye_controller.open_eyes();
  }
  else if(not detection::is_person_detected() and start_routine)
  {
    start_routine = false;
    // play good bye sound
    send_current_state("goodbye");
    greeting_played = false;
    eye_controller.close_eyes();
  }
  else if(start_routine)
  {
    if(eye_controller.eyes_open() and not greeting_played)
    {
      send_current_state("hello");
      greeting_played = true;
    }
    else if(current_millis > next_eye_look)
    {
      int coin = random(0, 2);
      if(coin == HEAD)
      {
        send_current_state("random");
      }
      else
      {
        eye_controller.look_up_down();
      }
      next_eye_look = current_millis + random(10000, 15000);
    }
  }
}
