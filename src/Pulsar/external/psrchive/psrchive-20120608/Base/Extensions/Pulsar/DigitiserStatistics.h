//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// Class for holding digitiser statistics

#ifndef __DigitiserStatistics_h
#define __DigitiserStatistics_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar
{

  //! Stores digitizer statistics
  class DigitiserStatistics : public Pulsar::Archive::Extension
  {

  public:

    //Default constructor
    DigitiserStatistics ();

    // Copy constructor
    DigitiserStatistics (const DigitiserStatistics& extension);

    // Operator =
    const DigitiserStatistics& operator= (const DigitiserStatistics&);

    // Destructor
    ~DigitiserStatistics ();

    //! Clone method
    DigitiserStatistics* clone () const
      { return new DigitiserStatistics( *this ); }

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();
    class Interface;

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "dig"; }

    //! set ndigr
    void set_ndigr( int s_ndigr ) { ndigr = s_ndigr; }

    //! get_ndigr
    int get_ndigr( void ) const { return ndigr; }

    //! set npar
    void set_npar( int s_npar ) { npar = s_npar; }

    //! get npar
    int get_npar( void ) const { return npar; }

    //! set ncycsub
    void set_ncycsub( int s_ncycsub ) { ncycsub = s_ncycsub; }

    //! get ncycsub
    int get_ncycsub( void ) const { return ncycsub; }

    //! set diglev
    void set_diglev( std::string s_diglev ) { diglev = s_diglev; }

    //! get diglev
    std::string get_diglev( void ) const { return diglev; }

    //! set dig_mode
    void set_dig_mode( std::string s_dig_mode ) { dig_mode = s_dig_mode; }
    
    //! get dig_mode
    std::string get_dig_mode( void ) const { return dig_mode; }

    //! Resize the extension
    void resize (unsigned nsubint,
		    unsigned npol  = 0,
		    unsigned nchan = 0,
		    unsigned nbin  = 0);

    ///////////////////////////////////////////////////////////////
    // The information container

    class row : public Reference::Able
    {

    public:

      row () { init(); }
      ~row ();

      float get_atten (unsigned index) const { return atten.at(index); }
      void set_atten (unsigned index, float value) { atten.at(index) = value; }
      unsigned get_natten () const { return atten.size(); }

      int index;

      std::vector<float> atten;
      std::vector<float> data;

    private:

      void init ();

    };

    std::vector<row> rows;

    DigitiserStatistics::row& get_row (unsigned i);
    DigitiserStatistics::row& get_last ();

    DigitiserStatistics::row* get_row_ptr (unsigned i) { return &rows.at(i); }
    unsigned get_nrow () const { return rows.size(); }

    void push_blank_row ();

  private:

    int ndigr;
    int npar;
    int ncycsub;
    std::string diglev;
    std::string dig_mode;

    void init ();

  };

}

#endif
