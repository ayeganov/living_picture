#include "filters.hpp"


namespace filter
{

Complementary::Complementary(float alpha)
  : alpha_(alpha),
    value_(0)
  {}


float Complementary::filter(float value)
{
  value_ = (value * alpha_) + (1.0 - alpha_) * value_;
  return value_;
}

}
