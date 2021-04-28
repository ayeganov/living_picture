#include "Arduino.h"
#include "motor.hpp"

namespace motor
{

StepperMotor::StepperMotor(int pin1, int pin2, int pin3, int pin4)
  : last_run_(millis()),
    pin_1_(pin1),
    pin_2_(pin2),
    pin_3_(pin3),
    pin_4_(pin4)
{
  pinMode(pin_1_, OUTPUT);
  pinMode(pin_2_, OUTPUT);
  pinMode(pin_3_, OUTPUT);
  pinMode(pin_4_, OUTPUT);
}


bool StepperMotor::is_running()
{
  return steps_left_ > 0;
}


void StepperMotor::set_move(unsigned long num_steps, Direction direction)
{
  direction_ = direction;
  steps_left_ = num_steps;
}


void StepperMotor::set_move(long num_steps)
{
  direction_ = num_steps < 0 ? Direction::CW : Direction::CCW;
  steps_left_ = abs(num_steps);
}


void StepperMotor::run()
{
  if(steps_left_ == 0) return;

  auto passed_time = millis() - last_run_;
  if(passed_time < delay_ms_)
  {
    return;
  }
  last_run_ = millis();

  if(direction_ == Direction::CW)
  {
    --current_position_;
    switch(pin_step_)
    {
      case 0:
        digitalWrite(pin_1_, HIGH);
        digitalWrite(pin_2_, LOW);
        digitalWrite(pin_3_, LOW);
        digitalWrite(pin_4_, LOW);
        break;
      case 1:
        digitalWrite(pin_1_, LOW);
        digitalWrite(pin_2_, HIGH);
        digitalWrite(pin_3_, LOW);
        digitalWrite(pin_4_, LOW);
        break;
      case 2:
        digitalWrite(pin_1_, LOW);
        digitalWrite(pin_2_, LOW);
        digitalWrite(pin_3_, HIGH);
        digitalWrite(pin_4_, LOW);
        break;
      case 3:
        digitalWrite(pin_1_, LOW);
        digitalWrite(pin_2_, LOW);
        digitalWrite(pin_3_, LOW);
        digitalWrite(pin_4_, HIGH);
        break;
    }
  }
  else
  {
    ++current_position_;
    switch(pin_step_)
    {
      case 0:
        digitalWrite(pin_1_, LOW);
        digitalWrite(pin_2_, LOW);
        digitalWrite(pin_3_, LOW);
        digitalWrite(pin_4_, HIGH);
        break;
      case 1:
        digitalWrite(pin_1_, LOW);
        digitalWrite(pin_2_, LOW);
        digitalWrite(pin_3_, HIGH);
        digitalWrite(pin_4_, LOW);
        break;
      case 2:
        digitalWrite(pin_1_, LOW);
        digitalWrite(pin_2_, HIGH);
        digitalWrite(pin_3_, LOW);
        digitalWrite(pin_4_, LOW);
        break;
      case 3:
        digitalWrite(pin_1_, HIGH);
        digitalWrite(pin_2_, LOW);
        digitalWrite(pin_3_, LOW);
        digitalWrite(pin_4_, LOW);
    }
  }

  if(abs(current_position_) >= STEPS_PER_REVOLUTION)
  {
    current_position_ = 0;
  }

  ++pin_step_;
  if(pin_step_ > 3)
  {
    pin_step_ = 0;
  }

  if(steps_left_ > 0) --steps_left_;
}


}
