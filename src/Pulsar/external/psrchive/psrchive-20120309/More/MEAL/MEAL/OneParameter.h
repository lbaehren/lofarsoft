//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/OneParameter.h,v $
   $Revision: 1.9 $
   $Date: 2008/05/07 01:21:11 $
   $Author: straten $ */

#ifndef __OneParameter_H
#define __OneParameter_H

#include "MEAL/ParameterPolicy.h"
#include "Estimate.h"
#include "Error.h"

namespace MEAL {
  
  //! Abstract base class implements parameter storage and access
  class OneParameter : public ParameterPolicy {

  public:

    //! Default constructor
    OneParameter (Function* context = 0);

    //! Copy constructor
    OneParameter (const OneParameter& np);

    //! Assignment operator
    OneParameter& operator = (const OneParameter& np);

    //! Clone operator
    OneParameter* clone (Function* context) const;

    //! Return the name of the parameter
    std::string get_name () const
    {
      return name;
    }

    //! Set the name of the parameter
    void set_name (const std::string& _name)
    {
      name = _name;
    }

    //! Return the description of the parameter
    std::string get_description () const
    {
      return description;
    }

    //! Set the description of the parameter
    void set_description (const std::string& _description)
    {
      description = _description;
    }

    //! Return the value of the parameter
    virtual double get_param () const
    {
      return param.val;
    }

    //! Set the value of the parameter
    virtual void set_param (double value);

    //! Return the variance of the parameter
    double get_variance () const
    {
      return param.var;
    }

    //! Set the variance of the parameter
    void set_variance (double value)
    {
      param.var = value;
    }

    //! Return true if parameter is to be fitted
    bool get_infit () const
    {
      return fit;
    }

    //! Set flag for parameter to be fitted
    void set_infit (bool flag = true)
    {
      fit = flag;
    }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const
    {
      return 1;
    }

    //! Return the name of the specified parameter
    std::string get_name (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_name");
      return name;
    }

    //! Set the name of the specified parameter
    void set_name (unsigned index, const std::string& _name)
    {
      range_check (index, "MEAL::OneParameter::set_name");
      name = _name;
    }

    //! Return the description of the specified parameter
    std::string get_description (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_description");
      return description;
    }

    //! Set the description of the specified parameter
    void set_description (unsigned index, const std::string& _description)
    {
      range_check (index, "MEAL::OneParameter::set_description");
      description = _description;
    }

    //! Return the value of the specified parameter
    double get_param (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_param");
      return get_param ();
    }

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value) 
    {
      range_check (index, "MEAL::OneParameter::set_param");
      set_param (value);
    }

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_variance");
      return param.var;
    }

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double value)
    {
      range_check (index, "MEAL::OneParameter::set_variance");
      param.var = value;
    }

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_infit");
      return fit;
    }

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag)
    {
      range_check (index, "MEAL::OneParameter::set_infit");
      fit = flag;
    }

  protected:

    //! Ensure that index <= get_nparam
    void range_check (unsigned index, const char* method) const
    {
      if (index != 0)
	throw Error (InvalidRange, method, "index=%d != 0", index);
    }

  private:

    //! The Estimate of the parameter
    Estimate<double> param;

    //! Fit flag for the parameter
    bool fit;

    //! The name of the parameter
    std::string name;

    //! The description of the parameter
    std::string description;

  };

}

#endif
