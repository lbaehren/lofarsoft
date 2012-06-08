//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/TextParameters.h,v $
   $Revision: 1.5 $
   $Date: 2009/11/30 03:12:37 $
   $Author: straten $ */

#ifndef __TextParameters_h
#define __TextParameters_h

#include "Pulsar/Parameters.h"

namespace Pulsar {

  //! Stores pulsar parameters as uninterpreted text
  class TextParameters : public Parameters {

  public:

    //! Return a new, copy constructed instance of self
    Parameters* clone () const;

    //! Return true if *this == *that
    bool equals (const Parameters* that) const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

    //! Return the pulsar name
    std::string get_name () const;

    //
    //
    //

    //! Set the text
    void set_text (const std::string& t) { text = t; }

    //! Retrieve a string from the text
    std::string get_value (const std::string& keyword) const;

    //! Retrieve a value from the text, return true if found
    template<typename T>
    bool get_value (T& value, const std::string& keyword, 
		    bool exception=true) const;

    //! Return a value from the text, throw exeception if not found
    template<typename T>
    T get (const std::string& keyword) const
    { T value; get_value (value,keyword); return value; }

  protected:

    std::string text;
  };

}

template<typename T>
T parse (const std::string& value)
{
  return fromstring<T> (value);
}

inline std::string defortran (std::string value)
{
  std::string::size_type D = value.find ('D');
  if (D != std::string::npos)
    value[D] = 'E';
  return value;
}

// specializations for float and double address Fortran formatting issue
template<>
inline float parse<float> (const std::string& value)
{
  return fromstring<float> ( defortran(value) );
}

template<>
inline double parse<double> (const std::string& value)
{
  return fromstring<double> ( defortran(value) );
}

//! Retrieve a value from the text
template<typename T>
bool Pulsar::TextParameters::get_value (T& value, 
					const std::string& keyword, 
					bool exception) const
{
  std::string value_string = get_value (keyword);

  if (value_string.empty())
  {
    if (!exception)
      return false;

    throw Error (InvalidParam, "Pulsar::TextParameters::get_value<T>",
		 "keyword='" + keyword + "' not found");
  }

  value = parse<T> (value_string);
  return true;
}

#endif
