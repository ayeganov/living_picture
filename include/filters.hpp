#pragma once

namespace filter
{

class Complementary
{
private:
  float alpha_;
  float value_;

public:
  Complementary(float alpha);
  float filter(float value);
};


class KalmanSingle
{
private:
  float estimate_;
  float error_estimate_;
  float measurement_;
  float error_measurement_;

public:
  KalmanSingle();

  float filter(float value);
};

}
