/***************************************************************************
 *   Copyright (C) 2005-2006                                               *
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

/* $Id: Phasing.h,v 1.6 2006/11/08 17:42:42 bahren Exp $ */

#ifndef PHASING_H
#define PHASING_H

// C++ Standard library
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <tasking/Glish.h>
#include <measures/Measures.h>
#include <measures/Measures/MEpoch.h>
#include <casa/Quanta.h>

#include <Math/MathAlgebra.h>
#include <Math/MathGeometry.h>
#include <Math/VectorConversion.h>

using std::cout;

using casa::DComplex;
using casa::IPosition;
using casa::Matrix;
using casa::QC;
using casa::Vector;

namespace LOPES {  // namespace LOPES -- begin

/*!
  \class Phasing

  \ingroup Beamforming

  \brief Computation of delays for beamforming

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>Transformation between various coordinate system (polar, spherical polar,
    cartesian,...)
    <li>Mathematical concepts of phased arrays.
  </ul>
  
  <h3>Basic definitions:</h3>
  <img src="../figures/beamforming/beamforming-geometry.png">
  The fundamental geometry underlying the beamforming process is shown in the
  figure above.
  <ol>
    <li>The cartesian coordinate system frame is locked to the phase center of
    the array, i.e. the coordinates of the (default) array phase center are
    \f$\vec O = (0,0,0)\f$.
    <li>\f$\vec x_i\f$ is the  three-dimensional position of the i-th array
    antenna.
    <li>\f$\vec \rho_i\f$ denotes the direction vector from the position of
    the i-th antenna towards the position of the source.
    <li>We define the baseline vector between a pair of antennae i and j as
    \f[ \vec B_{ij} = \vec x_{i} - \vec x_{j} \f]
    <li>The light travel time delay between two antennae i and j,
    \f$\tau_{ij}\f$, is defined such, that \f$\tau_{ij} < 0\f$ if the signal
    arrives at antenna i before arriving at antenna j, i.e. 
    \f[ \tau_{ij} = \frac{1}{c}\, \Bigl( |\vec \rho_i| - |\vec \rho_j| \Bigr) \f]
    </ol>

  <table cellpadding="3" border="0">
    <tr valign="top">
      <td>
      Expressing of the position vector \f$\vec \rho_j\f$ in terms of the baseline 
      vector,
      \f[ \vec \rho_j = \vec B_{ij} + \vec \rho_i \f]
      we can rewrite the equation for the geometric light time travel delay as
      \f[
      \tau_{ij}
      = \frac{1}{c} \Bigl( |\vec \rho_i - \vec B_{ij}| - |\vec \rho_i| \Bigr)
      = \frac{1}{c} \left( \sqrt{|\vec \rho_i|^2 + |\vec B_{ij}|^2 - 2\,
      \vec \rho_i \cdot \vec B_{ij}} - |\vec \rho_i| \right)
      \f]
      which in the limit \f$|\rho| \rightarrow \infty\f$ becomes
      \f[ \tau_{ij} \simeq \frac{1}{c} \vec \rho_i \cdot \vec B_{ij} \f]
      The resulting time delay -- for both cases -- is shown in the figure to
      the right.
      </td>
      <td>
        <img src="../figures/beamforming/geometrical-delay-cpp.png" border="0">
      </td>
    </tr>
  </table>

  For a more extensive discussion see the <i>LOPES / LOFAR ITS Data Processing
  Manual</i> (\ref documents).

  <h3>Relation to Glish implementation</h3>
  
  The computation of the phase is implemented in <tt>calcphasegrad</tt>
  (phasing.g):
  \verbatim
  phase:= 2*pi/degree*delay*(flowlim+[0:(fftlen-1)]*fsteps);
  \endverbatim
  This expands into
  \f[
    \phi_k = 2 \pi \left( \frac{\pi}{180} \right)^{-1} \cdot \tau_{\rm geom}
    \cdot \nu_k 
    \,, \quad \hbox{where} \qquad 
    \nu_k = \nu_{\rm min} + k \cdot \Delta \nu
    \,, \qquad k \in [0,N_{\rm FFT})
  \f]
  By this we see that in order to obtain the weighting factor for the
  beamforming, \f$ w(\vec \rho, \vec x, \nu) \f$, we have to apply
  \f[ w(\phi_k) = \exp \left( - i \cdot \phi_k \cdot \left( \frac{\pi}{180}
  \right) \right) = \exp \left( - 2 \pi i \cdot \tau_{\rm geom} \cdot \nu_k
  \right) \f]
  to the result of <tt>calcphasegrad</tt>.
*/
class Phasing {

 public:

  // === Phase gradients =======================================================
  
  /*!
    \brief Phase gradients for a single antenna; all directions & frequencies 
    
    Given a set of directions for the array beam, \f$ (\phi, \theta) \f$,
    compute the (complex) phase delay factor for all frequencies, \f$ \nu \f$,
    at a supplied antenna position, \f$ \vec x_i \f$.
    
    \param frequencies      - Vector of frequency values, [Hz].
    \param directions       - Matrix of directions of the beam,
                              \f$ (\phi, \theta) \f$.
    \param antennaPosition  - Vector with the 3-dim antenna position.

    \return gradients - Array with the phase gradients, [nofFreq,nofDirections]
  */
  static Matrix<DComplex> phaseGradient (const Vector<double>&,
					 const Matrix<double>&,
					 const Vector<double>&);
  
  /*!
    \brief Phase gradients for a single direction; all frequencies & antennae.
    
    Given a direction for the array beam, \f$ (\phi, \theta) \f$, compute the
    (complex) phase delay factor for each supplied antenna position,
    \f$ \vec x_i \f$, and frequency, \f$ \nu \f$.
    
    \param frequencies      - Frequency values, [Hz].
    \param direction        - Direction of the beam, \f$ (\phi, \theta) \f$.
    \param antennaPositions - 3dim antenna positions, \f$ [N_{\rm Ant} \times 3] \f$

    \return gradients - Array with the phase gradients, [nofFreq,nofAntennae]
  */
  static Matrix<DComplex> phaseGradient (const Vector<double>&,
					 const Vector<double>&,
					 const Matrix<double>&);
  
  /*!
    \brief Phase gradients for a single frequency & direction; all antennae.

    Given a direction for the array beam, \f$ (\phi, \theta) \f$, compute the
    (complex) phase delay factor for each supplied antenna position.

    \param frequency        - Frequency, [Hz].
    \param direction        - Direction of the beam, \f$ (\phi, \theta) \f$.
    \param antennaPositions - 3dim antenna positions, \f$ [N_{\rm Ant} \times 3] \f$

    \return gradients - Vector with the phase gradients, [nofAntennae]
  */
  static Vector<DComplex> phaseGradient (const double,
					 const Vector<double>&,
					 const Matrix<double>&);
  
  /*!
    \brief Phase gradients for a single frequency, direction and antenna.
    
    Compute the complex phase shift factor used for beamforming in the fourier
    domain,
    \f[ w(\vec \rho, \vec x, \nu) = \exp (i \phi ) = \exp \Bigl( 2 \pi i\, \nu\,
    \tau_{\rm geom} (\vec \rho, \vec x) \Bigr) \f]

    \param frequency        - Frequency, [Hz].
    \param direction        - Direction of the beam, \f$ (\phi, \theta) \f$.
    \param antennaPosition  - 3dim antenna positions, \f$ [N_{\rm Ant} \times 3] \f$

    \return gradient - Phase gradient \f$ w(\vec \rho, \vec x, \nu) \f$.
  */
  static DComplex phaseGradient (const double frequency,
				 const Vector<double> &direction,
				 const Vector<double> &antennaPosition);
  
  // === Geometrical delays ====================================================
  
  /*!
    \brief Calculate the light travel delay, [sec].

    Calculate the light travel delay, \f$ \tau \f$, for a source at position
    \f$ \vec \rho \f$ and an antenna at position \f$ \vec x \f$ away from the
    phase center; this method assumes delay computation for the far-field
    regime.
    
    \param direction       - Direction of the beam.
    \param antennaPosition - 3dim antenna position.
  */
  static double geometricalDelay (const Vector<double> &direction,
				  const Vector<double> &antennaPosition);

  /*!
    \brief Calculate the light travel delay, [sec], for a set of antennae.

    \param direction        - Direction of the beam.
    \param antennaPositions - 3dim antenna positions.
  */
  static Vector<double> geometricalDelay (const Vector<double> &direction,
					  const Matrix<double> &antennaPosition);
  
  /*!
    \brief Calculate the light travel delay, [sec].
    
    Calculate the light travel delay, \f$ \tau \f$, for a source at position
    \f$ \vec \rho \f$ and an antenna at position \f$ \vec x \f$ away from the
    phase center.

    \param azel            - Source position for which the geometric delay is 
                             computed.
    \param antennaPosition - 3D antenna position.
    \param nearField       - Use delay formula for a source in the near field?
  */
  static double geometricalDelay (const Vector<double> &azel,
				  const Vector<double> &antennaPosition,
				  bool nearField);
 private:

  static double PI_p;
  
};

}  // namespace LOPES -- end

#endif
