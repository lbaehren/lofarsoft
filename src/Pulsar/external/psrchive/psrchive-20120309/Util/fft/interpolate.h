//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/fft/interpolate.h,v $
   $Revision: 1.15 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $*/

#ifndef __fft_interpolate_h
#define __fft_interpolate_h

#include "Traits.h"
#include "Error.h"
#include "FTransform.h"

#include <vector>

namespace fft {

  template <class T>
  void interpolate (std::vector<T>& out, const std::vector<T>& in) {
    
    if (in.size() >= out.size())
      throw Error (InvalidParam, "fft::interpolate",
		   "in.size=%d >= out.size=%d.  just scrunch",
		   in.size() >= out.size());
    
    std::vector< std::complex<float> > dom1 (out.size());
    std::vector< std::complex<float> > dom2 (out.size());
    
    DatumTraits<T> datum_traits;

    unsigned ndim = datum_traits.ndim();
    
    unsigned ipt;

    // for each dimension of the type T, perform an interpolation
    for (unsigned idim=0; idim<ndim; idim++) {

      for (ipt=0; ipt < in.size(); ipt++)
	dom1[ipt] = datum_traits.element (in[ipt], idim);

      FTransform::fcc1d (in.size(), (float*)&(dom2[0]), (float*)&(dom1[0]));

      // shift the negative frequencies
      unsigned npt2 = in.size()/2;
      for (ipt=0; ipt<npt2; ipt++)
	dom2[out.size()-1-ipt] = dom2[in.size()-1-ipt];
      
      // zero pad the rest
      unsigned end = out.size() - npt2;
      for (ipt=npt2; ipt<end; ipt++)
	dom2[ipt] = 0;

      FTransform::bcc1d (out.size(), (float*)&(dom1[0]), (float*)&(dom2[0]));

      float scalefac = 1.0;

      if (FTransform::get_norm() == FTransform::unnormalized)
        scalefac = 1.0 / float(in.size());
      else
        scalefac = float(out.size()) / float(in.size());

      for (ipt=0; ipt < out.size(); ipt++)
	datum_traits.element (out[ipt], idim) = 
	  datum_traits.element_traits.from_complex (dom1[ipt]*scalefac);

    } // end for each dimension

  } // end of interpolate

} // end of namespace fft

#endif // ! __fft_interpolate_h
