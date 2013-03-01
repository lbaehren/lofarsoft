//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/fft/median_smooth.h,v $
   $Revision: 1.7 $
   $Date: 2008/10/31 01:14:20 $
   $Author: straten $*/

#ifndef __fft_smooth_h
#define __fft_smooth_h

#include <algorithm>
#include <vector>
#include <iostream>

namespace fft {

  template <class T> 
  void median_smooth (std::vector<T>& data, unsigned ndim, unsigned wsize)
  {
    std::vector<typename T::type> temp (data.size());
    for (unsigned idim=0; idim<ndim; idim++)
    {
      for (unsigned idat=0; idat<data.size(); idat++)
	temp[idat] = data[idat][idim];
      median_smooth (temp, wsize);
      for (unsigned idat=0; idat<data.size(); idat++)
	data[idat][idim] = temp[idat];
    }
  }

  template <class T>
  void median_smooth (std::vector<T>& data, unsigned wsize)
  {
    if (data.size() < 4)
      return;

    if (wsize < 3)
      return;
    
    if (data.size() < wsize + 1)
      return;

    // make it an odd number
    if (wsize%2 == 0)
      wsize ++;

    unsigned ipt = 0, jpt = 0;

    unsigned middle = wsize/2;
    std::vector<T> window (wsize);

    unsigned rsize = data.size() - wsize + 1;
    std::vector<T> result (data.size());

    unsigned truncated = 0;
    unsigned tmid = 0;
    // deal with leading edge
    for (ipt=0; ipt < middle; ipt++)
    {
      truncated = middle + 1 + ipt;
      tmid = truncated / 2;
      for (jpt=0; jpt < truncated; jpt++)
	window[jpt] = data[jpt];
      std::nth_element (window.begin(), window.begin()+tmid, 
			window.begin()+truncated);
      result[ipt] = window[tmid];
    }

    for (ipt=0; ipt < rsize; ipt++)
    {
      for (jpt=0; jpt<wsize; jpt++)
	window[jpt] = data[ipt+jpt];
      std::nth_element (window.begin(), window.begin()+middle, window.end());
      result[ipt+middle] = window[middle];
    }

    // deal with trailing edge
    for (ipt=rsize; ipt < data.size()-middle; ipt++)
    {
      truncated = data.size() - ipt;
      tmid = truncated / 2;
      for (jpt=0; jpt < truncated; jpt++)
	window[jpt] = data[ipt+jpt];
      std::nth_element (window.begin(), window.begin()+tmid,
			window.begin()+truncated);
      result[ipt+middle] = window[tmid];
    }

    for (ipt=0; ipt < data.size(); ipt++)
      data[ipt] = result[ipt];
 
  } // end median_smooth

  template <class T>
  T absolute (T x) { if (x<0) return -x; return x; }

  template <class T>
  void median_filter (std::vector<T>& data, unsigned wsize, T threshold)
  {
    std::vector<T> copy (data);
    median_smooth (copy, wsize);

    const unsigned ndat = copy.size();

    T variance = 0;
    for (unsigned idat=0; idat < ndat; idat++)
    {
      copy[idat] -= data[idat];
      copy[idat] *= copy[idat];
      variance += copy[idat];
    }

    variance /= ndat;

    bool zapped = true;
    unsigned round = 1;
    unsigned total_zapped = 0;

    while (zapped)
    {
      T cutoff = threshold * threshold * variance;

      zapped = false;
      round ++;

      for (unsigned idat=0; idat < ndat; idat++)
      {
        if (copy[idat] > cutoff ||
            (idat && absolute(copy[idat]-copy[idat-1]) > 2*cutoff))
        {
          variance -= copy[idat]/ndat;
          copy[idat] = data[idat] = 0;
	  total_zapped ++;
          zapped = true; 
        }
      }
    }
  }

} // end namespace fft

#endif // ! _fft_smooth_h
