//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _Utils_fft_FFTW3_Transform_h_
#define _Utils_fft_FFTW3_Transform_h_

#include "FTransformAgent.h"

namespace FTransform {

  class FFTW3 {

  public:

    class Plan : public FTransform::Plan {

    public:

      //! Constructor
      Plan (size_t nfft, type t);
      
      //! Destructor
      ~Plan ();
      
      void fcc1d (size_t nfft, float* dest, const float* src);
      void bcc1d (size_t nfft, float* dest, const float* src);
      void frc1d (size_t nfft, float* dest, const float* src);
      void bcr1d (size_t nfft, float* dest, const float* src);
      
    protected:
      
      void* plan;
      
    };


    class Plan2 : public FTransform::Plan2 {

    public:

      //! Constructor
      Plan2 (size_t nx, size_t ny, type t);

      //! Destructor
      ~Plan2 ();

      void fcc2d (size_t nx, size_t ny, float* , const float*);
      void bcc2d (size_t nx, size_t ny, float* , const float*);

    protected:

      void* plan;

    };

    //! Agent class
    class Agent : public PlanAgent<FFTW3> {
    public:
      Agent () : PlanAgent<FFTW3> ("FFTW3", unnormalized) { }
    };

  };

}

#endif
