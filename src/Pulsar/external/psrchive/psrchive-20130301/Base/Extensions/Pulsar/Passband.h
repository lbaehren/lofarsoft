//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Passband.h,v $
   $Revision: 1.12 $
   $Date: 2008/04/24 02:35:53 $
   $Author: straten $ */

#ifndef __Pulsar_Passband_h
#define __Pulsar_Passband_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Instrumental passband (or bandpass)
  /*! This Extension stores the instrumental bandpass in a Pulsar::Archive. */
  class Passband : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    Passband ();

    //! Copy constructor
    Passband (const Passband& extension);

    //! Operator =
    const Passband& operator= (const Passband& extension);

    //! Destructor
    ~Passband ();

    //! Clone method
    Passband* clone () const { return new Passband( *this ); }
    
    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();
    class Interface;

    std::string get_short_name () const { return "band"; }

    //! Get the number of frequency channels in each passband
    unsigned get_nchan () const;

    //! Get the number of polarizations
    unsigned get_npol () const;

    //! Get the number of bands
    unsigned get_nband () const;

    //! Set the number of channels, polarizations, and bands
    void resize (unsigned nchan, unsigned npol, unsigned nband=1);

    //! Get the specified passband
    const std::vector<float>&
    get_passband (unsigned ipol, unsigned iband=0) const;

    //! Set the specified passband
    void set_passband (const std::vector<float>&,
                       unsigned ipol, unsigned iband=0);

    //! Set the specified passband
    void set_passband (const float* data, unsigned ipol, unsigned iband=0);

    //! Set all passband datum (not dimensions) to zero
    void zero ();

  protected:

    //! Number of frequency channels
    unsigned nchan;

    //! Number of polarizations
    /*! A full polarimetric representation of the bandpass may be stored. */
    unsigned npol;

    //! Number of frequency bands
    /*! The number of frequency bands may be other than one when an Archive
      contains data from multiple bands. */
    unsigned nband;

    //! Average passbands
    /*! The passbands are organized in ipol major order, I think.
      band0,pol0 band0,pol2 ... bandN,polM band1,pol0 ... */
    std::vector< std::vector<float> > passband;

    //! Throw an exception if ipol or iband are out of range
    void range_check (unsigned ipol, unsigned iband, const char* method) const;

  };
 
}

#endif
