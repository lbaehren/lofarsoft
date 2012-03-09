//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CoherentDedispersion.h,v $
   $Revision: 1.4 $
   $Date: 2010/04/06 21:44:07 $
   $Author: straten $ */

#ifndef __CoherentDedispersion_h
#define __CoherentDedispersion_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Record of phase-coherent dispersion removal algorithm
  /*! This class stores information about any phase-coherent
    dispersion removal performed by the backend, if applicable.
   */
  class CoherentDedispersion : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    CoherentDedispersion ();

    //! Clone method
    CoherentDedispersion* clone () const;

    //! get the text interface 
    TextInterface::Parser* get_interface();

    // Text interface to a CoherentDedispersion instance
    class Interface : public TextInterface::To<CoherentDedispersion>
    {
      public:
	Interface( CoherentDedispersion* = NULL );
    };

    //! Return a short name
    std::string get_short_name () const { return "cd"; }

    //////////////////////////////////////////////////////////////////////
    //
    // OutputChannel nested class
    //
    //////////////////////////////////////////////////////////////////////

    //! Coherent dedispersion attributes specific to output frequency channel
    class OutputChannel
    {

    public:

      //! Centre frequency of output channel
      double get_centre_frequency () const { return centre_frequency; }
      //! Centre frequency of output channel
      void set_centre_frequency (double val) { centre_frequency = val; }

      //! Bandwidth of output channel
      double get_bandwidth () const { return bandwidth; }
      //! Bandwidth of output channel
      void set_bandwidth (double val) { bandwidth = val; }

      //! Number of complex time samples in each cyclical convolution
      unsigned get_nsamp () const { return nsamp; }
      //! Number of complex time samples in each cyclical convolution
      void set_nsamp (unsigned val) { nsamp = val; }

      //! Number of complex time samples in wrap-around region, left-hand side
      unsigned get_nsamp_overlap_pos () const { return nsamp_overlap_pos; }
      //! Number of complex time samples in wrap-around region, left-hand side
      void set_nsamp_overlap_pos (unsigned val) { nsamp_overlap_pos = val; }
      
      //! Number of complex time samples in wrap-around region, right-hand side
      unsigned get_nsamp_overlap_neg () const { return nsamp_overlap_neg; }
      //! Number of complex time samples in wrap-around region, right-hand side
      void set_nsamp_overlap_neg (unsigned val) { nsamp_overlap_neg = val; }

    protected:
      
      //! Centre frequency of output channel
      double centre_frequency;
      
      //! Bandwidth of output channel
      double bandwidth;
      
      //! Number of complex time samples in each cyclical convolution
      unsigned nsamp;
      
      //! Number of complex time samples in wrap-around region, left-hand side
      unsigned nsamp_overlap_pos;
      
      //! Number of complex time samples in wrap-around region, right-hand side
      unsigned nsamp_overlap_neg;

    };

    //////////////////////////////////////////////////////////////////////
    //
    // InputChannel nested class
    //
    //////////////////////////////////////////////////////////////////////

    //! Coherent dedispersion attributes specific to input frequency channel
    class InputChannel
    {

    public:

      //! Centre frequency of input channel
      double get_centre_frequency () const { return centre_frequency; }
      //! Centre frequency of input channel
      void set_centre_frequency (double val) { centre_frequency = val; }
      
      //! Bandwidth of input channel
      double get_bandwidth () const { return bandwidth; }
      //! Bandwidth of input channel
      void set_bandwidth (double val) { bandwidth = val; }
      
      //! Number of frequency channels into which this channel was divided
      unsigned get_nchan_output () const { return output.size(); }
      void set_nchan_output (unsigned nchan) { output.resize( nchan ); }
      
      const OutputChannel& get_output (unsigned ichan_output) const;
      OutputChannel& get_output (unsigned ichan_output);
      
    protected:
      
      //! Centre frequency of input channel
      double centre_frequency;
      
      //! Bandwidth of input channel
      double bandwidth;
      
      //! Information specific to each output frequency channel
      std::vector<OutputChannel> output;
      
      void check_index (unsigned ichan_output) const;
    };

    //////////////////////////////////////////////////////////////////////
    //
    // CoherentDedisperion class attributes/methods
    //
    //////////////////////////////////////////////////////////////////////

    //! Domain in which the algorithm operates (time or frequency)
    Signal::Dimension get_domain () const { return domain; }
    void set_domain (Signal::Dimension val) { domain = val; }

    //! Description of the algorithm
    std::string get_description () const { return description; }
    void set_description (const std::string& val) { description = val; }

    //! Dispersion measure used for coherent dedispersion
    double get_dispersion_measure () const { return dispersion_measure; }
    void set_dispersion_measure (double val) { dispersion_measure = val; }

    //! Doppler shift correction applied to frequencies and bandwidths
    double get_doppler_correction () const { return doppler_correction; }
    void set_doppler_correction (double val) { doppler_correction = val; }

    //! Number of bits used to represent data
    int get_nbit_data () const { return nbit_data; }
    void set_nbit_data (int nbit) { nbit_data = nbit; }

    //! Number of bits used to represent chirp
    int get_nbit_chirp () const { return nbit_chirp; }
    void set_nbit_chirp (int nbit) { nbit_chirp = nbit; }

    //! Number of frequency channels in the input signal
    unsigned get_nchan_input () const { return input.size(); }
    void set_nchan_input (unsigned nchan) { input.resize( nchan ); }

    const InputChannel& get_input (unsigned ichan_input) const;
    InputChannel& get_input (unsigned ichan_input);

  protected:

    //! Domain in which the algorithm operates (time or frequency)
    Signal::Dimension domain;

    //! Description of the algorithm
    std::string description;

    //! Dispersion measure used for coherent dedispersion
    double dispersion_measure;

    //! Doppler shift correction applied to frequencies and bandwidths
    double doppler_correction;

    //! Number of bits used to represent data
    int nbit_data;

    //! Number of bits used to represent chirp
    int nbit_chirp;

    //! Information specific to each input frequency channel
    std::vector<InputChannel> input;

    //! Throw an exception if index is out of range
    void check_index (unsigned ichan_input) const;
  };

}

#endif
