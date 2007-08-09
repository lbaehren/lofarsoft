/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id$*/

#ifndef _CCBEAM_H_
#define _CCBEAM_H_

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>

using casa::AipsError;
using casa::IPosition;
using casa::Matrix;
using casa::Vector;

/*!
  \class ccBeam

  \ingroup Beamforming

  \brief Methods for the computation of the 'cc-Beam'

  \author Lars B&auml;hren

  \date 2005/12/09

  \test tccBeam.cc

  <h3>Prerequisite</h3>
  
  <ul type="square">
    <li>Beamformer -- An implementation for various beamforming methods.
  </ul>

  <h3>Synopsis</h3>
  
  The data from each unique pair of antennas is multiplied, the resulting
  values are averaged, and then the square root is taken while preserving
  the sign.
  
  \f[
    cc(\vec \rho)[t] = \, ^+_- \sqrt{\left| \frac{1}{N_{Pairs}} \sum^{N-1}_{i=1}
    \sum^{N}_{j>i} s_i(\vec\rho)[t] s_j(\vec\rho)[t] \right|}
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

  <h3>Example(s)</h3>

*/
template <class T, class S> class ccBeam {
  
  //! Size of a block of data, [samples]; required for backwards FFT
  int blocksize_p;
  
 public:

  // -------------------------------------------------------- Object construction

  /*!
    \brief Default constructor 
  */
  ccBeam ();

  /*!
    \brief Argumented constructor

    \param blocksize -- Size of a block of data, [samples]; required for
                        backwards FFT
  */
  ccBeam (const int &blocksize);

  // --------------------------------------------------------- Object destruction

  /*!
    \brief Destructor
  */
  ~ccBeam ();

  /*!
    \brief Compute the so-called 'cc-beam'

    \param data      -- Input data (in the time domain); [sample,antenna]

    \return cc  -- The 'cc-beam' towards a direction \f$ \vec \rho \f$.
  */
  Vector<T> ccbeam (const Matrix<T> &data);

  /*!
    \brief Computation of the 'cc-beam' from a set of beam-formed spectra

    \param data -- A set of beamformed spectra, [channel,antenna].
    \param blocksize -- Blocksize in the time domain (required for the inverse
                        Fourier transform).

    \return cc  -- The 'cc-beam' towards a direction \f$ \vec \rho \f$.
  */
  Vector<T> ccbeam (const Matrix<S>& data,
		    const int &blocksize);
  
};

#endif /* _CCBEAM_H_ */
