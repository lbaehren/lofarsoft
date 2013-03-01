//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __QT_RangePolicy_H
#define __QT_RangePolicy_H

#include <string>

class RangePolicy
{
 public:

  //! Construct with the current minimum and maximum input values
  RangePolicy (int min, int max);

  //! Destructor
  virtual ~RangePolicy ();

  //! Set the minimum value of the integer input
  virtual void set_input_min (int min);

  //! Set the maximum value of the integer input
  virtual void set_input_max (int max);

  //! Get the recommended minimum value of the integer input
  virtual int get_input_min () const;

  //! Get the recommended maximum value of the integer input
  virtual int get_input_max () const;

  //! Set the minimum value of the noutput
  virtual void set_output_min (const std::string& min) = 0;

  //! Set the maximum value of the output
  virtual void set_output_max (const std::string& max) = 0;

  //! Get the understood minimum value of the output
  virtual std::string get_output_min () const = 0;

  //! Get the understood maximum value of the output
  virtual std::string get_output_max () const = 0;

  //! Get the output value corresponding to the input range index
  virtual std::string get_output (int input) const = 0;

 public:

  int input_min;
  int input_max;

};

class IntegerRangePolicy : public RangePolicy
{
 public:

  //! Construct with the current minimum and maximum input values
  IntegerRangePolicy (int min, int max);

  //! Set the minimum value of the noutput
  void set_output_min (const std::string& min);

  //! Set the maximum value of the output
  void set_output_max (const std::string& max);

  //! Get the understood minimum value of the output
  std::string get_output_min () const;

  //! Get the understood maximum value of the output
  std::string get_output_max () const;

  //! Get the output value corresponding to the input range index
  std::string get_output (int input) const;

};

class RealRangePolicy : public RangePolicy
{
 public:

  //! Construct with the current minimum and maximum input values
  RealRangePolicy ();

  //! Set the minimum value of the noutput
  void set_output_min (const std::string& min);

  //! Set the maximum value of the output
  void set_output_max (const std::string& max);

  //! Get the understood minimum value of the output
  std::string get_output_min () const;

  //! Get the understood maximum value of the output
  std::string get_output_max () const;

  //! Get the output value corresponding to the input range index
  std::string get_output (int input) const;

protected:

  double output_min;
  double output_max;

};

#endif
