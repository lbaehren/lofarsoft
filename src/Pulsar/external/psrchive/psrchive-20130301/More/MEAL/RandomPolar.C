#include "MEAL/RandomPolar.h"
#include "MEAL/Polar.h"

#include <stdlib.h>

template <class T, class U>
T random (U min, U max)
{
  return T(min) + T(rand()) * T(max - min) / RAND_MAX;
}

template <class T, class U>
T random (U abs_max)
{
  return T(abs_max) * ( T(rand()) / RAND_MAX - 0.5 );
}

//! Default constructor
MEAL::RandomPolar::RandomPolar()
{
  min_gain = 0.25;
  max_gain = 4.0;

  max_abs_rotation = 1.0;
  max_abs_boost = 0.5;
}

void MEAL::RandomPolar::get (Polar* polar)
{
  polar->set_gain( random<double>(min_gain, max_gain) );

  for (unsigned i=0; i < 3; i++) {
    polar->set_boost( i, random<double>(max_abs_boost) );
    polar->set_rotation( i, random<double>(max_abs_rotation) );
  }
}
