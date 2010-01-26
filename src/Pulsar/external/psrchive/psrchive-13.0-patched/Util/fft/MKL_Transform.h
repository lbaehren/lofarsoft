//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _Utils_fft_MKL_Transform_h_
#define _Utils_fft_MKL_Transform_h_

#include "FTransformAgent.h"

namespace FTransform {

  class MKL {

  public:

    class Plan : public FTransform::Plan {

    public:

      Plan (size_t nfft, type t);
      ~Plan ();

      void fcc1d (size_t nfft, float* dest, const float* src);
      void bcc1d (size_t nfft, float* dest, const float* src);
      void frc1d (size_t nfft, float* dest, const float* src);
      void bcr1d (size_t nfft, float* dest, const float* src);

    protected:

      float* mkl_plan;

    };

    //! Not implemented
    class Plan2 : public NotImplemented {
    public:
      Plan2 (size_t nx, size_t ny, type t) : NotImplemented ("MKL") {}
    };

    class Agent : public PlanAgent<MKL> {
    public:
      Agent () : PlanAgent<MKL> ("MKL", normalized) { }
    };

  };

}

#endif

