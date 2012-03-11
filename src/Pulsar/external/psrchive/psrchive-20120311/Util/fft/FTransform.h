//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FTransform_h_
#define __FTransform_h_

#include "Reference.h"

#include <string>
#include <vector>

//! Defines a single interface to a variety of Fourier transform libraries
namespace FTransform {

  /* ////////////////////////////////////////////////////////////////////
     
  One-dimensional FFT library interface
  
  //////////////////////////////////////////////////////////////////// */

  //! Forward real-to-complex FFT 
  /*! 'into' must point to an array of nfft+2 floats */
  void frc1d (size_t nfft, float* into, const float* from);

  //! Backward complex-to-real FFT 
  /*! both 'into' and 'from' must point to an array of nfft+2 floats */
  void bcr1d (size_t nfft, float* into, const float* from);

  //! Forward complex-to-complex FFT
  void fcc1d (size_t nfft, float* into, const float* from);

  //! Backward complex-to-complex FFT
  void bcc1d (size_t nfft, float* into, const float* from);

  //! Whether to optimize or not
  extern bool optimize;

  //! Whether to use SIMD or not
  extern bool simd;

  //! Number of threads that will use the plan
  extern unsigned nthread;

  //! Returns the name of the currently selected library
  std::string get_library ();
  
  //! Set the library to the specified name
  void set_library (const std::string& name);

  //! Get the number of available libraries
  unsigned get_num_libraries ();

  //! Get the name of the ith available library
  std::string get_library_name (unsigned i);

  //! Clears out the memory associated with the plans
  void clean_plans();

  //! The normalization convention
  enum normalization { normalized, unnormalized };

  //! The FFT direction
  enum direction { forward = 0x10, backward = 0x20 };

  //! The FFT dimension
  enum dimension { real = 0x01, analytic = 0x02 };

  //! The FFT type
  enum type {
    //! forward real to complex
    frc = forward  | real,
    //! backward complex to real
    bcr = backward | real,
    //! forward complex to complex
    fcc = forward  | analytic,
    //! backward complex to complex
    bcc = backward | analytic
  };

  //! Returns the normalization convention of the currently selected library
  normalization get_norm ();

  //! Returns the scale factor associated with the FFT operation
  double get_scale (size_t nfft, type t);

  void shift (unsigned npts, float* arr, double shift);

  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_frc1d(size_t nfft, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_fcc1d(size_t nfft, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_bcc1d(size_t nfft, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_bcr1d(size_t nfft, float* srcdest);


  /* ////////////////////////////////////////////////////////////////////
     
  Two-dimensional FFT library interface
  
  //////////////////////////////////////////////////////////////////// */

  //! Forward complex-to-complex FFT
  void fcc2d (size_t nx, size_t ny, float* into, const float* from);

  //! Backward complex-to-complex FFT
  void bcc2d (size_t nx, size_t ny, float* into, const float* from);

}

#endif

