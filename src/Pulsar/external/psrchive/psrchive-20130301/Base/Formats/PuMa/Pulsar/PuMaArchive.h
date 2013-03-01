//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PuMa/Pulsar/PuMaArchive.h,v $
   $Revision: 1.6 $
   $Date: 2009/01/09 11:40:06 $
   $Author: straten $ */

#ifndef __PuMaArchive_h
#define __PuMaArchive_h

#include "Pulsar/Archive.h"
#include "Pulsar/Agent.h"

// PuMa header definition
#include <pumadata.h>

namespace Pulsar {

  //! Loads and unloads PuMa Pulsar archives

  /*!  This class inherits the Archive class and impliments load
    and unload functions with which to read/write data from PuMa
    format files into a Pulsar::Archive class.

    The Dutch Pulsar Machine, PuMa, and its associated data file
    format were developed by Marco, Lodie, Ben and Ramach. */
  class PuMaArchive : public Archive {

  public:
    
    //! Default constructor
    PuMaArchive ();

    //! Copy constructor
    PuMaArchive (const PuMaArchive& archive);

    //! Destructor
    ~PuMaArchive ();

    //! Assignment operator
    const PuMaArchive& operator = (const PuMaArchive& archive);
    
    //! Base copy constructor
    PuMaArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    // //////////////////////////////////////////////////////////////////
    //
    // Implementation of Pulsar::Archive pure virtual methods
    //
    // //////////////////////////////////////////////////////////////////

    //! Return a new copy-constructed PuMaArchive instance
    Archive* clone () const;

    //! Get the number of pulsar phase bins used
    unsigned get_nbin () const;

    //! Get the number of frequency channels used
    unsigned get_nchan () const;

    //! Get the number of frequency channels used
    unsigned get_npol () const;

    //! Get the number of sub-integrations stored in the file
    unsigned get_nsubint () const;

    //! Get the telescope name
    std::string get_telescope () const;
    //! Set the telescope name
    void set_telescope (const std::string& telescope);

    //! Get the state of the profiles
    Signal::State get_state () const;
    //! Set the state of the profiles
    void set_state (Signal::State state);

    //! Get the scale of the profiles
    virtual Signal::Scale get_scale () const;
    //! Set the scale of the profiles
    virtual void set_scale (Signal::Scale scale);

    //! Get the observation type (psr, cal)
    Signal::Source get_type () const;
    //! Set the observation type (psr, cal)
    void set_type (Signal::Source type);

    //! Get the source name
    std::string get_source () const;
    //! Set the source name
    void set_source (const std::string& source);

    //! Get the backend
    std::string get_backend () const;
    //! Set the backend
    void set_backend (const std::string& bak);

    //! Get the receiver
    std::string get_receiver () const;
    //! Set the receiver
    void set_receiver (const std::string& rec);

    //! Get the coordinates of the source
    sky_coord get_coordinates () const;
    //! Set the coordinates of the source
    void set_coordinates (const sky_coord& coordinates);

    //! Get the centre frequency of the observation
    double get_centre_frequency () const;
    //! Set the centre frequency of the observation
    void set_centre_frequency (double cf);

    //! Get the overall bandwidth of the observation
    double get_bandwidth () const;
    //! Set the overall bandwidth of the observation
    void set_bandwidth (double bw);

    //! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    double get_dispersion_measure () const;
    //! Set the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
    void set_dispersion_measure (double dm);

    //! Inter-channel dispersion delay has been removed
    bool get_dedispersed () const;
    //! Set the status of the parallactic angle flag
    void set_dedispersed (bool done = true);

    //! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    double get_rotation_measure () const;
    //! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
    void set_rotation_measure (double rm);

    //! Data has been corrected for ISM faraday rotation
    bool get_faraday_corrected () const;
    //! Set the status of the ISM RM flag
    void set_faraday_corrected (bool done = true);

    //! Data has been poln calibrated
    bool get_poln_calibrated () const;
    //! Set the status of the poln calibrated flag
    void set_poln_calibrated (bool done = true);


  protected:

    //! Set the number of pulsar phase bins
    void set_nbin (unsigned numbins);

    //! Set the number of frequency channels
    void set_nchan (unsigned numchan);

    //! Set the number of polarization measurements
    void set_npol (unsigned numpol);

    //! Set the number of sub-integrations
    void set_nsubint (unsigned nsubint);

    //! The subints vector will point to BasicIntegrations
    Integration* new_Integration (const Integration* copy_this = 0);

    //! Load the PuMa header information from filename
    void load_header (const char* filename);

    //! Load the PuMa header information from filename
    static void load_header (Header_type& hdr, const char* filename);

    //! Load the specified Integration from filename, returning new instance
    Integration*
    load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is implemented
    bool can_unload () const { return true; }

    //! Unload the PuMaArchive (header and Integration data) to filename
    void unload_file (const char* filename) const;

    // //////////////////////////////////////////////////////////////////
    //
    // Pulsar::PuMaArchive::Agent
    //
    // //////////////////////////////////////////////////////////////////

    friend class Archive::Advocate<PuMaArchive>;

    //! This class registers the PuMaArchive plugin class for use
    class Agent : public Archive::Advocate<PuMaArchive> {

    public:

      //! Default constructor (necessary even when empty)
      Agent ();
      
      //! Advocate the use of PuMaArchive to interpret filename
      bool advocate (const char* filename);
      
      //! Return the name of the PuMaArchive plugin
      std::string get_name () { return "PuMa"; }
      
      //! Return description of this plugin
      std::string get_description ();
      
    };

  private:

    //! The PuMa binary header
    Header_type hdr;

    //! Number of polarizations
    unsigned npol;

    //! State of data
    Signal::State state;

    //! The type of observed source
    Signal::Source source;

    //! The size of the header information in bytes
    unsigned header_bytes;

    //! Set if the machine is little endian
    bool little_endian;

    //! Initialize all values to null
    void init ();

    //! Set the appropriate bit field in hdr.redn.GenType
    void set_GenType (int code, bool done);

  };
 

}

#endif
