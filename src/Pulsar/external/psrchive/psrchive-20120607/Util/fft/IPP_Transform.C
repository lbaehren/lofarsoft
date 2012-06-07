/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*

See threads related documentation at:
http://support.intel.com/support/performancetools/libraries/ipp/sb/cs-010662.htm

*/

#include "IPP_Transform.h"
#include "Error.h"

#include <ippcore.h>

using namespace std;

FTransform::IPP::Plan::~Plan()
{
  if( Spec )
  {
    if( call & real )
      ippsFFTFree_R_32f( (IppsFFTSpec_R_32f*)Spec );
    else
      ippsFFTFree_C_32fc( (IppsFFTSpec_C_32fc*)Spec );
  }

  if (threads)
    delete threads;
#ifdef _DEBUG
  cerr << "IPP::Plan destroyed" << endl;
#endif
}


FTransform::IPP::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::IPP::Plan nfft=" << n_fft
       << " call='" << t << "'" << endl;
#endif

  ippSetNumThreads (1);

  int order = 0;
  int pSize = 0;

  {
    unsigned doubling = 1;
    while( doubling < n_fft ){
      order++;
      doubling *= 2;
    }
    if( doubling != n_fft )
      throw Error (InvalidState, "FTransform::IPP::Plan",
		   "nfft=%d is not a power of 2", n_fft);
  }

  IppStatus ret;

  if( t & real ) 
  {
    ret = ippsFFTInitAlloc_R_32f( (IppsFFTSpec_R_32f**)&Spec, order,
					    IPP_FFT_NODIV_BY_ANY, ippAlgHintFast );
    if (ret != ippStsNoErr)
      throw Error (InvalidState, "FTransform::IPP::Plan",
       "ippsFFTInitAlloc_R_32f failed with %s", ippGetStatusString(ret) );

    ret = ippsFFTGetBufSize_R_32f( (IppsFFTSpec_R_32f*)Spec, &pSize );
    if (ret != ippStsNoErr)
      throw Error (InvalidState, "FTransform::IPP::Plan",
       "ippsFFTGetBufSize_R_32f failed with  %s", ippGetStatusString(ret) );
 
  }
  else 
  {
    ret = ippsFFTInitAlloc_C_32fc( (IppsFFTSpec_C_32fc**)&Spec, order,
			        IPP_FFT_NODIV_BY_ANY, ippAlgHintFast );
    if (ret != ippStsNoErr)
      throw Error (InvalidState, "FTransform::IPP::Plan",
       "ippsFFTInitAlloc_C_32fc failed with %s", ippGetStatusString(ret) );

    ret = ippsFFTGetBufSize_C_32fc( (IppsFFTSpec_C_32fc*)Spec, &pSize );
    if (ret != ippStsNoErr)
      throw Error (InvalidState, "FTransform::IPP::Plan",
       "ippsFFTGetBufSize_R_32fc failed with  %s", ippGetStatusString(ret) );
  }

  // normally > 0 for nfft > 64
  workBufferSize = (size_t) pSize;

#ifdef _DEBUG
  cerr << "FTransform::IPP::Plan order=" << order << " n_fft=" << n_fft
       << " workBufferSize=" << workBufferSize << endl;
#endif

  if (workBufferSize)
    threads = new ThreadIPPMemory;
  else
    threads = 0;

  nfft = n_fft;
  call = t;
  optimized = false;

}

Ipp8u * FTransform::IPP::Plan::get_buffer ()
{
#ifdef _DEBUG
  cerr << "FTransform::IPP::Plan::get_buffer" << endl;
#endif
  if (workBufferSize)
  {
    Ipp8u * buffer = 0;
    try
    {
      buffer =  threads->get();
    }
    catch (Error& error)
    {
#ifdef _DEBUG
      cerr << "FTransform::IPP::Plan::get_buffer ippsMalloc_8u(" << workBufferSize << ")" <<
endl;
#endif
      buffer = ippsMalloc_8u(workBufferSize);
      if (!buffer)
        throw Error (InvalidState, "FTransform::IPP::Plan::get_buffer",
                     "could not allocate memory");
      threads->set (buffer);
    }
#ifdef _DEBUG
    cerr << "FTransform::IPP::Plan::get_buffer buffer=" << &buffer << endl;
#endif
    return buffer;
  }
  else
    return NULL;
}

void FTransform::IPP::Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTFwd_RToCCS_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)Spec, get_buffer() );
}

void FTransform::IPP::Plan::fcc1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTFwd_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)Spec, get_buffer() );
}

void FTransform::IPP::Plan::bcc1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTInv_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)Spec, get_buffer() );
}

void FTransform::IPP::Plan::bcr1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTInv_CCSToR_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)Spec, get_buffer() );
}

