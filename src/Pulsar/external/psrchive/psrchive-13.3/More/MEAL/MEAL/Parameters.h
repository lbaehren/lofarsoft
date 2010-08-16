//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Parameters.h,v $
   $Revision: 1.11 $
   $Date: 2009/08/19 21:06:49 $
   $Author: straten $ */

#ifndef __Parameters_H
#define __Parameters_H

#include "MEAL/ParameterPolicy.h"
#include "Estimate.h"
#include "Error.h"

#include <vector>

namespace MEAL {
  
  //! Abstract base class implements parameter storage and access
  class Parameters : public ParameterPolicy {

  public:

    //! Default constructor
    Parameters (Function* context, unsigned nparam = 0);

    //! Copy constructor
    Parameters (const Parameters& np);

    //! Assignment operator
    Parameters& operator = (const Parameters& np);

    //! Clone construtor
    Parameters* clone (Function*) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const
    {
      return params.size();
    }

    //! Return the name of the specified parameter
    std::string get_name (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_name");
      return names[index];
    }

    //! Return the name of the specified parameter
    void set_name (unsigned index, const std::string& name)
    {
      range_check (index, "MEAL::Parameters::set_name");
      names[index] = name;
    }

    //! Return the description of the specified parameter
    std::string get_description (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_description");
      return descriptions[index];
    }

    //! Return the description of the specified parameter
    void set_description (unsigned index, const std::string& description)
    {
      range_check (index, "MEAL::Parameters::set_description");
      descriptions[index] = description;
    }

    //! Return the value of the specified parameter
    double get_param (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_param");
      return params[index].val;
    }

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value);

    //! Swap the value of the specified parameter
    void swap_param (unsigned index, double& value);

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_variance");
      return params[index].var;
    }

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double value)
    {
      range_check (index, "MEAL::Parameters::set_variance");
      params[index].var = value;
    }

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_infit");
      return fit[index];
    }

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag)
    {
      range_check (index, "MEAL::Parameters::set_infit");
      fit[index] = flag;
    }

    //! Resize arrays, setting fit=true for new parameters
    void resize (unsigned nparam);

    //! Erase the specified parameter
    void erase (unsigned iparam);

  protected:

    //! Ensure that index <= get_nparam
    void range_check (unsigned index, const char* method) const
    {
      if (index >= params.size())
	throw Error (InvalidRange, method, "index=%d >= nparam=%d", 
		     index, params.size());
    }

  private:

    //! The Estimates of the parameters
    std::vector< Estimate<double> > params;

    //! Fit flag for each parameter
    std::vector<bool> fit;

    //! The name of each parameter
    std::vector<std::string> names;

    //! The description of each parameter
    std::vector<std::string> descriptions;

  };

}

#endif
