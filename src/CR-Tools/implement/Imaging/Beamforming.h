/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef BEAMFORMING_H
#define BEAMFORMING_H

// Standard library header files
#include <string>

// casacore header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

// CR-Tools header files
#include <Math/MathAlgebra.h>
#include <Math/VectorConversion.h>
#include <Math/VectorNorms.h>
#include <Filters/StatisticsFilter.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \file Beamforming.h
    
    \ingroup CR_Imaging
    
    \brief A collection of functions for beamforming
    
    \author Andreas Horneffer
    \author Lars B&auml;hren

    \date 2007/06/16

    <h3>Synopsis</h3>

    <b>(a) cc-beam</b>
    
    The data from each unique pair of antennas is multiplied, the resulting
    values are averaged, and then the square root is taken while preserving
    the sign.
    
    \f[
      cc(\vec \rho)[t] = \, ^+_- \sqrt{\left| \frac{1}{N_{Pairs}}
      \sum^{N-1}_{i=1} \sum^{N}_{j>i} s_i(\vec\rho)[t] s_j(\vec\rho)[t] \right|}
    \f]
    
    where 
    \f[
    s_j(\vec\rho)[t]
    = \mathcal{F}^{-1} \left\{ \tilde s_j(\vec\rho)[k] \right\}
    = \mathcal{F}^{-1} \left\{ w_j(\vec\rho)[k] \cdot \tilde s_j[k]\right\}
    \f]
    is the time shifted field strength of the single antennas for a direction
    \f$\vec \rho \f$. \f$ N \f$ is the number of antennas, \f$t\f$ the time or pixel
    index and \f$N_{Pairs}\f$ the number of unique pairs of antennas.
    The negative sign is taken if the sum had a negative sign before taking the
    absolute values, and the positive sign otherwise.
  */  

  const double startDefault  = 1./8.;
  const double stopDefault   = 3./8.;
  const double nsigmaDefault = 1.;
  const int navgDefault      = 3;

  // === Geometrical delays ====================================================
  
  //! Calculate the light travel delay, [sec].
  double geometricalDelay (const casa::Vector<double> &direction,
			   const casa::Vector<double> &antPosition,
			   bool &nearField);
  
  //! Calculate the light travel delay, [sec], for a set of antennae.
  casa::Vector<double> geometricalDelay (const casa::Vector<double> &direction,
					 const casa::Matrix<double> &antPositions);
  
  // === Beams ==================================================================

  //_____________________________________________________________________________
  //                                                                  calc_ccbeam
  
  /*!
    \brief Computation of the 'cc-beam' from a set of beam-formed spectra
    
    \param data      -- A set of beamformed spectra, [channel,antenna].
    \param blocksize -- Blocksize in the time domain (required for the inverse
           Fourier transform).
    
    \return cc  -- The 'cc-beam' towards a direction \f$ \vec \rho \f$.
  */
  template <typename T, typename S>
    casa::Vector<T> calc_ccbeam (const Matrix<S>& data,
				 int const &blocksize)
    {
      casa::IPosition shape (data.shape());
      casa::Matrix<T> dataTime (blocksize,shape(1),static_cast<T>(0));
      
      casa::FFTServer<T,S> server(casa::IPosition(1,blocksize),
				  casa::FFTEnums::REALTOCOMPLEX);
      
      try {
	casa::Vector<T> tmp(blocksize);
	// inverse FFT on the input data
	for (int antenna(0); antenna<shape(1); antenna++) {
	  server.fft(tmp,data.column(antenna));
	  // .. and copy the result
	  dataTime.column(antenna) = tmp;
	}
      } catch (casa::AipsError x) {
	cerr << "[calc_ccbeam] " << x.getMesg() << endl;
      }
      
      return calc_ccbeam(dataTime);
    }
  
  //_____________________________________________________________________________
  //                                                                  calc_ccbeam
  
  /*!
    \brief Function implementing the computation of the 'cc-beam'
    
    \param data      -- Input data (in the time domain); [sample,antenna]
    
    \return cc  -- The 'cc-beam' towards a direction \f$ \vec \rho \f$.
  */
  template <typename T, typename S>
    Vector<T> calc_ccbeam (const Matrix<T>& data)
    {
      int ant1 (0);
      int ant2 (0);
      int nofPairs (0);
      IPosition shape (data.shape());
      Vector<T> dataAnt1 (shape(0));
      Vector<T> cc (shape(0),static_cast<T>(0));
      
      for (ant1=0; ant1<shape(1); ant1++) {
	dataAnt1 = data.column(ant1);
	for (ant2=ant1+1; ant2<shape(1); ant2++) {
	  cc += dataAnt1*data.column(ant2);
	  //
	  nofPairs += 1;
	}
      }
    
    // normalization by number of antenna pairs
    cc /= static_cast<T>(nofPairs);
    
    // we need to extract the sign at this point
    
    return CR::sign(cc)*sqrt(abs(cc));
  }
  
  //_____________________________________________________________________________
  //                                                                   calc_xbeam
  
  /*!
    \brief Function implementing the computation of the 'x-beam'

    \param data   -- A set of beamformed spectra, [channel,antenna].
    \param start  -- 
    \param stop   -- 
    \param nsigma -- 
    \param navg   -- 
  */
  template <typename T, typename S>
    casa::Vector<T> calc_xbeam (const Matrix<T> &data,
				double const &start=startDefault,
				double const &stop=stopDefault,
				double const &nsigma=nsigmaDefault,
				int const &navg=navgDefault)
    {
      // Vector returning the result of the beamforming
      casa::Vector<T> xbeamS;
      
      try {
	casa::IPosition shape (data.shape());
	casa::Vector<T> corr(shape(0),0.), tmpvec, pown, corrn;
	int i;
	int j;
	int nc              = 0;
	int np              = 1;
	int nofAntennas     = shape(1);
	casa::Vector<T> pow = square(data.column(nofAntennas-1));
	
	for (i=0; i<(nofAntennas-1); i++) {
	  tmpvec = data.column(i);
	  for (j=i+1; j<nofAntennas; j++) {
	    corr += tmpvec*data.column(j);
	    nc++;
	  };
	  pow += square(data.column(i));
	  np++;
	};
	pow /= (T)np;
	corr /= (T)nc;
	
	i = ifloor(start*shape(0));
	j = ifloor(stop*shape(0));
	Slice sli(i,(j-i));
	T mp;
	T sigma;
	T mc;
	mp = mean(pow(sli));
	sigma = stddev(pow(sli))*nsigma;
	mc = mean(corr(sli));

	StatisticsFilter<T> mf(navg,FilterType::MEAN);

	pown             = mf.filter(pow)-mp;
	corrn            = mf.filter(corr)-mc;
	pown(pown<sigma) = sigma; //implicit MaskedArray
	tmpvec           = abs(corrn/pown);
	xbeamS           = tmpvec*corr;
	
	return CR::sign(xbeamS)*sqrt(abs(xbeamS));
      } catch (casa::AipsError x) {
	cerr << "[calc_xbeam] " << x.getMesg() << endl;
	return Vector<T>();
      }    
    }
  
  //_____________________________________________________________________________
  //                                                                   calc_xbeam
  
  /*!
    \brief Computation of the 'x-beam' from a set of "time-shifted" spectra
    
    \param data      -- A set of beamformed spectra, [channel,antenna].
    \param blocksize -- Blocksize in the time domain (required for the inverse
           Fourier transform).
    
    \return cc  -- The 'x-beam' data.
  */
  template <typename T, typename S> 
    casa::Vector<T> xbeam (const casa::Matrix<S>& data,
			   int const &blocksize)
    {
      // get the shape of the input data
      casa::IPosition shape (data.shape());
      // matrix with intermediate data which will be forwarded for processing
      casa::Matrix<T> dataTime (blocksize,shape(1),static_cast<T>(0));
      // set up FFT server
      casa::FFTServer<T,S> server(casa::IPosition(1,blocksize),
				  casa::FFTEnums::REALTOCOMPLEX);
      try {
	casa::Vector<T> tmp(blocksize);
	// inverse FFT on the input data
	for (int antenna(0); antenna<shape(1); antenna++) {
	  server.fft(tmp,data.column(antenna));
	  // .. and copy the result
	  dataTime.column(antenna) = tmp;
	}
	return xbeam(dataTime);
      } catch (casa::AipsError x) {
	cerr << "xBeam:xbeam_from_FFT: " << x.getMesg() << endl;
	return casa::Vector<T>();
      }
    }
  
} // Namespace CR -- end

#endif /* BEAMFORMING_H */
