//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/dspReduction.h,v $
   $Revision: 1.13 $
   $Date: 2009/08/11 06:08:04 $
   $Author: straten $ */

#ifndef __dspReduction_h
#define __dspReduction_h

#include "Pulsar/Backend.h"

namespace Pulsar {

  //! Stores dspsr data reduction parameters
  /*! This class stores backend information specific to the baseband
    recording and processing system implemented by dspsr.
   */
  class dspReduction : public Pulsar::Backend {

  public:
    
    //! Default constructor
    dspReduction ();

    //! Copy constructor
    dspReduction (const dspReduction& extension);

    //! Assignment operator
    const dspReduction& operator= (const dspReduction& extension);

    //! Destructor
    ~dspReduction ();

    //! Clone method
    dspReduction* clone () const;

    //! get the text interface 
    TextInterface::Parser* get_interface();

    // Text interface to a dspReduction instance
    class Interface : public TextInterface::To<dspReduction>
    {
      public:
	Interface( dspReduction *s_instance = NULL );
    };

    //! Return a short name
    std::string get_short_name () const { return "dspsr"; }

    //////////////////////////////////////////////////////////////////////
    //
    // Backend implementation
    //
    //////////////////////////////////////////////////////////////////////
    
    //! Get the software information
    std::string get_software () const { return software; }

    //! Set the software information
    void set_software (const std::string& soft) { software = soft; }

    /** @name dsp::Input parameters
     *  These parameters describe the state of the dsp::Input from which the
     *  raw data was loaded
     */
    //@{

    //! The total number of time samples
    uint64_t get_total_samples () const { return total_samples; }
    //! The total number of time samples
    void set_total_samples (uint64_t _total_samples)
    { total_samples= _total_samples; }

    //! The number of time samples in each block
    uint64_t get_block_size () const { return block_size; }
    //! The number of time samples in each block
    void set_block_size (uint64_t _block_size)
    { block_size = _block_size; }

    //! The number of time samples by which consecutive blocks overlap
    uint64_t get_overlap () const { return overlap; }
    //! The number of time samples by which consecutive blocks overlap
    void set_overlap (uint64_t _overlap)
    { overlap = _overlap; }

    //! The state of the signal from Input::info
    Signal::State get_state () const { return state; }
    //! The state of the signal from Input::info
    void set_state (Signal::State _state)
    { state = _state; }

    //@}

    /** @name dsp::Convolution parameters 
     *  These parameters describe the state of the
     *  dsp::Convolution instance used to process the data.
     */
    //@{

    //! Number of time samples in each FFT
    unsigned get_nsamp_fft () const { return nsamp_fft; }
    //! Number of time samples in each FFT
    void set_nsamp_fft (unsigned _nsamp_fft)
    { nsamp_fft = _nsamp_fft; }

    //! Number of time samples in wrap-around region, left-hand side
    unsigned get_nsamp_overlap_pos () const { return nsamp_overlap_pos; }
    //! Number of time samples in wrap-around region, left-hand side
    void set_nsamp_overlap_pos (unsigned _nsamp_overlap_pos)
    { nsamp_overlap_pos = _nsamp_overlap_pos; }

    //! Number of time samples in wrap-around region, right-hand side
    unsigned get_nsamp_overlap_neg () const { return nsamp_overlap_neg; }
    //! Number of time samples in wrap-around region, right-hand side
    void set_nsamp_overlap_neg (unsigned _nsamp_overlap_neg)
    { nsamp_overlap_neg = _nsamp_overlap_neg; }

    //@}

    /** @name dsp::Filterbank parameters 
     *  These parameters describe the state of the
     *  dsp::Filterbank instance used to process the data.
     */
    //@{

    //! Number of frequency channels
    unsigned get_nchan () const { return nchan; }
    //! Number of frequency channels
    void set_nchan (unsigned _nchan)
    { nchan = _nchan; }

    //! Frequency resolution
    unsigned get_freq_res () const { return freq_res; }
    //! Frequency resolution
    void set_freq_res (unsigned  _freq_res)
    {  freq_res= _freq_res; }

    //! Time resolution
    unsigned get_time_res () const { return time_res; }
    //! Time resolution
    void set_time_res (unsigned _time_res)
    {  time_res= _time_res; }

    //@}


    /** @name dsp::TScrunch parameters 
     *  These parameters describe the state of the
     *  dsp::TScrunch instance used to process the data.
     */
    //@{

    //! Time resolution reduction factor
    unsigned get_ScrunchFactor () const { return ScrunchFactor; }
    //! Time resolution reduction factor
    void set_ScrunchFactor (unsigned _ScrunchFactor)
    { ScrunchFactor = _ScrunchFactor; }


    //@}

    /** @name dsp::PhaseSeries parameters 
     *  These parameters describe the state of the dsp::PhaseSeries
     *  instance from which the Pulsar::Profile data are constructed.
     */
    //@{

    //! Pulsar::Profiles are normalized by Observation::scale 
    double get_scale () const { return scale; }
    //! Pulsar::Profiles are normalized by Observation::scale 
    void set_scale (double _scale) 
    { scale = _scale; }

    //@}


  protected:

    //! The software information
    std::string software;

    /** @name dsp::Input parameters
     *  These parameters describe the state of the dsp::Input from which the
     *  raw data was loaded
     */
    //@{

    //! The total number of time samples
    uint64_t total_samples;

    //! The number of time samples in each block
    uint64_t block_size;

    //! The number of time samples by which consecutive blocks overlap
    uint64_t overlap;

    //! The state of the signal from Input::info
    Signal::State state;

    //@}

    /** @name dsp::Convolution parameters 
     *  These parameters describe the state of the
     *  dsp::Convolution instance used to process the data.
     */
    //@{

    //! Number of time samples in each FFT
    unsigned nsamp_fft;

    //! Number of time samples in wrap-around region, left-hand side
    unsigned nsamp_overlap_pos;

    //! Number of time samples in wrap-around region, right-hand side
    unsigned nsamp_overlap_neg;

    //@}

    /** @name dsp::Filterbank parameters 
     *  These parameters describe the state of the
     *  dsp::Filterbank instance used to process the data.
     */
    //@{

    //! Number of frequency channels
    unsigned nchan;

    //! Frequency resolution
    unsigned freq_res;

    //! Time resolution
    unsigned time_res;

    //@}


    /** @name dsp::TScrunch parameters 
     *  These parameters describe the state of the
     *  dsp::TScrunch instance used to process the data.
     */
    //@{

    //! Time resolution reduction factor
    unsigned ScrunchFactor;

    //@}

    /** @name dsp::PhaseSeries parameters 
     *  These parameters describe the state of the dsp::PhaseSeries
     *  instance from which the Pulsar::Profile data are constructed.
     */
    //@{

    //! Pulsar::Profiles are normalized by Observation::scale 
    double scale;

    //@}

  };
 

}

#endif
