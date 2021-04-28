#include "Arduino.h"
#include "eye_controller.hpp"


const int CLOSED_EYES_POSITION = 0;
const int OPEN_EYES_POSITION = 510;
const int LOOK_MOVE_STEPS = 250;
const int UP_EYES_POSITION = OPEN_EYES_POSITION + LOOK_MOVE_STEPS;
const int DOWN_EYES_POSITION = OPEN_EYES_POSITION - LOOK_MOVE_STEPS;


/*
 * Clockwise - eyes look down
 * CCW - eyes look up
 */

namespace eyes
{

EyeController::EyeController()
  : motor_(9, 10, 11, 12)
{}


void EyeController::init_eye_position(long current_position)
{
  motor_.set_current_position(current_position);
  current_state_ = EyeState::READY;
}


bool EyeController::eyes_closed()
{
  return motor_.get_current_position() == CLOSED_EYES_POSITION;
}


bool EyeController::eyes_open()
{
  return motor_.get_current_position() == OPEN_EYES_POSITION;
}


bool EyeController::eyes_down()
{
  return motor_.get_current_position() <= DOWN_EYES_POSITION;
}


bool EyeController::eyes_up()
{
  return motor_.get_current_position() >= UP_EYES_POSITION;
}


bool EyeController::eyes_moving()
{
  return motor_.steps_to_go() > 0;
}


void EyeController::open_eyes()
{
  long steps_left = OPEN_EYES_POSITION - motor_.get_current_position();
  motor_.set_move(steps_left, motor::Direction::CCW);
  current_state_ = EyeState::OPEN;
}


void EyeController::move_eyes_down()
{
  long steps_to_go = LOOK_MOVE_STEPS * 2;
  motor_.set_move(steps_to_go, motor::Direction::CW);
  current_state_ = EyeState::DOWN;
}


void EyeController::close_eyes()
{
  auto steps_to_closed = motor_.get_current_position();
  motor_.set_move(steps_to_closed, motor::Direction::CW);
  current_state_ = EyeState::CLOSED;
}


void EyeController::look_up_down()
{
  motor_.set_move(LOOK_MOVE_STEPS, motor::Direction::CCW);
  current_state_ = EyeState::UP;
}


void EyeController::update_state()
{
  switch(current_state_)
  {
    case EyeState::CLOSED:
    {
      if(eyes_closed())
      {
        current_state_ = EyeState::READY;
      }
      break;
    }
    case EyeState::OPEN:
    {
      if(eyes_open())
      {
        current_state_ = EyeState::READY;
      }
      break;
    }
    case EyeState::UP:
    {
      if(eyes_up())
      {
        move_eyes_down();
      }
      break;
    }
    case EyeState::DOWN:
    {
      if(eyes_down())
      {
        open_eyes();
      }
    }
    case EyeState::READY:
    case EyeState::UNKNOWN:
    {
      break;
    }
  }
  motor_.run();
}


}
