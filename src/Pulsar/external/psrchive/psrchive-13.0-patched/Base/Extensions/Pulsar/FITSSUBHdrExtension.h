//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FITSSUBHdrExtension.h,v $
   $Revision: 1.9 $
   $Date: 2009/11/04 00:13:32 $
   $Author: jonathan_khoo $ */

#ifndef __Pulsar_FITSSUBHdrExtension_h
#define __Pulsar_FITSSUBHdrExtension_h

#include "Pulsar/ArchiveExtension.h"
#include <TextInterface.h>



namespace Pulsar
{

  //! Stores PSRFITS SUBINT header parameters
  class FITSSUBHdrExtension : public Archive::Extension
  {
  public:
    //! Default constructor
    FITSSUBHdrExtension ();

    //! Copy constructor
    FITSSUBHdrExtension (const Pulsar::FITSSUBHdrExtension& extension);

    //! Operator =
    const FITSSUBHdrExtension& operator= (const FITSSUBHdrExtension& extension);

    //! Destructor
    ~FITSSUBHdrExtension ();

    //! Clone method
    FITSSUBHdrExtension* clone () const { return new FITSSUBHdrExtension( *this ); }

    //! Get the text interface
    TextInterface::Parser* get_interface();

    // Text interface to a FITSSUBHdrExtension
    class Interface : public TextInterface::To<FITSSUBHdrExtension>
    {
      public:
	Interface( FITSSUBHdrExtension *s_instance = NULL );
    };

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "sub"; }

    std::string get_int_type( void ) const { return int_type; }
    std::string get_int_unit( void ) const { return int_unit; }
    double get_tsamp( void ) const { return tsamp; }
    int get_nbits( void ) const { return nbits; }
    int get_nch_strt( void ) const { return nch_strt; }
    int get_nsblk( void ) const { return nsblk; }
    long get_nrows( void ) const { return nrows; }

    void set_int_type( std::string s_int_type )  { int_type = s_int_type; }
    void set_int_unit( std::string s_int_unit )  { int_unit = s_int_unit; }
    void set_tsamp( double s_tsamp )  { tsamp = s_tsamp; }
    void set_nbits( int s_nbits )  { nbits = s_nbits; }
    void set_nch_strt( int s_nch_strt )  { nch_strt = s_nch_strt; }
    void set_nsblk( int s_nsblk )  { nsblk = s_nsblk; }
    void set_nrows( int s_nrows )  { nrows = s_nrows; }

  private:
    std::string int_type;
    std::string int_unit;
    double tsamp;
    int nbits;
    int nch_strt;
    int nsblk;
    long nrows;
  };
}

#endif
