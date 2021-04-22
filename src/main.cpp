#include "Arduino.h"
#include "motor.hpp"

#include "person_detector.hpp"


const int ECHO_PIN = 7;
const int TRIGGER_PIN = 6;
const int MAX_DISTANCE_CM = 300;

auto stepper = motor::StepperMotor(10, 11, 12, 13);

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
unsigned long ping_timer = 0;
unsigned long ping_speed = 30;
const long BAUD_RATE = 115200;
const float SOUND_THRESHOLD = 542;
const int SOUND_SENSOR_PIN = A1;
const int SOUND_SENSOR_DIG_PIN = 2;
float current_distance = 0;
bool do_sense_sound = false;
unsigned long last_event = 0;

bool start_routine = false;


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


void setup()
{
  // setup detection objects
  Serial.begin(BAUD_RATE);
  ping_timer = last_event = millis();
//  stepper.set_move(2000, motor::Direction::CW);
  detection::init_sonic_person_detector(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM, 30);
  detection::set_detection_time(1000);
}



void loop()
{
  detection::update_person_state();
  // detect person
  // on detection
  //  1. Open the eyes
  //  2. Play sound - random 'khm', or 'hello there'
  //  3. Enter sound detection mode
  //    3a. If detected sound - play congrats sound
  //  4. When person leaves - play good bye sound
  if(detection::is_person_detected() and not start_routine)
  {
    Serial.println("Detected person");
    start_routine = true;
  }

  if(!detection::is_person_detected() and start_routine)
  {
    Serial.println("Person left");
    start_routine = false;
    // play good bye sound
  }

  if(start_routine)
  {
    // eye_controller.open_eyes();
    // if(eye_controller.eyes_open()
    // {
    //    play sound
    // }

    // if(loud sound detected)
    // {
    //    play congrats sound
    // }
  }

//  if(do_sense_sound)
//  {
//    sense_sound();
//  }
//
//  if(stepper.steps_to_go() == 0)
//  {
//    Serial.print("Reversing the direction: ");
//    if(stepper.direction() == motor::Direction::CW)
//    {
//      Serial.println("counter clockwise");
//      stepper.set_move(2000, motor::Direction::CCW);
//    }
//    else
//    {
//      Serial.println("clockwise");
//      stepper.set_move(2000, motor::Direction::CW);
//    }
//  }
//
//  stepper.run();
}
