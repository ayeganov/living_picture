#pragma once

namespace motor
{


enum class Direction
{
  CW,
  CCW
};


class StepperMotor
{
private:
  unsigned long steps_left_ = 0;
  int pin_step_ = 0;
  Direction direction_ = Direction::CW;
  long delay_ms_ = 2;
  long last_run_;
  int pin_1_, pin_2_, pin_3_, pin_4_;


public:
  StepperMotor(int pin1, int pin2, int pin3, int pin4);

  void set_move(unsigned long num_steps, Direction direction);
  bool is_running();
  unsigned long steps_to_go() { return steps_left_; }

  void run();

  void set_delay(long delay) { delay_ms_ = delay; }
  Direction direction() { return direction_; }
};


}
