/**************************************************************************
 *  Calibration module for the CR Pipeline Python bindings.               *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

// ========================================================================
//
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mCalibration.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "legacy.h"
#include "mMath.h"
#include "mCalibration.h"
#include "ElementResponse.h"

#include <tmf.h>


// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;
using namespace casa;

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: Calibration functions
// ========================================================================

//$DOCSTRING: Calibrate polarization
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalibratePolarizationLBA
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(pol0)()("Polarization 0.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(pol1)()("Polarization 1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Frequencies in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Given the signals of both LOFAR dipoles in frequency space, the corresponding frequencies and a direction
  (in azimuth - elevation with respect to the antenna frame) this routine corrects for the antenna response (beam pattern).

  For each frequency the Jones matrix containing the antenna response is calculated using the LOFAR provided ElementResponse
  library.
  This matrix contains the beam pattern and multiplying by the inverse of this matrix gives the polarized signal in the on-sky
  frame.
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter pol0, const CIter pol0_end,
    const CIter pol1, const CIter pol1_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
  std::complex<double> J[2][2]; // Jones matrix for element response
  std::complex<double> Jinv[2][2]; // Inverse Jones matrix used for beam correction in on sky polarization
  std::complex<double> det; // Determinant of Jones matrix (used for matrix inversion)

  // Get lengths
  const int N = std::distance(frequencies, frequencies_end);

  // Sanity checks
  if (N != std::distance(pol0, pol0_end) || N != std::distance(pol1, pol1_end))
  {
    throw PyCR::ValueError("[hCalibratePolarization] input vectors have incompatible sizes.");
  }

  // Get iterators
  CIter pol0_it = pol0;
  CIter pol1_it = pol1;
  NIter freq_it = frequencies;

  for (int i=0; i<N; i++)
  {
    // Get element response
    LOFAR::element_response_lba(*freq_it, az, el, J);

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction)
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarization] Jones matrix is singular.");

    Jinv[0][0] = J[1][1] / det;
    Jinv[0][1] = -1.0 * J[0][1] / det;
    Jinv[1][0] = -1.0 * J[1][0] / det;
    Jinv[1][1] = J[0][0] / det;

    // Correct polarizations for antenna gain pattern
    *pol0_it = Jinv[0][0] * *pol0_it + Jinv[0][1] * *pol1_it;
    *pol1_it = Jinv[1][0] * *pol0_it + Jinv[1][1] * *pol1_it;

    ++pol0_it;
    ++pol1_it;
    ++freq_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calibrate polarization and obtain x,y,z components for Cosmic Rays application
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalibratePolarizationLBA
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(polx)()("Polarization X.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(poly)()("Polarization Y.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(polz)()("Polarization Z.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(frequencies)()("Frequencies in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Given the signals of both LOFAR dipoles in frequency space, the corresponding frequencies and a direction
  (in azimuth - elevation with respect to the antenna frame) this routine corrects for the antenna response (beam pattern).

  For each frequency the Jones matrix containing the antenna response is calculated using the LOFAR provided ElementResponse
  library.
  This matrix contains the beam pattern and multiplying by the inverse of this matrix gives the polarized signal in the on-sky
  frame.

  The corrected signal is then projected onto the X (N-S), Y (E-W), Z (zenith) frame as needed for the Cosmic Rays application.
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter polx, const CIter polx_end,
    const CIter poly, const CIter poly_end,
    const CIter polz, const CIter polz_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
  std::complex<double> J[2][2]; // Jones matrix for element response
  std::complex<double> Jinv[2][2]; // Inverse Jones matrix used for beam correction in on sky polarization
  std::complex<double> det; // Determinant of Jones matrix (used for matrix inversion)
  std::complex<double> p_theta, p_phi; // Polarization in on-sky theta phi frame

  // Direction in spherical coordinates with +x direction along N-S
  const double theta = (M_PI / 2) - el;
  const double phi = az - (M_PI / 4);

  // Get lengths
  const int N = std::distance(frequencies, frequencies_end);

  // Sanity checks
  if (N != std::distance(polx, polx_end) || N != std::distance(poly, poly_end) || N != std::distance(polz, polz_end))
  {
    throw PyCR::ValueError("[hCalibratePolarization] input vectors have incompatible sizes.");
  }

  // Get iterators
  CIter polx_it = polx;
  CIter poly_it = poly;
  CIter polz_it = polz;
  NIter freq_it = frequencies;

  for (int i=0; i<N; i++)
  {
    // Get element response
    LOFAR::element_response_lba(*freq_it, az, el, J);

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction)
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarization] Jones matrix is singular.");

    Jinv[0][0] = J[1][1] / det;
    Jinv[0][1] = -1.0 * J[0][1] / det;
    Jinv[1][0] = -1.0 * J[1][0] / det;
    Jinv[1][1] = J[0][0] / det;

    // Correct polarizations for antenna gain pattern
    p_theta = Jinv[0][0] * *polx_it + Jinv[0][1] * *poly_it;
    p_phi = Jinv[1][0] * *polx_it + Jinv[1][1] * *poly_it;

    // Project onto new coordinate frame
    *polx_it = cos(theta) * cos(phi) * p_theta - sin(phi) * p_phi;
    *poly_it = cos(theta) * sin(phi) * p_theta + cos(phi) * p_phi;
    *polz_it = sin(theta) * p_theta;

    ++polx_it;
    ++poly_it;
    ++freq_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

