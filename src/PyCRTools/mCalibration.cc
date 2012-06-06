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

#define SMALL_OFFSET 1.e-9

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
    throw PyCR::ValueError("[hCalibratePolarizationLBA] input vectors have incompatible sizes.");
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

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationLBA] Jones matrix is singular.");

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
    throw PyCR::ValueError("[hCalibratePolarizationLBA] input vectors have incompatible sizes.");
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

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationLBA] Jones matrix is singular.");

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

//$DOCSTRING: Calibrate polarization
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalibratePolarizationHBA
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
    throw PyCR::ValueError("[hCalibratePolarizationHBA] input vectors have incompatible sizes.");
  }

  // Get iterators
  CIter pol0_it = pol0;
  CIter pol1_it = pol1;
  NIter freq_it = frequencies;

  for (int i=0; i<N; i++)
  {
    // Get element response
    LOFAR::element_response_hba(*freq_it, az, el, J);

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction)
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationHBA] Jones matrix is singular.");

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
#define HFPP_FUNC_NAME hCalibratePolarizationHBA
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
    throw PyCR::ValueError("[hCalibratePolarizationHBA] input vectors have incompatible sizes.");
  }

  // Get iterators
  CIter polx_it = polx;
  CIter poly_it = poly;
  CIter polz_it = polz;
  NIter freq_it = frequencies;

  for (int i=0; i<N; i++)
  {
    // Get element response
    LOFAR::element_response_hba(*freq_it, az, el, J);

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction)
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationHBA] Jones matrix is singular.");

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

//$DOCSTRING: Get inverse Jones matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetInverseJonesMatrixLBA
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Jinv)()("Inverse Jones matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Frequency in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Calculates the inverse Jones matrix containing the antenna response calculated using the LOFAR provided ElementResponse library.
  The Jones matrix contains the beam pattern and multiplying by inverse of this matrix gives the polarized signal in the on-sky frame.
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter Jinv, const CIter Jinv_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
  std::complex<double> J[2][2]; // Jones matrix for element response
  std::complex<double> det; // Determinant of Jones matrix (used for matrix inversion)

  // Get lengths
  const int Nj = std::distance(Jinv, Jinv_end);
  const int Nf = std::distance(frequencies, frequencies_end);

  // Sanity checks
  if (Nj != 4*Nf)
  {
    throw PyCR::ValueError("[hGetInverseJonesMatrixLBA] Provided storrage has invalid size.");
  }

  // Get iterators
  CIter Jinv_it = Jinv;
  NIter freq_it = frequencies;

  // Loop over frequencies
  for (int i=0; i<Nf; i++)
  {
    // Get element response
    LOFAR::element_response_lba(*freq_it++, az, el, J);

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction)
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationLBA] Jones matrix is singular.");

    *Jinv_it++ = J[1][1] / det;
    *Jinv_it++ = -1.0 * J[0][1] / det;
    *Jinv_it++ = -1.0 * J[1][0] / det;
    *Jinv_it++ = J[0][0] / det;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Get inverse Jones matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetNormalizedInverseJonesMatrixLBA
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Jinv)()("Inverse Jones matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Frequency in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Calculates the inverse Jones matrix containing the antenna response calculated using the LOFAR provided ElementResponse library.
  The Jones matrix contains the beam pattern and multiplying by inverse of this matrix gives the polarized signal in the on-sky frame.
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter Jinv, const CIter Jinv_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
  std::complex<double> J[2][2]; // Jones matrix for element response
  std::complex<double> N[2][2]; // Normalization Jones matrix for element response
  std::complex<double> det; // Determinant of Jones matrix (used for matrix inversion)

  // Get lengths
  const int Nj = std::distance(Jinv, Jinv_end);
  const int Nf = std::distance(frequencies, frequencies_end);

  // Sanity checks
  if (Nj != 4*Nf)
  {
    throw PyCR::ValueError("[hGetInverseJonesMatrixLBA] Provided storrage has invalid size.");
  }

  // Get iterators
  CIter Jinv_it = Jinv;
  NIter freq_it = frequencies;

  // Loop over frequencies
  for (int i=0; i<Nf; i++)
  {
    // Get element response
    LOFAR::element_response_lba(*freq_it, az, el, J);

    // Get element response for normalization factor
    LOFAR::element_response_lba(*freq_it, 0.0, M_PI / 2 - SMALL_OFFSET, N);

    freq_it++;

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction) and normalize
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]) * (N[0][0] * N[1][1]) - (N[0][1] * N[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationLBA] Normalization Jones matrix is singular.");

    *Jinv_it++ = J[1][1] / det;
    *Jinv_it++ = -1.0 * J[0][1] / det;
    *Jinv_it++ = -1.0 * J[1][0] / det;
    *Jinv_it++ = J[0][0] / det;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Get inverse Jones matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetInverseJonesMatrixHBA
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Jinv)()("Inverse Jones matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Frequency in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Calculates the inverse Jones matrix containing the antenna response calculated using the LOFAR provided ElementResponse library.
  The Jones matrix contains the beam pattern and multiplying by inverse of this matrix gives the polarized signal in the on-sky frame.
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter Jinv, const CIter Jinv_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
  std::complex<double> J[2][2]; // Jones matrix for element response
  std::complex<double> det; // Determinant of Jones matrix (used for matrix inversion)

  // Get lengths
  const int Nj = std::distance(Jinv, Jinv_end);
  const int Nf = std::distance(frequencies, frequencies_end);

  // Sanity checks
  if (Nj != 4*Nf)
  {
    throw PyCR::ValueError("[hGetInverseJonesMatrixHBA] Provided storrage has invalid size.");
  }

  // Get iterators
  CIter Jinv_it = Jinv;
  NIter freq_it = frequencies;

  // Loop over frequencies
  for (int i=0; i<Nf; i++)
  {
    // Get element response
    LOFAR::element_response_lba(*freq_it++, az, el, J);

    // Invert the Jones matrix (J contains beam pattern so J^-1 is the correction)
    det = (J[0][0] * J[1][1]) - (J[0][1] * J[1][0]);

    if (det == std::complex<double>(0,0)) throw PyCR::ValueError("[hCalibratePolarizationHBA] Jones matrix is singular.");

    *Jinv_it++ = J[1][1] / det;
    *Jinv_it++ = -1.0 * J[0][1] / det;
    *Jinv_it++ = -1.0 * J[1][0] / det;
    *Jinv_it++ = J[0][0] / det;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Get unity inverse Jones matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetUnityInverseJonesMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Jinv)()("Inverse Jones matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Returns a unity inverse Jones matrix for testing.
*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter Jinv, const CIter Jinv_end)
{
  // Get lengths
  const int Nj = std::distance(Jinv, Jinv_end);
  const int Nf = Nj / 4;

  // Get iterators
  CIter Jinv_it = Jinv;

  // Loop over frequencies
  for (int i=0; i<Nf; i++)
  {
    *Jinv_it++ = 1.0;
    *Jinv_it++ = 0.0;
    *Jinv_it++ = 0.0;
    *Jinv_it++ = 1.0;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Apply inverse Jones matrix to get on sky polarizations
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetOnSkyPolarizations
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(pol0)()("Polarization 0.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(pol1)()("Polarization 1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(Jinv)()("Inverse Jones matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter pol0, const CIter pol0_end,
    const CIter pol1, const CIter pol1_end,
    const CIter Jinv, const CIter Jinv_end)
{
  // Temporary variable
  complex<double> temp[2];

  // Get lengths
  const int Nj = std::distance(Jinv, Jinv_end);
  const int N = std::distance(pol0, pol0_end);

  // Sanity checks
  if ((N != std::distance(pol1, pol1_end)) || (Nj != 4*N))
  {
    throw PyCR::ValueError("[hMatrixMultiply2D] input vectors have incompatible sizes.");
  }

  // Get iterators
  CIter pol0_it = pol0;
  CIter pol1_it = pol1;
  CIter Jinv_it = Jinv;

  for (int i=0; i<N; i++)
  {
    temp[0]  = *Jinv_it++ * *pol0_it;
    temp[0] += *Jinv_it++ * *pol1_it;
    temp[1]  = *Jinv_it++ * *pol0_it;
    temp[1] += *Jinv_it++ * *pol1_it;

    *pol0_it++ = temp[0];
    *pol1_it++ = temp[1];
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Project on-sky polarizations onto x,y,z.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hProjectPolarizations
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(polx)()("Polarization X.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(poly)()("Polarization Y.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(polz)()("Polarization Z.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(pol0)()("Polarization 0.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(pol1)()("Polarization 1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter polx, const NIter polx_end,
    const NIter poly, const NIter poly_end,
    const NIter polz, const NIter polz_end,
    const NIter pol0, const NIter pol0_end,
    const NIter pol1, const NIter pol1_end,
    const HNumber az, const HNumber el)
{
  // Get length of output vector
  const int N = std::distance(pol0, pol0_end);

  // Sanity checks
  if (N != std::distance(pol1, pol1_end) || N != std::distance(polx, polx_end) || N != std::distance(poly, poly_end) || N != std::distance(polz, polz_end))
  {
    throw PyCR::ValueError("[hProjectPolarizations] input vectors have incompatible sizes.");
  }

  // Direction in spherical coordinates with +x direction along N-S
  const double theta = (M_PI / 2) - el;
  const double phi = az - (M_PI / 4);

  // Get iterators
  NIter pol0_it = pol0;
  NIter pol1_it = pol1;
  NIter polx_it = polx;
  NIter poly_it = poly;
  NIter polz_it = polz;

  for (int i=0; i<N; i++)
  {
    // Project onto new coordinate frame
    *polx_it = cos(theta) * cos(phi) * *pol0_it - sin(phi) * *pol1_it;
    *poly_it = cos(theta) * sin(phi) * *pol0_it + cos(phi) * *pol1_it;
    *polz_it = -1.0 * sin(theta) * *pol0_it;

    ++pol0_it;
    ++pol1_it;
    ++polx_it;
    ++poly_it;
    ++polz_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate Stokes parameters I, Q, U and V
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStokesParameters
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(S)()("Stokes parameters I, Q, U and V.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(polx)()("Polarization X.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(poly)()("Polarization Y.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(polz)()("Polarization Z.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(polxh)()("Hilbert transform of polarization X.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(polyh)()("Hilbert transform of polarization Y.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HNumber)(polzh)()("Hilbert transform of polarization Z.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  The definitions of the Stokes parameters in the horizontal plane are,

  \begin{align}
  S_0 &= I = \frac{1}{n} \sum_{i=1}^{n} x_i^2 + \hat{x}_i^2 + y_i^2 + \hat{y}_i^2\\
  S_1 &= Q = \frac{1}{n} \sum_{i=1}^{n} x_i^2 + \hat{x}_i^2 - y_i^2 - \hat{y}_i^2\\
  S_2 &= U = \frac{2}{n} \sum_{i=1}^{n} x_i y_i + \hat{x}_i \hat{y}_i\\
  S_3 &= V = \frac{2}{n} \sum_{i=1}^{n} \hat{x}_i y_i - x_i \hat{y}_i
  \end{align}

  in which $x_i$ gives the time-dependent electric field in the east-west direction, while $y_i$ corresponds to the north-south direction. The $\hat{x}_i$ and $\hat{y}_i are the complex propagation of the time-series obtained by a Hilbert transformation.
*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter S, const NIter S_end,
    const NIter polx, const NIter polx_end,
    const NIter poly, const NIter poly_end,
    const NIter polz, const NIter polz_end,
    const NIter polxh, const NIter polxh_end,
    const NIter polyh, const NIter polyh_end,
    const NIter polzh, const NIter polzh_end)
{
  double temp = 0;

  // Get length of output vector
  const int N = std::distance(polx, polx_end);

  // Sanity checks
  if (std::distance(S, S_end) != 4 || N != std::distance(poly, poly_end) || N != std::distance(polz, polz_end) ||
    N != std::distance(polxh, polxh_end) || N != std::distance(polyh, polyh_end) || N != std::distance(polzh, polzh_end))
  {
    throw PyCR::ValueError("[hStokesParameters] input vectors have incompatible sizes.");
  }

  // Get iterators
  NIter I = S;
  NIter Q = S+1;
  NIter U = S+2;
  NIter V = S+3;
  NIter polx_it = polx;
  NIter poly_it = poly;
  NIter polz_it = polz;
  NIter polxh_it = polxh;
  NIter polyh_it = polyh;
  NIter polzh_it = polzh;

  // Calculate Stokes parameters
  *I = 0; *Q = 0; *U = 0; *V = 0;

  for (int i=0; i<N; i++)
  {
    *I += *polx_it * *polx_it + *polxh_it * *polxh_it;
    *Q -= *poly_it * *poly_it + *polyh_it * *polyh_it;
    *U += *polx_it * *poly_it + *polxh_it * *polyh_it;
    *V += *polxh_it * *poly_it - *polx_it * *polyh_it;

    ++polx_it;
    ++poly_it;
    ++polz_it;
    ++polxh_it;
    ++polyh_it;
    ++polzh_it;
  }

  /* I = 1/N \sum x^2 + y^2 + xh^2 + yh^2 and Q = 1/N \sum x^2 + y^2 - xh^2 - yh^2
   * but after the loop I contains x^2 + y^2 and Q contains -xh^2 -yh^2
   * so we may combine them to get the correct value using fewer calculations in the inner loop */
  temp = *I;
  *I -= *Q;
  *Q += temp;

  // Normalize
  *I = *I / N;
  *Q = *Q / N;
  *U = 2 * *U / N;
  *Q = 2 * *Q / N;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

