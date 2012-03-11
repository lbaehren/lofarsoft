//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/FourthMoments.h,v $
   $Revision: 1.3 $
   $Date: 2009/06/08 19:23:32 $
   $Author: straten $ */

#ifndef __Pulsar_FourthMoments_h
#define __Pulsar_FourthMoments_h

#include "Pulsar/MoreProfiles.h"

namespace Pulsar
{
  /*! Phase-resolved fourth-order moments of the electric field */
  class FourthMoments : public MoreProfiles
  {
  public:

    //! Construct with a name
    FourthMoments (const char* name = "FourthMoments");
    
    //! Clone operator
    FourthMoments* clone () const;

    //! multiplies each bin of the profile by scale
    void scale (double scale);

    //! offsets each bin of the profile by offset
    void offset (double offset);

    //! integrate neighbouring phase bins in profile
    void bscrunch (unsigned nscrunch);

    //! integrate neighbouring phase bins in profile
    void bscrunch_to_nbin (unsigned nbin);

    //! integrate neighbouring sections of the profile
    void fold (unsigned nfold);

    //! average information from another MoreProfiles
    void average (const MoreProfiles*);

    //! self destruct on pscrunch
    void pscrunch ();

  };

}

#endif
