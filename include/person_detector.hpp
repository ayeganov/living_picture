#pragma once


namespace detection
{

namespace detail
{
  void echo_check();
}

void init_sonic_person_detector(int trig_pin, int echo_pin, int max_distance, int detection_distance);
void set_detection_distance(float detection_distance);
void update_person_state();
void set_detection_time(unsigned long time_ms);
bool is_person_detected();



}
