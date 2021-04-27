#include "person_detector.hpp"
#include "filters.hpp"

#include <NewPing.h>

namespace detection
{

static bool threshold_crossed_ = false;
static unsigned long ping_timer_ = millis();
static unsigned int ping_delay_ms_ = 20;
static NewPing* sonar_;
static filter::Complementary sonic_filter_(0.5);
static float detection_distance_;
static unsigned long detection_time_ms_ = 3000;
static unsigned long person_arrived_time_ms_ = 0;
static unsigned long person_left_time_ms_ = 0;
static bool person_detected = false;


void init_sonic_person_detector(int trig_pin, int echo_pin, int max_distance, int detection_distance)
{
  detection_distance_ = detection_distance;
  sonar_ = new NewPing(trig_pin, echo_pin, max_distance);
}


void set_detection_time(unsigned long time_ms)
{
  detection_time_ms_ = time_ms;
}


void set_detection_distance(float detection_distance)
{
  detection_distance_ = detection_distance;
}


bool is_person_detected()
{
  return person_detected;
}


void update_person_state()
{
  bool issue_new_ping = millis() >= ping_timer_;
  if(issue_new_ping)
  {
    ping_timer_ += ping_delay_ms_;
    sonar_->ping_timer(detail::echo_check);
  }
}

namespace detail
{

void echo_check()
{
  if(sonar_->check_timer())
  {
    float current_distance = sonic_filter_.filter(sonar_->ping_result / US_ROUNDTRIP_CM);

    threshold_crossed_ = current_distance < detection_distance_;

    if(threshold_crossed_)
    {
      person_left_time_ms_ = 0;
      if(person_arrived_time_ms_ == 0)
      {
//        Serial.println("Starting countdown to person detection");
        person_arrived_time_ms_ = millis();
      }
      else if(person_arrived_time_ms_ > 0)
      {
        auto passed_time = millis() - person_arrived_time_ms_;
        person_detected = passed_time > detection_time_ms_;
      }
    }
    else if(person_detected)
    {
      if(person_left_time_ms_ == 0)
      {
//        Serial.println("Starting countdown to person left");
        person_left_time_ms_ = millis();
      }
      else
      {
        auto passed_time = millis() - person_left_time_ms_;
        bool person_left = passed_time > detection_time_ms_;
        if(person_left)
        {
          person_arrived_time_ms_ = 0;
          person_detected = false;
        }
      }
    }
    else
    {
      person_arrived_time_ms_ = 0;
    }
  }
}

}


}
