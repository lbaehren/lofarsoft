//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StandardPrepare.h,v $
   $Revision: 1.1 $
   $Date: 2008/06/17 14:09:26 $
   $Author: straten $ */

#ifndef __Calibration_StandardPrepare_H
#define __Calibration_StandardPrepare_H

#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"

namespace Calibration
{

  //! Conditions input data and selects optimal input states
  class StandardPrepare : public Reference::Able
  {
  public:

    static StandardPrepare* factory (std::string name);

    //! Prepare the data for use by the StandardData class
    virtual void prepare (Pulsar::Archive*);

    //! Set the number of input states to be chosen
    virtual void set_input_states (unsigned);

    //! Choose the input states
    virtual void choose (const Pulsar::Archive*);

    //! Return the rank of the specified phase bin
    virtual double get_rank (unsigned ibin) = 0;

    //! Called when a phase bin has been chosen
    virtual void set_chosen (unsigned ibin);

    //! Return the selected bins
    virtual void get_bins (std::vector<unsigned>&) const;

  protected:

    friend class MultipleRanking;

    //! Do anything extra required to choose
    virtual void extra (const Pulsar::Archive*);

    //! The total integrated polarization profile
    Reference::To<Pulsar::PolnProfile> profile;

    //! The chosen phase bins
    std::vector<unsigned> bins;

  };

  class MaximumPolarization : public StandardPrepare
  {
  public:

    //! Return the total polarization
    double get_rank (unsigned ibin);

  };

  class MaximumIntensity : public StandardPrepare
  {
  public:

    //! Return the total intensity
    double get_rank (unsigned ibin);

  };

  class MaximumOrthogonality : public StandardPrepare
  {
  public:

    //! Return the total intensity
    double get_rank (unsigned ibin);

    //! Called when a phase bin has been chosen
    void set_chosen (unsigned ibin);

  protected:

    //! The last chosen input state
    Stokes<float> last_chosen;

  };

  class MaximumInvariant : public StandardPrepare
  {
  public:

    //! Return the total intensity
    double get_rank (unsigned ibin);

    //! Choose the input states
    void extra (const Pulsar::Archive*);

  protected:

    //! The invariant profile
    Reference::To<Pulsar::Profile> invariant;

  };

  class MultipleRanking : public StandardPrepare
  {
  public:

    //! Default constructor
    MultipleRanking ();

    //! Add a ranking scheme
    void add (StandardPrepare*);

    //! Return the total intensity
    double get_rank (unsigned ibin);

    //! Called when a phase bin has been chosen
    void set_chosen (unsigned ibin);

    //! Choose the input states
    void extra (const Pulsar::Archive*);

  protected:

    //! Available ranking schemes
    std::vector< Reference::To<StandardPrepare> > rankings;

    //! The current ranking scheme
    unsigned current;

  };

}

#endif
