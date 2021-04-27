#pragma once

#include "motor.hpp"

namespace eyes
{

/*
 * Clockwise - eyes look down, negative number
 * CCW - eyes look up, positive number
 */


enum class EyeState
{
  OPEN,
  CLOSED,
  UP,
  DOWN,
  READY,
  UNKNOWN
};


class EyeController
{
public:
  EyeController();

  void init_eye_position(long current_position);

  void close_eyes();
  void open_eyes();
  bool eyes_closed();
  bool eyes_open();
  bool eyes_up();
  bool eyes_down();
  void look_up_down();
  bool eyes_moving();
  void update_state();

  long get_eye_position() { return motor_.get_current_position(); }

private:
  void move_eyes_down();

private:
  EyeState current_state_{EyeState::UNKNOWN};
  motor::StepperMotor motor_;
};


};
