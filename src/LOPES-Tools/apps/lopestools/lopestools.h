
#ifndef _LOPESTOOLS_H_
#define _LOPESTOOLS_H_

/* $Id: lopestools.h,v 1.2 2005/12/21 14:13:06 bahren Exp $ */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Quanta/QMath.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
#include <tasking/Glish.h>

#include <casa/namespace.h>

/*!
  \file lopestools.h

  \brief Various functions to use with the LOPES-glish software

  [still needs to be written]
*/

// =============================================================================
//
//  Computation routines
//
// =============================================================================

/*!
  \fn Matrix<Complex> fft(int, int, float *)

  \brief Compute the Fourier transform an array of timeline vectors.

  \param numdata - Number of antennas
  \param tsize   - Length of timelines
  \param files   - Array with data (timeline vectors)
 */
Matrix<Complex> fft (int, int, float *);

/*!
  \fn float *calc_cal_vals(int, int, float *, float *, int)

  \brief Compute factors for weighing the frequencies.

  \param numdata  - Number of antennas
  \param tsize    - Length of a Fourier data vector
  \param calfreq  - Array with frequency-data, mounting from 0 to 1
  \param calvalue - Array with gainfactors
  \param calsize  - Length of arrays
 */
float *calc_cal_vals(int, int, float *, float *, int);

/*!
  \fn Matrix<Complex> fftgain(int, int, float *, Matrix<Complex>)

  \brief Application of gainfactors, overwrites input fftarray

  \param numdata  - Number of antennas
  \param fsize    - Length of Fourier data vector
  \param erg      - 
  \param fftarray - Array with the frequency domain data
*/
Matrix<Complex> fftgain(int, int, float *, Matrix<Complex>);

/*!
  \brief Choice of filter and its strength, computes mask

  \param numdata  - Number of antennae
  \param fsize    - Number of frequency channels
  \param fftarray - Array with the frequency domain data,
                    \f$ [N_{\rm Ant}, N_{\rm Freq}] \f$
  \param f        - Number of times the filter is to be applied
  \param k        - Filter strenght
  \param sel
 */
int *gen_mask(int numdata,
	      int fsize,
	      Matrix<Complex> fftarray,
	      char f,
	      char k,
	      char sel);

/*!
  \brief Compute mean power by averaging over all blocks
  
  Computes meanpower by averaging over all blocks; taking datasets, applying fft
  
  \param bsize - size of blocks
  \param files - array with filenames 
  \param nch   - number of filenames
  \param w     - index for choice of windows
  \return erg - array with data
*/
float *do_calc_meanpower (int nch,
			  char **files,
			  int bsize,
			  char w);

/*!
  \brief Calculate the values on one pixel of a map directly from the files
  
  \param nch    - number of (antenna) channels
  \param files  - array of filesnames
  \param delays - array with the delays for this pixel;
  \param bsize  - size of one block;
  \param nfreq  - number of frequency channels

  \return erg - Array with the data
*/
float *do_calc_pixel (int nch,
		      char **files,
		      int *delays,
		      int bsize,
		      int nfreq);

/*!
  \brief frequency-filter for all antennas

  Applies fft and filters the spectrum by cutting out the peaks, inverse fft
  (real to complex to real)
  
  \param files   - array with data (timeline vectors)
  \param numdata - number of antennas
  \param tsize   - length of timelines
  \param sel     - choice of meanfilter (sel=1) or normfilter (sel=0)
  \param k       - strength of filter (k-times standard-deviation)
  \param f       - how often filter is applied
  \return files - array with filtered data, overwrites input array
*/
float *do_fftfilter (int numdata,
		     int tsize,
		     float *files,
		     int f,
		     int k,
		     int sel);

// =============================================================================
//
//  Glish interfacing routines
//
// =============================================================================

/*!
  \brief Apply a meanvalue filter to the data
  
  Event Callback Function

  \todo Actual computation to be implemented in separate class (MeanFilter)
  
  - ret-data: GlishArray with the data
  - inp-data: filtrec =    Record with the data
  - filtrec.ary      = data to be filtered
  - filtrec.strength = strength of the filter (default =3)
*/
Bool meanfilter (GlishSysEvent &event, 
		 void *);


#endif
