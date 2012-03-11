//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Wavelet_h
#define __Pulsar_Wavelet_h

#include "ReferenceAble.h"

#include <gsl/gsl_wavelet.h>

namespace Pulsar {

  class Profile;

  //! Performs 1-D discrete wavelet transforms (DWT)
  /*! This class is basically a wrapper for the GSL wavelet implementation.
   * Refer to GSL documentation for more info. 
   */
  class WaveletTransform : public Reference::Able {

    public:

      //! Constructor
      WaveletTransform();

      //! Destructor
      ~WaveletTransform();

      //! Set wavelet type
      /*! Possible types (as of GSL 1.11) are:
       *  <ol>
       *    <li> gsl_wavelet_daubechies
       *    <li> gsl_wavelet_haar
       *    <li> gsl_wavelet_bspline
       *  </ol>
       *  All types can have _centered appended to their name to 
       *  get centered versions.
       */
      void set_type(const gsl_wavelet_type *t) { type=t; };

      //! Set wavelet order
      /*! Implemented values (as of GSL 1.11) are:
       *  <ol>
       *    <li> daubechies: order=4,6,...,20 (even values only).
       *    <li> haar: order=2 only.
       *    <li> bspline: order=103,105,202,204,206,208,301,303,305,307,309.
       *  </ol>
       */
      void set_order(int o) { order=o; };

      //! Set wavelet type and order using short string notation
      void set_wavelet(std::string s);

      //! Set whether or not to decimate during transform
      void set_decimate(bool _dec=true) { decimate = _dec; }

      //! Perform forward transform
      void transform(unsigned npts, const float *in);

      //! Perform forward transform
      void transform(const std::vector<float>& in);

      //! Perform forward transform
      void transform(const Pulsar::Profile* in);

      //! Perform inverse transform in-place
      void invert();

      //! Possible states of the data
      enum State { Wavelet, Time, Empty };

      //! Get decimation setting
      const bool get_decimate() const { return decimate; }

      //! Get current state
      State get_state() { return state; }

      //! Get wavelet data
      double get_data(int level, int k);

      //! Get data element
      double get_data(int n);

      //! Get pointer to data array
      double* get_data();

      //! Get const pointer to data array
      const double* get_data() const;

      //! Get number of pts currently in use
      const int get_npts() const { return npts; };

      //! Get log2 of number of pts currently in use
      const int get_log2_npts() const { return log2_npts; };

      //! Get total number of wavelet coeffs computed
      const int get_ncoeff() const { return decimate ? npts : log2_npts*npts; }

      //! Get number of coeffs at a given level
      const int get_ncoeff(int level) const;

    protected:

      //! Wavelet type
      const gsl_wavelet_type *type;

      //! Use decimated (standard) or undecimated transform
      bool decimate;

      //! Wavelet order
      int order;

      //! Wavelet coeffs
      gsl_wavelet *wave;

      //! Number of points
      int npts;

      //! Log2 of npts
      int log2_npts;

      //! Data storage
      double *data;

      //! Mean of data
      double mean;

      //! Current state of the data
      State state;

      //! GSL workspace
      gsl_wavelet_workspace *work;

    private:

      //! Free internal memory
      void free_mem();

      //! Allocate internal memory, set up wavelet coeffs
      void init_mem();


  };

}

#endif
