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
  if( pBuffer )
    delete [] pBuffer;
  if( Spec )
  {
    if( call & real )
      ippsFFTFree_R_32f( (IppsFFTSpec_R_32f*)Spec );
    else
      ippsFFTFree_C_32fc( (IppsFFTSpec_C_32fc*)Spec );
  }
  // cerr << "IPP::Plan destroyed" << endl;
}


FTransform::IPP::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::IPP::Plan nfft=" << n_fft
       << " call='" << fft_call << "'" << endl;
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

  if( t & real ) {
    IppStatus ret = ippsFFTInitAlloc_R_32f( (IppsFFTSpec_R_32f**)&Spec, order,
					    IPP_FFT_NODIV_BY_ANY,
					    ippAlgHintFast );
    ret = ippsFFTGetBufSize_R_32f( (IppsFFTSpec_R_32f*)Spec, &pSize );
  }
  else {
    ippsFFTInitAlloc_C_32fc( (IppsFFTSpec_C_32fc**)&Spec, order,
			     IPP_FFT_NODIV_BY_ANY, ippAlgHintFast );
    ippsFFTGetBufSize_C_32fc( (IppsFFTSpec_C_32fc*)Spec, &pSize );
  }

  if( n_fft == 131072 ) 
    // KLUDGE!
    pBuffer = new Ipp8u[2*pSize];
  else
    pBuffer = new Ipp8u[pSize];

  nfft = n_fft;
  call = t;
  optimized = false;

}

void FTransform::IPP::Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTFwd_RToCCS_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)Spec, pBuffer );
}

void FTransform::IPP::Plan::fcc1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTFwd_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)Spec, pBuffer );
}

void FTransform::IPP::Plan::bcc1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTInv_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)Spec, pBuffer );
}

void FTransform::IPP::Plan::bcr1d (size_t nfft, float* dest, const float* src)
{
  ippsFFTInv_CCSToR_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)Spec, pBuffer );
}

