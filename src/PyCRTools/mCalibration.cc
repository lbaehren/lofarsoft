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
#define HFPP_FUNC_NAME hCalibratePolarization
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
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter pol0, const CIter pol0_end,
    const CIter pol1, const CIter pol1_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
  std::complex<double> J[2][2]; // Jones matrix for element response
  std::complex<double> Jinv[2][2]; // Inverse Jones matrix used for beam correction in on sky polarization
  std::complex<double> result[2][2]; // Final matrix used for beam correction
  std::complex<double> det; // Determinant of Jones matrix (used for matrix inversion)
  double R[2][2]; // Rotation + Projection matrix
  double phi, theta;

  const double pi_2 = M_PI / 2;
  const double pi_4 = M_PI / 4;

  // Get lengths
  const int N = std::distance(frequencies, frequencies_end);

  // Sanity checks
  if (N != std::distance(pol0, pol0_end) || N != std::distance(pol1, pol1_end))
  {
    throw PyCR::ValueError("[hCalibratePolarization] input vectors have incompatible sizes.");
  }

  // Calculate angles
  phi = az;
  theta = pi_2 - el;

  // Calculate rotation matrix
  R[0][0] = cos(phi - pi_4) * cos(theta);
  R[0][1] = -1.0 * sin(phi - pi_4);
  R[1][0] = sin(phi - pi_4) * cos(theta);
  R[1][1] = cos(phi - pi_4);

  // Get iterators
  CIter pol0_it = pol0;
  CIter pol1_it = pol1;
  NIter frequencies_it = frequencies;

  for (int i=0; i<N; i++)
  {
    // Get element response
//    LOFAR::element_response_lba(*freq_it, az, el, J);

    // Invert the Jones matrix
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarization] Jones matrix is singular.");

    Jinv[0][0] = J[1][1] / det;
    Jinv[0][1] = -1.0 * J[0][1] / det;
    Jinv[1][0] = -1.0 * J[1][0] / det;
    Jinv[1][1] = J[0][0] / det;

    // Get the final multiplication matrix
    result[0][0] = R[0][0] * Jinv[0][0] + R[0][1] * Jinv[1][0];
    result[0][1] = R[0][0] * Jinv[0][1] + R[0][1] * Jinv[1][1];
    result[1][0] = R[1][0] * Jinv[0][0] + R[1][1] * Jinv[1][0];
    result[1][1] = R[1][0] * Jinv[0][1] + R[1][1] * Jinv[1][1];

    // Correct polarizations for antenna gain pattern
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

