//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __BPPArchive_h
#define __BPPArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"

// BPP header struct
#include <bpp_header.h>

namespace Pulsar {

  //! Loads and unloads BPP Pulsar archives
  /*! Class for loading Berkeley Pulsar Processor (BPP) data files.
    Based on ExampleArchive class.  Probably will not have unloading
    functionality ever. */
  class BPPArchive : public BasicArchive {

  public:
    
    //! Default constructor
    BPPArchive ();

    //! Copy constructor
    BPPArchive (const BPPArchive& archive);

    //! Destructor
    ~BPPArchive ();

    //! Assignment operator
    const BPPArchive& operator = (const BPPArchive& archive);
    
    //! Base copy constructor
    BPPArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    //! Return a new copy-constructed BPPArchive instance
    BPPArchive* clone () const;

    //! Different linearization methods  
    /*! Mean = use only mean power levels,
     *  Bins = use measured power at each pulse phase, 
     *  None = do nothing.
     */
    enum Linearization { Mean, Bins, None };
    
  protected:

    //! Correct the RFs array from the file.
    void fix_orig_rfs();

    //! Convert raw header data to big endian
    void hdr_to_big_endian ();

    //! Load the BPP header information from filename
    virtual void load_header (const char* filename);

    //! Get integer MJD out of the BPP header.
    int get_mjd_from_hdr();

    //! Selected linearization method
    Linearization lin_method;

    //! Linearize 2-bit power scale.
    int linearize_power(float quant_power, float *input_power, float *gain,
        float *xgain);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Fill in Archive extension info
    void load_extensions();

    //! The unload_file method is not implemented
    bool can_unload () const { return false; }

    //! Unload the BPPArchive (header and Integration data) to filename.
    virtual void unload_file (const char* filename) const;

    // Advocates the use of the BPPArchive plugin
    class Agent;

    //! Enable Advocate template class to access protected Agent class
    friend class Archive::Advocate<BPPArchive>;

  private:

    //! The raw BPP header struct
    bpp_header hdr;

    //! The original RFs array from the file.  NOTE: Don't use these
    //! unless orig_rfs_corrected has been set.
    double *orig_rfs;
    int orig_rfs_corrected;

    //! Initialize all values to null
    void init ();

  };
 

  // Advocates the use of the BPPArchive plugin
  class BPPArchive::Agent : public Archive::Advocate<BPPArchive> {
    
  public:
    
    Agent () { } 
    
    //! Advocate the use of BPPArchive to interpret filename
    bool advocate (const char* filename);
    
    //! Return the name of the BPPArchive plugin
    std::string get_name () { return "BPP"; }
    
    //! Return description of this plugin
    std::string get_description ();
    
  };


}

#endif
