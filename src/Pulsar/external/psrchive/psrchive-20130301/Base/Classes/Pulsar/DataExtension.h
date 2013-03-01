//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/DataExtension.h,v $
   $Revision: 1.3 $
   $Date: 2009/06/08 19:23:37 $
   $Author: straten $ */

#ifndef __Pulsar_Data_Extension_h
#define __Pulsar_Data_Extension_h

#include "Pulsar/ProfileExtension.h"

namespace Pulsar
{
  /*! Profile Extension with extra phase-resolved data */
  class DataExtension : public Profile::Extension
  {
  public:

    //! Construct with a name
    DataExtension (const char* name) : Profile::Extension (name) { }

    //! Get the number of phase bins in the extra data
    virtual unsigned get_nbin() const = 0;
   
    //! Resize the data area
    virtual void resize (unsigned nbin) = 0;

    //! set the weight of the profile
    virtual void set_weight (float wt) = 0;

    //! multiplies each bin of the profile by scale
    virtual void scale (double scale) = 0;

    //! offsets each bin of the profile by offset
    virtual void offset (double offset) = 0;

    //! rotates the profile by phase (in turns)
    virtual void rotate_phase (double phase) = 0;

    //! set all amplitudes to zero
    virtual void zero () = 0;

    //! integrate neighbouring phase bins in profile
    virtual void bscrunch (unsigned nscrunch) = 0;

    //! integrate neighbouring phase bins in profile
    virtual void bscrunch_to_nbin (unsigned nbin) = 0;

    //! integrate neighbouring sections of the profile
    virtual void fold (unsigned nfold) = 0;

    //! some extensions may have to respond to pscrunching
    virtual void pscrunch () {}
  };

}

#endif
