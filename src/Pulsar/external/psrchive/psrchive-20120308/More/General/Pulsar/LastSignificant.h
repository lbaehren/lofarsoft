//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/LastSignificant.h,v $
   $Revision: 1.2 $
   $Date: 2008/03/03 07:21:41 $
   $Author: straten $ */

#ifndef __Pulsar_LastSignificant_h
#define __Pulsar_LastSignificant_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  class Profile;

  //! Find the last significant harmonic in a PSD
  /*! This algorithm is much like PeakConsecutive, but can be called
    multiple times and assumes that all power starts at bin zero. */

  class LastSignificant : public Algorithm
  {

  public:

    //! Default constructor
    LastSignificant ();

    //! Set the cut-off threshold as a multiple of the rms
    void set_threshold (float sigma);

    //! Get the cut-off threshold
    float get_threshold () const;

    //! Set the number of consecutive bins that must remain above threshold
    void set_consecutive (unsigned consecutive);

    //! Get the number of consecutive bins
    unsigned get_consecutive () const;

    //! Find the last significant bin
    void find (const Profile*, double rms);

    //! Find the last significant bin
    void find (const std::vector<float>&, double rms);

    //! Find the last significant bin
    void find (unsigned nbin, const float*, double rms);

    //! Get the last signficant bin
    unsigned get () const;

    //! Reset the last significant phase bin
    void reset ();

  protected:

    //! The cut-off threshold
    float cutoff_sigma;

    //! The number of consecutive bins that must remain above threshold
    unsigned consecutive;

    //! The last significant bin
    unsigned last_significant;

  }; 

}

#endif
