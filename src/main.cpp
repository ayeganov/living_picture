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
const int EYE_POSITION_PIN = 5;
const char* yes = "yes";
const char* no = "no";
bool start_routine = false;
bool greeting_played = false;
unsigned long next_eye_look = 0;

auto eye_controller = eyes::EyeController();


const long BAUD_RATE = 115200;
const float SOUND_THRESHOLD = 542;
const int SOUND_SENSOR_PIN = A1;
const int SOUND_SENSOR_DIG_PIN = 2;
unsigned long last_event = 0;



void sense_sound()
{
  float sensor_value = analogRead(SOUND_SENSOR_PIN);
  float abs_value = abs(sensor_value - SOUND_THRESHOLD);

  int sensor_data = digitalRead(SOUND_SENSOR_DIG_PIN);
  if(sensor_data == LOW)
  {
    auto passed_time = millis() - last_event;
    if(passed_time > 25)
    {
      Serial.print("Detected sound: ");
      Serial.println(abs_value, 2);
      last_event = millis();
    }
  }
}


void send_current_state(const char* sound = nullptr)
{
  long eye_pos = eye_controller.get_eye_position();
  Serial.print("eyes: ");
  Serial.print(eye_pos);
  Serial.print(" closed: ");
  Serial.print(eye_controller.eyes_closed() ? yes : no);
  Serial.print(" sound: ");
  if(sound)
  {
    Serial.print(sound);
  }
}


void setup()
{
  // setup detection objects
  Serial.begin(BAUD_RATE);
  pinMode(EYE_POSITION_PIN, INPUT);

  detection::init_sonic_person_detector(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM, 30);
  detection::set_detection_time(1000);
  // read unconnected pin to get analog noise value
  randomSeed(analogRead(0));
}


void loop()
{
  if(current_state == State::INITIALIZATION)
  {
    Serial.println("get_eye_position");

    if(Serial.available() > 0)
    {
      auto position = (int)Serial.parseFloat();
      eye_controller.init_eye_position(position);
      eye_controller.close_eyes();
      current_state = State::CLOSING_EYES;
    }
  }
  else if(current_state == State::CLOSING_EYES)
  {
    send_current_state();
    if(eye_controller.eyes_closed())
    {
      current_state = State::RUNNING;
    }
  }
  else
  {
    // detect person
    // on detection
    //  1. Open the eyes
    //  2. Play sound - random 'khm', or 'hello there'
    //  3. Enter sound detection mode
    //    3a. If detected sound - play congrats sound
    //  4. When person leaves - play good bye sound

    detection::update_person_state();
    if(detection::is_person_detected() and not start_routine)
    {
      start_routine = true;
      next_eye_look = millis() + random(10000, 15000);
    }
    else if(not detection::is_person_detected() and start_routine)
    {
      start_routine = false;
      // play good bye sound
      send_current_state("goodbye");
      greeting_played = false;
    }
    else if(start_routine)
    {
      if(eye_controller.eyes_open() and not greeting_played)
      {
        send_current_state("hello");
        greeting_played = true;
      }
      else if(millis() > next_eye_look)
      {
        send_current_state("random");
        next_eye_look = millis() + random(10000, 15000);
      }
      else
      {
        send_current_state();
      }
    }
  }
}
