//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FITSHdrExtension.h,v $
   $Revision: 1.22 $
   $Date: 2009/04/08 05:19:15 $
   $Author: straten $ */

#ifndef __FITSHdrExtension_h
#define __FITSHdrExtension_h

#include "Pulsar/ArchiveExtension.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Stores PSRFITS header extensions
  class FITSHdrExtension : public Pulsar::Archive::Extension
  {

  public:

    //! Default constructor
    FITSHdrExtension ();

    //! Copy constructor
    FITSHdrExtension (const FITSHdrExtension& extension);

    //! Clone method
    FITSHdrExtension* clone () const { return new FITSHdrExtension( *this ); }

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    // The text interface to a FITSHdrExtension object
    class Interface : public TextInterface::To<FITSHdrExtension>
    {
      public:
	Interface( FITSHdrExtension *s_instance = NULL );
    };

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "ext"; }

    //! Return two strings based on coordmode attribute
    void get_coord_string (const sky_coord& coordinates,
                           std::string& coord1, std::string& coord2) const;

    //! Get the coordinate mode
    std::string get_coordmode () const;
    //! Set the coordinate mode string
    void set_coordmode (const std::string& mode);

    //! Set the creation date string
    void set_date_str (const std::string& date);

    //! Get the header version
    std::string get_hdrver () const;
    void set_hdrver (const std::string& txt) { hdrver = txt; }

    //! Get the file creation date
    std::string get_creation_date () const;
    void set_creation_date (const std::string& txt) { creation_date = txt; }

    //! Get the observation mode
    std::string get_obs_mode () const { return obs_mode; }
    void set_obs_mode (const std::string& txt) { obs_mode = txt; }

    //! Get the equinox
    std::string get_equinox () const;
    void set_equinox (const std::string& txt) { equinox = txt; }

    //! Get tracking mode
    std::string get_trk_mode () const;
    void set_trk_mode (const std::string& txt) { trk_mode = txt; }

    //! Get the beam position angle
    double get_bpa () const { return bpa; }
    //! Set the beam position angle
    void set_bpa( const double _bpa ) { bpa = _bpa; }
    
    //! Get the beam major axis length
    double get_bmaj () const { return bmaj; }
    //! Set the beam major axis length
    void set_bmaj( const double _bmaj ) { bmaj = _bmaj; }

    //! Get the beam minor axis length
    double get_bmin () const { return bmin; }
    //! Set the beam minor axis length
    void set_bmin( const double _bmin ) { bmin = _bmin; }

    //! Get the start date
    std::string get_stt_date () const { return stt_date; }
    void set_stt_date (const std::string& date) { stt_date = date; }

    //! Get the start time
    std::string get_stt_time () const { return stt_time; }
    void set_stt_time (const std::string& time) { stt_time = time; }

    //! Get the stt_imjd
    int get_stt_imjd () const { return start_time.intday(); }
    void set_stt_imjd (int);

    //! Get the stt_smjd
    int get_stt_smjd () const { return start_time.get_secs(); }
    void set_stt_smjd (int);

    //! Get the stt_offs
    double get_stt_offs () const { return start_time.get_fracsec(); }
    void set_stt_offs (double);

    //! Get the start time
    MJD get_start_time () const { return start_time; }
    void set_start_time (const MJD& mjd);

    //! Get stt_lst
    double get_stt_lst () const { return stt_lst; }
    void set_stt_lst (double lst) { stt_lst = lst; }

    // ra
    std::string ra;
    std::string get_ra () const { return ra; }
    void set_ra( std::string new_ra ) { ra = new_ra; }

    // dec
    std::string dec;
    std::string get_dec () const { return dec; }
    void set_dec( std::string new_dec ) { dec = new_dec; }

    // stt_crd1
    std::string stt_crd1;
    std::string get_stt_crd1 () const { return stt_crd1; }
    void set_stt_crd1( std::string new_stt_crd1 ) { stt_crd1 = new_stt_crd1; }

    // stt_crd2
    std::string stt_crd2;
    std::string get_stt_crd2 () const { return stt_crd2; }
    void set_stt_crd2( std::string new_stt_crd2 ) { stt_crd2 = new_stt_crd2; }

    // stp_crd1
    std::string stp_crd1;
    std::string get_stp_crd1 () const { return stp_crd1; }
    void set_stp_crd1( std::string new_stp_crd1 ) { stp_crd1 = new_stp_crd1; }

    // stp_crd2
    std::string stp_crd2;
    std::string get_stp_crd2 () const { return stp_crd2; }
    void set_stp_crd2( std::string new_stp_crd2 ) { stp_crd2 = new_stp_crd2; }

    //! Set the observed frequency
    void set_obsfreq( double set_obsfreq );

    //! Get the observed frequency
    double get_obsfreq () const;

    //////////////////////////////////////////////////////////////////////

    //! Pulsar FITS header version information
    std::string hdrver;
    int major_version;
    int minor_version;

    //! File creation date
    std::string creation_date;

    //! Coordinate mode (EQUAT, GAL, ECLIP, AZEL, HADEC)
    std::string coordmode;

    //! Equinox of coordinates
    std::string equinox;

    //! Track mode (TRACK, SCANGC, SCANLAT)
    std::string trk_mode;

    //! Start UT date (YYYY-MM-DD)
    std::string stt_date;

    //! Start UT (hh:mm:ss)
    std::string stt_time;

    //! Start LST
    double stt_lst;

    //! Observed Frequency
    double obsfreq;

    //! Observation Mode
    std::string obs_mode;

    //! Beam position angle
    double bpa;

    //! Beam major axis length
    double bmaj;

    //! Beam minor axis length
    double bmin;

  protected:

    //! Header start time (as opposed to subint start time)
    MJD start_time;

  };

}

#endif

