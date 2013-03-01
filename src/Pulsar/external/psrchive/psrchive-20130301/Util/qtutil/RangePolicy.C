/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "RangePolicy.h"
#include "tostring.h"

RangePolicy::RangePolicy (int min, int max)
{
  input_min = min;
  input_max = max;
}

//! Destructor
RangePolicy::~RangePolicy ()
{
}

//! Set the minimum value of the integer input
void RangePolicy::set_input_min (int min)
{
  input_min = min;
}

//! Set the maximum value of the integer input
void RangePolicy::set_input_max (int max)
{
  input_max = max;
}

//! Get the recommended minimum value of the integer input
int RangePolicy::get_input_min () const
{
  return input_min;
}

//! Get the recommended maximum value of the integer input
int RangePolicy::get_input_max () const
{
  return input_max;
}

//! Construct with the current minimum and maximum input values
IntegerRangePolicy::IntegerRangePolicy (int min, int max)
  : RangePolicy (min, max)
{
}

//! Set the minimum value of the noutput
void IntegerRangePolicy::set_output_min (const std::string& min)
{
  input_min = fromstring<int> (min);
}

//! Set the maximum value of the output
void IntegerRangePolicy::set_output_max (const std::string& max)
{
  input_max = fromstring<int> (max);
}

//! Get the understood minimum value of the output
std::string IntegerRangePolicy::get_output_min () const
{
  return tostring (input_min);
}

//! Get the understood maximum value of the output
std::string IntegerRangePolicy::get_output_max () const
{
  return tostring (input_max);
}

//! Get the output value corresponding to the input range index
std::string IntegerRangePolicy::get_output (int input) const
{
  return tostring (input);
}

//! Construct with the current minimum and maximum input values
RealRangePolicy::RealRangePolicy ()
  : RangePolicy (0, 1023)
{
  output_min = input_min;
  output_max = input_max;
}

//! Set the minimum value of the noutput
void RealRangePolicy::set_output_min (const std::string& min)
{
  output_min = fromstring<double> (min);
}

//! Set the maximum value of the output
void RealRangePolicy::set_output_max (const std::string& max)
{
  output_max = fromstring<double> (max);
}

//! Get the understood minimum value of the output
std::string RealRangePolicy::get_output_min () const
{
  return tostring (output_min);
}

//! Get the understood maximum value of the output
std::string RealRangePolicy::get_output_max () const
{
  return tostring (output_max);
}

//! Get the output value corresponding to the input range index
std::string RealRangePolicy::get_output (int input) const
{
  double output_range = output_max - output_min;
  int input_range = input_max - input_min;

  double output = output_min + input * output_range / double(input_range);

  return tostring (output);
}
