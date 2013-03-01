//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _IPP_Transform_h_
#define _IPP_Transform_h_

#include "FTransformAgent.h"
#include "ThreadIPPMemory.h"
#include <ipps.h>

namespace FTransform {

  class IPP {

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
    
      Ipp8u* get_buffer ();
      void* Spec;
      size_t workBufferSize;
      ThreadIPPMemory * threads;

    };

    //! Not implemented
    class Plan2 : public NotImplemented {
    public:    
      Plan2 (size_t nx, size_t ny, type t) : NotImplemented ("IPP") {}
    };
  
    class Agent : public PlanAgent<IPP> {
    public:
      Agent () : PlanAgent<IPP> ("IPP", unnormalized) { }
    };

  };

}

#endif

