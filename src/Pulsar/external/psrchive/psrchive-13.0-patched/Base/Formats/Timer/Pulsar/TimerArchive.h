//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/TimerArchive.h,v $
   $Revision: 1.27 $
   $Date: 2008/11/07 22:15:36 $
   $Author: straten $ */

#ifndef __Timer_Archive_h
#define __Timer_Archive_h

#include "Pulsar/Archive.h"
#include "Pulsar/Agent.h"
#include "timer.h"

namespace Pulsar {

  class TapeInfo;
  class CalInfoExtension;

  //! Reads and writes the timer archive file format
  class TimerArchive : public Archive {

  public:

    static bool big_endian;

    //! Default constructor
    TimerArchive ();

    //! Copy constructor
    TimerArchive (const TimerArchive& archive);

    //! Destructor
    ~TimerArchive ();

    //! Assignment operator
    const TimerArchive& operator = (const TimerArchive& archive);
    
    //! Base copy constructor
    TimerArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    // //////////////////////////////////////////////////////////////////
    //
    // implement the pure virtual methods of the Archive base class
    //

    //! Return a pointer to a new copy constructed instance equal to this
    TimerArchive* clone () const;

    //! Get the telescope name
    virtual std::string get_telescope () const;
    //! Set the telescope name
    virtual void set_telescope (const std::string& telescope);

    //! Get the state of the profiles
    virtual Signal::State get_state () const;
    //! Set the state of the profiles
    virtual void set_state (Signal::State state);

    //! Get the scale of the profiles
    virtual Signal::Scale get_scale () const;
    //! Set the scale of the profiles
    virtual void set_scale (Signal::Scale scale);

    //! Get the observation type (psr, cal)
    virtual Signal::Source get_type () const;
    //! Set the observation type (psr, cal)
    virtual void set_type (Signal::Source type);

    //! Get the source name
    virtual std::string get_source () const;
    //! Set the source name
    virtual void set_source (const std::string& source);

    //! Get the coordinates of the source
    virtual sky_coord get_coordinates () const;
    //! Set the coordinates of the source
    virtual void set_coordinates (const sky_coord& coordinates);

    //! Get the number of pulsar phase bins used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nbin () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nchan () const;

    //! Get the number of frequency channels used
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_npol () const;

    //! Get the number of sub-integrations stored in the file
    /*! This attribute may be set only through Archive::resize */
    virtual unsigned get_nsubint () const;

    //! Get the overall bandwidth of the observation
    virtual double get_bandwidth () const;
    //! Set the overall bandwidth of the observation
    virtual void set_bandwidth (double bw);

    //! Get the centre frequency of the observation
    virtual double get_centre_frequency () const;
    //! Set the centre frequency of the observation
    virtual void set_centre_frequency (double cf);

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual double get_dispersion_measure () const;
    //! Set the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    virtual void set_dispersion_measure (double dm);

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual double get_rotation_measure () const;
    //! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    virtual void set_rotation_measure (double rm);

    //! Inter-channel dispersion delay has been removed
    virtual bool get_dedispersed () const;
    //! Set true when the inter-channel dispersion delay has been removed
    virtual void set_dedispersed (bool done = true);

    //! Data has been corrected for ISM faraday rotation
    virtual bool get_faraday_corrected () const;
    //! Set the status of the ISM RM flag
    virtual void set_faraday_corrected (bool done = true);

    //! Data has been poln calibrated
    virtual bool get_poln_calibrated () const;
    //! Set the status of the poln calibrated flag
    virtual void set_poln_calibrated (bool done = true);

    //! Returns the Hydra observation type, given the coordinates
    int hydra_obstype ();

    //! Set various redundant parameters in the timer and mini headers
    virtual void correct ();

  protected:

    friend class Archive::Advocate<TimerArchive>;

    // Advocates the use of the TimerArchive plugin
    class Agent : public Archive::Advocate<TimerArchive> {
    
      public:

        //! Default constructor (necessary even when empty)
        Agent () {}

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        std::string get_name () { return "Timer"; }

        //! Return description of this plugin
        std::string get_description ();

    }; 

    //! Load the header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is implemented
    bool can_unload () const { return true; }

    //! Unload the Archive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    //! The original FPTM header information
    struct timer hdr;

    //! State of the TimerArchive attributes can be trusted
    /*! This flag is set true only when data is loaded from a file.
      Whenever a new TimerArchive is created in memory, the
      TimerArchive::correct method should be called to ensure that the
      state of all timer and mini header variables are properly set. */
    bool valid;

    //! Set the number of pulsar phase bins
    virtual void set_nbin (unsigned numbins);

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned numchan);

    //! Set the number of polarization measurements
    virtual void set_npol (unsigned numpol);

    //! Set the number of sub-integrations
    virtual void set_nsubint (unsigned nsubint);

    //! The subints vector will point to TimerIntegrations
    virtual Integration* new_Integration (const Integration* copy_this = 0);

    //! set code bit in hdr.corrected
    void set_corrected (int code, bool done);

    //! set code bit in hdr.calibrated
    void set_calibrated (int code, bool done);

    //! load the archive from an open file
    void load (FILE* fptr);

    //! unload the archive to an open file
    void unload (FILE* fptr) const;

    // loading is broken up into the following steps, so that inherited
    // types can use the building blocks to do different things

    //! load the timer header from an open file
    void hdr_load (FILE* fptr);

    //! load the backend-specific information from an open file
    virtual void backend_load (FILE* fptr);

    //! load the polyco and ephemeris from an open file
    void psr_load (FILE* fptr);

    //! load the sub-integrations from an open file
    void subint_load (FILE* fptr);

    // unloading is broken up into the following steps, so that inherited
    // types can use the building blocks to do different things

    //! unload the timer header to an open file
    void hdr_unload (FILE* fptr) const;

    //! unload the backend-specific information to an open file
    virtual void backend_unload (FILE* fptr) const;

    //! unload the polyco and ephemeris from an open file
    void psr_unload (FILE* fptr) const;

    //! unload the sub-integrations to an open file
    void subint_unload (FILE* fptr) const;

    //! Set the state of various redundant parameters in the mini headers
    void correct_Integrations ();

    //! Unpack the Receiver and Telescope Extension classes (after loading)
    void unpack_extensions ();

    //! Pack the Receiver and Telescope Extension classes (before unloading)
    void pack_extensions () const;

    //! Unpack the Receiver extension
    void unpack (Receiver* receiver);

    //! Pack the Receiver extension
    void pack (const Receiver* receiver);

    //! Unpack the TapeInfo extension
    void unpack (TapeInfo* tape);

    //! Pack the TapeInfo extension
    void pack (const TapeInfo* tape);

    //! Unpack the CalInfoExtension
    void unpack (CalInfoExtension* cal);

  };



}

#endif
