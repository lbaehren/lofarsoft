/**************************************************************************
 *  Calibration module for the CR Pipeline Python bindings.               *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
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

#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

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

/* GSL to STL type conversion functions. */
gsl_complex complex_to_gsl(const std::complex<double> &d)
{
  gsl_complex g;
  g.dat[0]=d.real();
  g.dat[1]=d.imag();
  return g;
}

std::complex<double> gsl_to_complex(const gsl_complex &g)
{
  std::complex<double> d(g.dat[0],g.dat[1]);
  return d;
}

double phase_interpolate(const double a, const double b, const double delta)
{
  double phase = 0.0;

  if (a * b > 0)
  {
    phase = a * (1 - delta) + b * delta;
  }
  else if (abs(a - b) < M_PI)
  {
    phase = (a + M_PI) * (1 - delta) + (b + M_PI) * delta - M_PI;
  }
  else if (a < 0)
  {
    phase = (a + 2 * M_PI) * (1 - delta) + b * delta;
  }
  else
  {
    phase = a * (1 - delta) + (b + 2 * M_PI) * delta;
  }

  return phase;
}

/* Linear interpolation of complex values on a 3 dimensional grid.
   Interpolation is performed using the phase and amplitude separately,
   no phase wrapping is applied so care is needed. */
std::complex<double> interpolate_trilinear(const std::complex<double> (&V)[8],
                                           const double x, const double y, const double z,
                                           const double x0, const double y0, const double z0,
                                           const double x1, const double y1, const double z1)
{
    double c00, c10, c01, c11, c0, c1, rho, theta;
    double a[8];
    int i;

    const double xd = (x - x0) / (x1 - x0);
    const double yd = (y - y0) / (y1 - y0);
    const double zd = (z - z0) / (z1 - z0);

    /* Interpolate amplitude */
    for (i=0; i<8; i++)
    {
      a[i] = abs(V[i]);
    }

    c00 = a[0] * (1 - xd) + a[4] * xd;
    c10 = a[2] * (1 - xd) + a[6] * xd;
    c01 = a[1] * (1 - xd) + a[5] * xd;
    c11 = a[3] * (1 - xd) + a[7] * xd;

    c0 = c00 * (1 - yd) + c10 * yd;
    c1 = c01 * (1 - yd) + c11 * yd;

    rho = c0 * (1 - zd) + c1 * zd;

    /* Interpolate phase */
    for (i=0; i<8; i++)
    {
      a[i] = arg(V[i]);
    }

    c00 = phase_interpolate(a[0], a[4], xd);
    c10 = phase_interpolate(a[2], a[6], xd);
    c01 = phase_interpolate(a[1], a[5], xd);
    c11 = phase_interpolate(a[3], a[7], xd);

    c0 = phase_interpolate(c00, c10, yd);
    c1 = phase_interpolate(c01, c11, yd);

    theta = phase_interpolate(c0, c1, zd);

    /* Return result as new complex value */
    return polar(rho, theta);
}

/* Linear interpolation of complex values on a 3 dimensional grid.
   Interpolation is performed using the phase and amplitude separately,
   no phase wrapping is applied so care is needed. */
double interpolate_amplitude_trilinear(const std::complex<double> (&V)[8],
                                       const double x, const double y, const double z,
                                       const double x0, const double y0, const double z0,
                                       const double x1, const double y1, const double z1)
{
    double c00, c10, c01, c11, c0, c1;
    double a[8];
    int i;

    const double xd = (x - x0) / (x1 - x0);
    const double yd = (y - y0) / (y1 - y0);
    const double zd = (z - z0) / (z1 - z0);

    /* Interpolate amplitude */
    for (i=0; i<8; i++)
    {
      a[i] = abs(V[i]);
    }

    c00 = a[0] * (1 - xd) + a[4] * xd;
    c10 = a[2] * (1 - xd) + a[6] * xd;
    c01 = a[1] * (1 - xd) + a[5] * xd;
    c11 = a[3] * (1 - xd) + a[7] * xd;

    c0 = c00 * (1 - yd) + c10 * yd;
    c1 = c01 * (1 - yd) + c11 * yd;

    rho = c0 * (1 - zd) + c1 * zd;

    return rho;
}

/* Linear interpolation of complex values on a 3 dimensional grid.
   Interpolation is performed using the phase and amplitude separately,
   no phase wrapping is applied so care is needed. */
double interpolate_phase_trilinear(const std::complex<double> (&V)[8],
                                   const double x, const double y, const double z,
                                   const double x0, const double y0, const double z0,
                                   const double x1, const double y1, const double z1)
{
    double c00, c10, c01, c11, c0, c1, rho, theta;
    double a[8];
    int i;

    const double xd = (x - x0) / (x1 - x0);
    const double yd = (y - y0) / (y1 - y0);
    const double zd = (z - z0) / (z1 - z0);

    /* Interpolate phase */
    for (i=0; i<8; i++)
    {
      a[i] = arg(V[i]);

      if (a[i] < 0) a[i] = 2 * M_PI + a[i];
    }

    c00 = a[0] * (1 - xd) + a[4] * xd;
    c10 = a[2] * (1 - xd) + a[6] * xd;
    c01 = a[1] * (1 - xd) + a[5] * xd;
    c11 = a[3] * (1 - xd) + a[7] * xd;

    c0 = c00 * (1 - yd) + c10 * yd;
    c1 = c01 * (1 - yd) + c11 * yd;

    theta = c0 * (1 - zd) + c1 * zd;

    return theta;
}

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
#define HFPP_PARDEF_5 (HNumber)(az)()("Azimuth in radians East positive from North.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(el)()("Elevation in radians positive up.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Convention: pol0 represents the on sky polarization theta, while pol1 represents phi

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

  // Direction in spherical coordinates with +x direction along E-W (phi = 0 is east)
  const double theta = (M_PI / 2) - el;
  const double phi = (M_PI / 2) - az;

  // Get iterators
  NIter pol0_it = pol0;
  NIter pol1_it = pol1;
  NIter polx_it = polx;
  NIter poly_it = poly;
  NIter polz_it = polz;

  for (int i=0; i<N; i++)
  {
    // Project onto new coordinate frame
    *polx_it = -1.0 * cos(phi) * cos(theta) * *pol0_it - sin(phi) * *pol1_it;
    *poly_it = -1.0 * sin(phi) * cos(theta) * *pol0_it + cos(phi) * *pol1_it;
    *polz_it = sin(theta) * *pol1_it;

    ++pol0_it;
    ++pol1_it;
    ++polx_it;
    ++poly_it;
    ++polz_it;
  }
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Project on-sky polarizations onto x,y,z.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hProjectPolarizationsInverse
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(pol0)()("Polarization 0.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(pol1)()("Polarization 1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HNumber)(polx)()("Polarization X.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(poly)()("Polarization Y.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(polz)()("Polarization Z.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Convention: pol0 represents the on sky polarization theta, while pol1 represents phi
  Theta and phi are not related to the LOFAR antenna being rotated.

*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter pol0, const NIter pol0_end,
    const NIter pol1, const NIter pol1_end,
    const NIter polx, const NIter polx_end,
    const NIter poly, const NIter poly_end,
    const NIter polz, const NIter polz_end,
    const HNumber az, const HNumber el)
{
  throw PyCR::ValueError("hProjectPolarizationsInverse is not defined.");

  /*
  // Get length of output vector
  const int N = std::distance(pol0, pol0_end);

  // Sanity checks
  if (N != std::distance(pol1, pol1_end) || N != std::distance(polx, polx_end) || N != std::distance(poly, poly_end) || N != std::distance(polz, polz_end))
  {
    throw PyCR::ValueError("[hProjectPolarizationsInverse] input vectors have incompatible sizes.");
  }

  // Direction in spherical coordinates with +x direction towards N
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
    // Cases are neccessary as the original equation system is overdefined and will get undefined for special cases of phi and theta

    if (theta == 0. ){
        *pol0_it = -1.0 * *polx_it;
        *pol1_it = *poly_it;
        }

    if (theta == M_PI / 2){
        if (phi == M_PI / 2 || phi == M_PI / 2 * 3.){
            *pol0_it = -1.0 * *polz_it;
            *pol1_it = *poly_it;
            }
         else {
            *pol0_it = -1.0 * *polz_it;
            *pol1_it = -1.0 * *polx_it / sin(theta);
            }
        }

    if (theta != 0. && theta != M_PI / 2)   {
        if (phi == M_PI / 2 ||  phi == M_PI / 2 * 3.){
            *pol0_it = *poly_it / cos(theta);
            *pol1_it = -1.0 * *polx_it;
            }
        else {
            *pol0_it = -1.0 * *polz_it / sin(theta);
            *pol1_it = *poly_it/cos(theta) + tan(phi)/tan(theta) * *polz_it;
            }
        }


    ++pol0_it;
    ++pol1_it;
    ++polx_it;
    ++poly_it;
    ++polz_it;
  }
  */
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
#define HFPP_PARDEF_3 (HNumber)(polxh)()("Hilbert transform of polarization X.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(polyh)()("Hilbert transform of polarization Y.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HInteger)(signal_start)()("Start of signal window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HInteger)(signal_end)()("End of signal window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
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

template <class NIter, class Iter>
void HFPP_FUNC_NAME (const NIter S, const NIter S_end,
    const NIter polx, const NIter polx_end,
    const NIter poly, const NIter poly_end,
    const NIter polxh, const NIter polxh_end,
    const NIter polyh, const NIter polyh_end,
    const Iter signal_start, const Iter signal_start_stub,
    const Iter signal_end, const Iter signal_end_stub)
{
  double temp = 0;

  // Get length of input vector
  const int N = std::distance(polx, polx_end);
  const int n = *signal_end - *signal_start;

  // Sanity checks
  if (std::distance(S, S_end) != 4 || N != std::distance(poly, poly_end) || N != std::distance(polxh, polxh_end) || N != std::distance(polyh, polyh_end) || n > N)
  {
    throw PyCR::ValueError("[hStokesParameters] input vectors have incompatible sizes.");
  }

  // Get iterators
  NIter I = S;
  NIter Q = S+1;
  NIter U = S+2;
  NIter V = S+3;
  NIter polx_it = polx + *signal_start;
  NIter poly_it = poly + *signal_start;
  NIter polxh_it = polxh + *signal_start;
  NIter polyh_it = polyh + *signal_start;

  // Calculate Stokes parameters
  *I = 0; *Q = 0; *U = 0; *V = 0;

  for (int i=0; i<n; i++)
  {
    *I += *polx_it * *polx_it + *polxh_it * *polxh_it;
    *Q -= *poly_it * *poly_it + *polyh_it * *polyh_it;
    *U += *polx_it * *poly_it + *polxh_it * *polyh_it;
    *V += *polxh_it * *poly_it - *polx_it * *polyh_it;

    ++polx_it;
    ++poly_it;
    ++polxh_it;
    ++polyh_it;
  }

  /* I = 1/N \sum x^2 + y^2 + xh^2 + yh^2 and Q = 1/N \sum x^2 + y^2 - xh^2 - yh^2
   * but after the loop I contains x^2 + y^2 and Q contains -xh^2 -yh^2
   * so we may combine them to get the correct value using fewer calculations in the inner loop */
  temp = *I;
  *I -= *Q;
  *Q += temp;

  // Normalize
  *I = *I / n;
  *Q = *Q / n;
  *U = 2 * *U / n;
  *Q = 2 * *Q / n;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate Jones matrix for given frequencies and positions.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetJonesMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(J)()("Jones matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(f)()("Frequency.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(az)()("Azimuth in degrees East positive from North.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(el)()("Elevation in degrees positive up.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HComplex)(Vtheta)()("Table with response of theta polarization.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HComplex)(Vphi)()("Table with response of phi polarization.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HNumber)(fstart)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(fstep)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HInteger)(fn)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_9 (HNumber)(tstart)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_10 (HNumber)(tstep)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_11 (HInteger)(tn)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_12 (HNumber)(pstart)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_13 (HNumber)(pstep)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_14 (HInteger)(pn)()("")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter J, const CIter J_end,
    const HNumber f, const HNumber az, HNumber el,
    const CIter Vtheta, const CIter Vtheta_end,
    const CIter Vphi, const CIter Vphi_end,
    const HNumber fstart, const HNumber fstep, const HInteger fn,
    const HNumber tstart, const HNumber tstep, const HInteger tn,
    const HNumber pstart, const HNumber pstep, const HInteger pn)
{
  // Variables
  HComplex V[8];
  HInteger fi, ti, pi, fe, te, pe;
  HNumber x, y, z_xdipole, z_ydipole, x0, y0, z0, x1, y1, z1;

  // Direction in spherical coordinates in degrees with +x direction towards N
  double theta = 90. - el;
  double phi = 225. - az;

  const HNumber fend = fstart + fstep * fn;

  // Sanity checks
  const HInteger jn = std::distance(J, J_end);
  const HInteger vtn = std::distance(Vtheta, Vtheta_end);
  const HInteger vpn = std::distance(Vphi, Vphi_end);

  if (jn != 4) throw PyCR::ValueError("Jones matrix array has incorrect size.");
  if (vtn != fn * tn * pn) throw PyCR::ValueError("Vtheta array has incorrect size.");
  if (vpn != fn * tn * pn) throw PyCR::ValueError("Vphi array has incorrect size.");
  if (f < 0) throw PyCR::ValueError("Invalid negative frequency.");
  if (theta < 0) throw PyCR::ValueError("Invalid negative angle theta.");
  if (phi < 0) phi += 360.;
  if (phi < 0) throw PyCR::ValueError("Invalid negative angle phi.");

  // Get iterators
  CIter J_it = J;

  // Set dimensions
  if (f >= fstart && f < fend)
  {
    x = f;
  }
  else if (f < fstart)
  {
    x = fstart;
  }
  else
  {
    x = fend;
  }

  y = theta;
  z_xdipole = fmod(phi, 360.0);
  z_ydipole = fmod(phi + 90.0, 360.0);

  // Calculate index into frequency, theta and phi parts of the array before the requested point
  fi = int((x - fstart) / fstep);
  fe = fi + 1;
  ti = int((y - tstart) / tstep);
  te = ti + 1;

  /********************************* x - dipole response for wave polarized purely in theta direction *********************************/

  // Calculate index into frequency, theta and phi parts of the array before the requested point
  pi = int((z_xdipole - pstart) / pstep);
  pe = pi + 1;

  // Retrieve values from theta table for corners of the cube surrounding the requested point
  V[0] = *(Vtheta + (fi * tn * pn) + (ti * pn) + pi);
  V[1] = *(Vtheta + (fi * tn * pn) + (ti * pn) + pe);
  V[2] = *(Vtheta + (fi * tn * pn) + (te * pn) + pi);
  V[3] = *(Vtheta + (fi * tn * pn) + (te * pn) + pe);

  V[4] = *(Vtheta + (fe * tn * pn) + (ti * pn) + pi);
  V[5] = *(Vtheta + (fe * tn * pn) + (ti * pn) + pe);
  V[6] = *(Vtheta + (fe * tn * pn) + (te * pn) + pi);
  V[7] = *(Vtheta + (fe * tn * pn) + (te * pn) + pe);

  // Calculate corresponding start / end values for each dimension
  x0 = fstart + fi * fstep; x1 = x0 + fstep;
  y0 = tstart + ti * tstep; y1 = y0 + tstep;
  z0 = pstart + pi * pstep; z1 = z0 + pstep;

  // Interpolate to find Jones matrix component for theta
  *J_it++ = interpolate_trilinear(V, x, y, z_xdipole, x0, y0, z0, x1, y1, z1);

  /********************************* y - dipole response for wave polarized purely in theta direction *********************************/

  // Calculate index into frequency, theta and phi parts of the array before the requested point
  pi = int((z_ydipole - pstart) / pstep);
  pe = pi + 1;

  // Retrieve values from theta table for corners of the cube surrounding the requested point
  V[0] = *(Vtheta + (fi * tn * pn) + (ti * pn) + pi);
  V[1] = *(Vtheta + (fi * tn * pn) + (ti * pn) + pe);
  V[2] = *(Vtheta + (fi * tn * pn) + (te * pn) + pi);
  V[3] = *(Vtheta + (fi * tn * pn) + (te * pn) + pe);

  V[4] = *(Vtheta + (fe * tn * pn) + (ti * pn) + pi);
  V[5] = *(Vtheta + (fe * tn * pn) + (ti * pn) + pe);
  V[6] = *(Vtheta + (fe * tn * pn) + (te * pn) + pi);
  V[7] = *(Vtheta + (fe * tn * pn) + (te * pn) + pe);

  // Calculate corresponding start / end values for each dimension
  x0 = fstart + fi * fstep; x1 = x0 + fstep;
  y0 = tstart + ti * tstep; y1 = y0 + tstep;
  z0 = pstart + pi * pstep; z1 = z0 + pstep;

  // Interpolate to find Jones matrix component for theta
  *J_it++ = interpolate_trilinear(V, x, y, z_ydipole, x0, y0, z0, x1, y1, z1);

  /********************************* x - dipole response for wave polarized purely in phi direction *********************************/

  pi = int((z_xdipole - pstart) / pstep);
  pe = pi + 1;

  // Retrieve values from phi table for corners of the cube surrounding the requested point
  V[0] = *(Vphi + (fi * tn * pn) + (ti * pn) + pi);
  V[1] = *(Vphi + (fi * tn * pn) + (ti * pn) + pe);
  V[2] = *(Vphi + (fi * tn * pn) + (te * pn) + pi);
  V[3] = *(Vphi + (fi * tn * pn) + (te * pn) + pe);

  V[4] = *(Vphi + (fe * tn * pn) + (ti * pn) + pi);
  V[5] = *(Vphi + (fe * tn * pn) + (ti * pn) + pe);
  V[6] = *(Vphi + (fe * tn * pn) + (te * pn) + pi);
  V[7] = *(Vphi + (fe * tn * pn) + (te * pn) + pe);

  // Calculate corresponding start / end values for each dimension
  x0 = fstart + fi * fstep; x1 = x0 + fstep;
  y0 = tstart + ti * tstep; y1 = y0 + tstep;
  z0 = pstart + pi * pstep; z1 = z0 + pstep;

  // Interpolate to find Jones matrix component for phi
  *J_it++ = interpolate_trilinear(V, x, y, z_xdipole, x0, y0, z0, x1, y1, z1);

  /********************************* y - dipole response for wave polarized purely in phi direction *********************************/

  // Calculate index into frequency, theta and phi parts of the array before the requested point
  pi = int((z_ydipole - pstart) / pstep);
  pe = pi + 1;

  // Retrieve values from theta table for corners of the cube surrounding the requested point
  V[0] = *(Vphi + (fi * tn * pn) + (ti * pn) + pi);
  V[1] = *(Vphi + (fi * tn * pn) + (ti * pn) + pe);
  V[2] = *(Vphi + (fi * tn * pn) + (te * pn) + pi);
  V[3] = *(Vphi + (fi * tn * pn) + (te * pn) + pe);

  V[4] = *(Vphi + (fe * tn * pn) + (ti * pn) + pi);
  V[5] = *(Vphi + (fe * tn * pn) + (ti * pn) + pe);
  V[6] = *(Vphi + (fe * tn * pn) + (te * pn) + pi);
  V[7] = *(Vphi + (fe * tn * pn) + (te * pn) + pe);

  // Calculate corresponding start / end values for each dimension
  x0 = fstart + fi * fstep; x1 = x0 + fstep;
  y0 = tstart + ti * tstep; y1 = y0 + tstep;
  z0 = pstart + pi * pstep; z1 = z0 + pstep;

  // Interpolate to find Jones matrix component for theta
  *J_it++ = interpolate_trilinear(V, x, y, z_ydipole, x0, y0, z0, x1, y1, z1);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Mix two vectors using a mixing matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMatrixMix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec0)()("Vector 0.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vec1)()("Vector 1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(M)()("2 dimensional mixing matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter vec0, const CIter vec0_end,
    const CIter vec1, const CIter vec1_end,
    const CIter M, const CIter M_end)
{
  // Temporary variable
  complex<double> temp[2];

  // Get lengths
  const int Nj = std::distance(M, M_end);
  const int N = std::distance(vec0, vec0_end);

  // Sanity checks
  if ((N != std::distance(vec1, vec1_end)) || (Nj != 4*N))
  {
    throw PyCR::ValueError("[hMatrixMix] input vectors have incompatible sizes.");
  }

  // Get iterators
  CIter vec0_it = vec0;
  CIter vec1_it = vec1;
  CIter M_it = M;

  for (int i=0; i<N; i++)
  {
    temp[0]  = *M_it++ * *vec0_it;
    temp[0] += *M_it++ * *vec1_it;
    temp[1]  = *M_it++ * *vec0_it;
    temp[1] += *M_it++ * *vec1_it;

    *vec0_it++ = temp[0];
    *vec1_it++ = temp[1];
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Invert n x n matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvertMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(Minv)()("Inverse matrix (can be the same as the input to save memory).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(M)()("Matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(n)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter Minv, const NIter Minv_end,
    const NIter M, const NIter M_end,
    const HInteger n)
{
  // Sanity checks
  const HInteger N = n * n;

  if (std::distance(Minv, Minv_end) != N || std::distance(M, M_end) != N)
  {
    throw PyCR::ValueError("Matrices are not square or identical in size.");
  }

  int s = 0;
  gsl_matrix *work = gsl_matrix_alloc(n,n);
  gsl_matrix *winv = gsl_matrix_alloc(n,n);
  gsl_permutation *p = gsl_permutation_alloc(n);

  NIter Minv_it = Minv;
  NIter M_it = M;

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<n; j++)
    {
      gsl_matrix_set(work, i, j, *M_it);
      M_it++;
    }
  }

  gsl_linalg_LU_decomp(work, p, &s);
  gsl_linalg_LU_invert(work, p, winv);

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<n; j++)
    {
      *Minv_it = gsl_matrix_get(winv, i, j);
      Minv_it++;
    }
  }

  gsl_matrix_free(work);
  gsl_matrix_free(winv);
  gsl_permutation_free(p);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Invert n x n matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvertComplexMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Minv)()("Inverse matrix (can be the same as the input to save memory).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(M)()("Matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(n)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter Minv, const CIter Minv_end,
    const CIter M, const CIter M_end,
    const HInteger n)
{
  // Sanity checks
  const HInteger N = n * n;

  if (std::distance(Minv, Minv_end) != N || std::distance(M, M_end) != N)
  {
    throw PyCR::ValueError("Matrices are not square or identical in size.");
  }

  int s = 0;
  gsl_matrix_complex *work = gsl_matrix_complex_alloc(n,n);
  gsl_matrix_complex *winv = gsl_matrix_complex_alloc(n,n);
  gsl_permutation *p = gsl_permutation_alloc(n);

  CIter Minv_it = Minv;
  CIter M_it = M;

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<n; j++)
    {
      gsl_matrix_complex_set(work, i, j, complex_to_gsl(*M_it));
      M_it++;
    }
  }

  gsl_linalg_complex_LU_decomp(work, p, &s);
  gsl_linalg_complex_LU_invert(work, p, winv);

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<n; j++)
    {
      *Minv_it = gsl_to_complex(gsl_matrix_complex_get(winv, i, j));
      Minv_it++;
    }
  }

  gsl_matrix_complex_free(work);
  gsl_matrix_complex_free(winv);
  gsl_permutation_free(p);
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Multiply two matrices
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMultiplyMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(C)()("Output (n * p) matrix (can be the same as one of the two inputs to save memory but only for square matrices).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(A)()("Input (n * m) matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(B)()("Input (m * p) matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(n)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(m)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(p)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter C, const NIter C_end,
    const NIter A, const NIter A_end,
    const NIter B, const NIter B_end,
    const HInteger n, const HInteger m, const HInteger p)
{
  const double alpha = 1.0;
  const double beta = 0.0;

  // Sanity check
  if (std::distance(A, A_end) != n * m || std::distance(B, B_end) != m * p || std::distance(C, C_end) != n * p)
  {
    throw PyCR::ValueError("Matrices have wrong size.");
  }

  gsl_matrix *a = gsl_matrix_alloc(n,m);
  gsl_matrix *b = gsl_matrix_alloc(m,p);
  gsl_matrix *c = gsl_matrix_alloc(n,p);

  // Get iterators
  NIter A_it = A;
  NIter B_it = B;
  NIter C_it = C;

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<m; j++)
    {
      gsl_matrix_set(a, i, j, *A_it);
      A_it++;
    }
  }

  for (HInteger i=0; i<m; i++)
  {
    for (HInteger j=0; j<p; j++)
    {
      gsl_matrix_set(b, i, j, *B_it);
      B_it++;
    }
  }

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, alpha, a, b, beta, c);

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<p; j++)
    {
      *C_it = gsl_matrix_get(c, i, j);
      C_it++;
    }
  }

  gsl_matrix_free(a);
  gsl_matrix_free(b);
  gsl_matrix_free(c);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Multiply two matrices
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMultiplyComplexMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(C)()("Output (n * p) matrix (can be the same as one of the two inputs to save memory but only for square matrices).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(A)()("Input (n * m) matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(B)()("Input (m * p) matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(n)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(m)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(p)()("Dimension.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter C, const CIter C_end,
    const CIter A, const CIter A_end,
    const CIter B, const CIter B_end,
    const HInteger n, const HInteger m, const HInteger p)
{
  const gsl_complex alpha = gsl_complex_rect(1.0, 0.0);
  const gsl_complex beta = gsl_complex_rect(0.0, 0.0);

  // Sanity check
  if (std::distance(A, A_end) != n * m || std::distance(B, B_end) != m * p || std::distance(C, C_end) != n * p)
  {
    throw PyCR::ValueError("Matrices have wrong size.");
  }

  gsl_matrix_complex *a = gsl_matrix_complex_alloc(n,m);
  gsl_matrix_complex *b = gsl_matrix_complex_alloc(m,p);
  gsl_matrix_complex *c = gsl_matrix_complex_alloc(n,p);

  // Get iterators
  CIter A_it = A;
  CIter B_it = B;
  CIter C_it = C;

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<m; j++)
    {
      gsl_matrix_complex_set(a, i, j, complex_to_gsl(*A_it));
      A_it++;
    }
  }

  for (HInteger i=0; i<m; i++)
  {
    for (HInteger j=0; j<p; j++)
    {
      gsl_matrix_complex_set(b, i, j, complex_to_gsl(*B_it));
      B_it++;
    }
  }

  gsl_blas_zgemm(CblasNoTrans, CblasNoTrans, alpha, a, b, beta, c);

  for (HInteger i=0; i<n; i++)
  {
    for (HInteger j=0; j<p; j++)
    {
      *C_it = gsl_to_complex(gsl_matrix_complex_get(c, i, j));
      C_it++;
    }
  }

  gsl_matrix_complex_free(a);
  gsl_matrix_complex_free(b);
  gsl_matrix_complex_free(c);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Invert 2 x 2 matrix
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvertComplexMatrix2D
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Minv)()("Inverse matrix (can be the same as the input to save memory).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(M)()("Matrix.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/

template <class CIter>
void HFPP_FUNC_NAME (const CIter Minv, const CIter Minv_end,
    const CIter M, const CIter M_end)
{
  const HInteger Nout = std::distance(Minv, Minv_end);
  const HInteger Nin = std::distance(M, M_end);
  const HInteger Nm = Nout / 4;

  if (Nout != Nin || Nout != Nm * 4)
  {
    throw PyCR::ValueError("Matrices have wrong size.");
  }

  CIter Minv_it = Minv;
  CIter M_it = M;
  HComplex det;

  for (HInteger i=0; i<Nm; i++)
  {
    det = *(M_it + 0) * *(M_it + 3) - *(M_it + 1) * *(M_it + 2);

    *(Minv_it + 0) = *(M_it + 3) / det;
    *(Minv_it + 1) = -1.0 * *(M_it + 1) / det;
    *(Minv_it + 2) = -1.0 * *(M_it + 2) / det;
    *(Minv_it + 3) = *(M_it + 0) / det;

    Minv_it += 4;
    M_it += 4;
  }
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

