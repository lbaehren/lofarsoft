//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ThresholdMatch.h,v $
   $Revision: 1.2 $
   $Date: 2008/11/07 01:20:12 $
   $Author: straten $ */

#ifndef __Pulsar_ThresholdMatch_h
#define __Pulsar_ThresholdMatch_h

#include "Pulsar/ArchiveMatch.h"

namespace Pulsar {

  //! Criterion used to determine if two archives match
  class ThresholdMatch : public Archive::Match {

  public:

    //! Default constructor
    ThresholdMatch ();

    //! Set the maximum relative bandwidth by which archives may differ
    void set_maximum_relative_bandwidth (double delta_bw);

    //! Return true if the bandwidths match
    bool get_bandwidth_match (const Archive* a, const Archive* b) const;

    //! Setup for BPP-derived backends
    static void set_BPP (Archive*);

  protected:

    double maximum_relative_bandwidth;

  };

}

#endif
