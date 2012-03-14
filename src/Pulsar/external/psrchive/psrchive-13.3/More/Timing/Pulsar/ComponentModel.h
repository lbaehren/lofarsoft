//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/ComponentModel.h,v $
   $Revision: 1.4 $
   $Date: 2009/11/10 04:39:04 $
   $Author: straten $ */

#ifndef __Pulsar_ComponentModel_h
#define __Pulsar_ComponentModel_h

#include "Pulsar/ProfileShiftEstimator.h"
#include "MEAL/Univariate.h"

#include <map>

namespace MEAL 
{
  class Scalar;
  class ScalarParameter;
  class ScaledVonMises;
  class ScaledVonMisesDeriv;
}

namespace Pulsar
{
  class Profile;
  class Archive;

  //! Models a pulse profile using multiple components
  /*! For now, only the von Mises distribution is used */
  class ComponentModel : public ProfileShiftEstimator
  {
  public:

    //! Default constructor
    ComponentModel ();

    //! Load model from file
    ComponentModel (const std::string& filename);

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface () { return 0; }

    //! Return a copy constructed instance of self
    ComponentModel* clone () const { return new ComponentModel(*this); }

    // I/O
    void load (const char *fname);
    void unload (const char *fname) const;

    // Manipulation
    void add_component (double centre, double concentration, double height,
			const char *name);

    void remove_component (unsigned icomp);

    MEAL::ScaledVonMises *get_component(unsigned icomp);

    unsigned get_ncomponents() const;

    void align (const Profile *profile);

    // Fitting

    void set_fit_derivative (bool flag) { fit_derivative = flag; }
    void set_threshold (float t) { threshold = t; }

    void set_infit (unsigned icomponent, unsigned iparam, bool infit);
    void set_infit (const char *fitstring);

    void fit (const Profile *profile);

    float get_chisq () const { return chisq; }
    unsigned get_nfree () const { return nfree; }
    
    // Evaluating
    void evaluate (float *vals, unsigned nvals, int icomp=-1) ;

  protected:

    std::vector< Reference::To<MEAL::ScaledVonMises> > components;

    mutable std::vector< Reference::To<MEAL::ScaledVonMises> > backup;

    std::vector<std::string> component_names;

    mutable std::vector< Reference::To<MEAL::ScaledVonMisesDeriv> > derivative;

    //! comments, indexed by line number
    std::map<unsigned, std::string> comments;
    
    mutable Reference::To< MEAL::ScalarParameter > phase;
    mutable Reference::To< MEAL::Univariate<MEAL::Scalar> > model;

    bool fit_derivative;
    float threshold;

    void build () const;
    void check (const char* method, unsigned icomponent) const;

    void clear ();
    void init ();

  private:

    float chisq;
    unsigned nfree;
  };
}

#endif
