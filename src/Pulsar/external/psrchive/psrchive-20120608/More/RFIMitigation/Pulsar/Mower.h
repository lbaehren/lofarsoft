//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/RFIMitigation/Pulsar/MADLM.h,v $
   $Revision: 1.5 $
   $Date: 2010/07/13 09:36:38 $
   $Author: straten $ */

#ifndef __Pulsar_MADLM_h
#define __Pulsar_MADLM_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Integration.h"

#include "TextInterface.h"
#include "Functor.h"

namespace Pulsar {

  //! Replaces impulsive DM=0 spikes with randomly selected noise
  /*! By adding preconditions, this class can be made interactive */
  class Mower : public Transformation<Integration> {

  public:

    //! Default constructor
    Mower ();

    //! Get the text interface to the mower attributes
    virtual TextInterface::Parser* get_interface ();

    // Text interface to the Mower class
    class Interface : public TextInterface::To<Mower> {
    public:
      Interface (Mower* = 0);
    };

    //! Set the Profile from which baseline PhaseWeight will be computed
    void transform (Integration*);

    //! Set the threshold above which samples are mown
    virtual void set_threshold (float madm);
    float get_threshold () const;

    //! Set the size of the window used during median smoothing
    virtual void set_median_smoothing (float turns);
    float get_median_smoothing () const;

    //! If set, search for spikes in fscrunched (DM=0) total
    virtual void set_broadband (bool);

    //! If set, mow only the points flagged in the prune mask
    virtual void set_prune (const PhaseWeight* prune_mask);
    const PhaseWeight* get_prune () const;

    //! One or more preconditions can be added
    virtual void add_precondition( Functor< bool(Profile*,PhaseWeight*) > );

  protected:
    
    //! Points to be mowed
    Reference::To<PhaseWeight> mowed;

    //! Specific points to be pruned
    Reference::To<const PhaseWeight> prune;

    //! Create mask and return true if all preconditions are satisfied
    virtual bool build_mask (Profile* profile);

    //! Find the spikes in median smoothed difference and flag them in mask
    virtual void compute (PhaseWeight* mask, const Profile* difference) = 0;

    //! Size of median smoothing window in turns
    float median_smoothing_turns;

    //! Cutoff threshold
    float cutoff_threshold;

    //! Search for spikes in fscrunch (DM=0) total
    bool broadband;

    //! Preconditions
    std::vector< Functor< bool(Profile*,PhaseWeight*) > > precondition;

  };

}

#endif
