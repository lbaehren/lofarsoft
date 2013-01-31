/**************************************************************************
 *  Imaging module for the CR Pipeline Python bindings.                   *
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
//$FILENAME: HFILE=mImaging.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include <time.h>
#include "core.h"
#include "legacy.h"
#include "mMath.h"
#include "mImaging.h"

#include <tmf.h>

#ifdef _OPENMP
#include <omp.h>
#endif


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
//$SECTION: Imaging functions
// ========================================================================

//$DOCSTRING: Converts a 3D spatial vector into a different Coordinate type (e.g. Spherical to Cartesian).
//$ORIGIN: Math/VectorConversion.cc
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCoordinateConvert
//-----------------------------------------------------------------------
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(source)()("Coordinates of the source to be converted - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (CRCoordinateType)(sourceCoordinate)()("Type of the coordinates for the source")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(target)()("Coordinates of the source to be converted - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (CRCoordinateType)(targetCoordinate)()("Type of the coordinates for the target (output vector)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (bool)(anglesInDegrees)()("True if the angles are in degree, otherwise in radians")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Available Coordinate Types are:

    ================ =========================================== ====================================
    AzElHeight       Azimuth-Elevation-Height                    :math:`\\vec x = (Az,El,H)`
    AzElRadius       Azimuth-Elevation-Radius                    :math:`\\vec x = (Az,El,R)`
    Cartesian        Cartesian coordinates                       :math:`\\vec x = (x,y,z)`
    Cylindrical      Cylindrical coordinates                     :math:`\\vec x = (r,\\phi,h)`
    Direction        Direction on the sky                        :math:`\\vec x = (Lon,Lat)`
    DirectionRadius  Direction on the sky with radial distance   :math:`\\vec x = (Lon,Lat,R)`
    Frquency         Frequency
    LongLatRadius    Longitude-Latitude-Radius
    NorthEastHeight  North-East-Height
    Spherical        Spherical coordinates                       :math:`\\vec x = (r,\\phi,\\theta)`
    Time             Time
    ================ =========================================== ====================================

    Precede the Coordinate Types by ``CoordinateTypes.``, i.e., CoordinateTypes.AzElRadius.

    Example:

    hCoordinateConvert(meandirection,CoordinateTypes.AzElRadius,azelr,CoordinateTypes.Cartesian,False)
  */

template <class Iter>
bool HFPP_FUNC_NAME  (Iter source,
                      CR::CoordinateType::Types const &sourceCoordinate,
                      Iter target,
                      CR::CoordinateType::Types const &targetCoordinate,
                      bool anglesInDegrees
                      )
{
  return CR::convertVector(*target,
                           *(target+1),
                           *(target+2),
                           targetCoordinate,
                           *source,
                           *(source+1),
                           *(source+2),
                           sourceCoordinate,
                           anglesInDegrees
                               );
    }
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a single complex pixel or a pixel vector (i.e., an image), from an cross correlation matrix and complex geometrical weights for each antenna by multiplying ``ccm`` and weights and summing over all baselines.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCImageFromCCM
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(image)()("Image vector containing ``N_freq`` frequency bins in the order ``[power(pix0,bin0), power(pix0,bin1), ..., power(pix1,bin0), ...]``. Size of vector is ``N_pixel * N_bins``")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(ccm)()("Upper half of the cross-correlation matrix (input) containing complex visibilities as a function of frequency. The ordering is ``ant0 * ant1, ant0 * ant2, ..., ant1 * ant2, ...`` - where each ``antN`` contains ``nfreq`` frequency bins.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(weights)()("Vector containing the weights to be applied for each pixel, antenna, and frequency bin (in that order, i.e. frequency index runs fastest).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(nofAntennas)()("Number of antennas used for ``weights`` and ``ccm``")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(nfreq)()("Number of frequency bins used for ``weights`` and ``ccm``")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The image contains the power as a function of pixel and frequency
  bins. Each value is the sum over all baselines of the norm
  of the complex visibilities times complex weights for that pixel and
  frequency bin.

  The results will be added to the image, hence for a new image the
  image must be initialized to 0! Otherwise an old image can be
  provided and the new data will simply be added to it.

  The length of the ``ccm`` vector is ``(N-1)/2*N_freq``, where ``N`` is the
  number of antennas and ``N_freq`` the number of frequency bins per
  antenna. The length of the weights vector is ``N_pixel*N_ant*N_freq``.

  Usage:
  hCrossCorrelationMatrix(ccm,fftdata,nfreq) -> ccm = ccm(old) + ccm(fftdata)

  Example:
  >>> ccm=Vector(complex,file[\"nofSelectedAntennas\"]*(file[\"nofSelectedAntennas\"]-1)/2*file[\"fftLength\"])
  >>> cimage=Vector(complex,n_pixels*nbins,fill=0)
  >>> image=hArray(float,[n_pixels,file[\"fftLength\"]])

  >>> hCrossCorrelationMatrix(ccm,file_fft.vec(),file[\"fftLength\"])
  >>> hCImageFromCCM(cimage,ccm,weights.vec(),file[\"nofSelectedAntennas\"],file[\"fftLength\"])
  >>> cimage.norm(image.vec())
  >>> intimage=np.array(image[...].sum()) # convert to numpy array
  >>> immax=intimage.max()
  >>> intimage /= immax
  >>> intimage.resize([n_az,n_el])

  >>> plt.imshow(intimage,cmap=plt.cm.hot)
*/
template <class IterC>
void HFPP_FUNC_NAME(const IterC image, const IterC image_end,
                    const IterC ccm,const IterC ccm_end,
                    const IterC weights,const IterC weights_end,
                    const HInteger nofAntennas,
                    const HInteger nfreq
                    )
{
  IterC it_im(image);
  IterC it_im_start=image;
  IterC it_im_end=image+nfreq;
  IterC it_ccm(ccm);
  IterC it_w1_start(weights);
  IterC it_w1_end(weights+nfreq);
  IterC it_w2_end = it_w1_start+nofAntennas*nfreq;
  IterC it_w1(it_w1_start); //start at antenna 0
  IterC it_w2(it_w1_end); //start at antenna 1
  HInteger lenCCM = ccm_end - ccm;
  HInteger lenWeights = weights_end - weights;

  if (lenCCM <= 0) {
    throw PyCR::ValueError("Incorrect size for ccm vector (len <= 0).");
    return;
  }
  if (lenWeights <= 0) {
    throw PyCR::ValueError("Incorrect size for weight vector (len <= 0).");
    return;
  }

  while (it_im_start<image_end) {
    *it_im+=(*it_ccm) * (*it_w1)*conj(*it_w2);
    ++it_w1; ++it_w2; ++it_ccm; ++it_im;
    if (it_w1>=it_w1_end) { //reached the end of frequency bins of first antenna, restart at first bin and advance second antenna by one
      if (it_w2>=it_w2_end) { //the second antenna has reached the last antenna value (last baseline to first antenna), so increase first antenna by one
        it_w1_start+=nfreq; //advance by one antenna (each containing nfreq frequencies), i.e. next baseline
        it_w1_end+=nfreq;
        if (it_w1_end>=it_w2_end) { //has cycled through all antenna pairs/all baselines, restart at beginning and advance to next pixel
          if (it_im!=it_im_end) {
            ERROR_RETURN("hImageFromCCM: Mismatch between image vector, ccm, and weights");
          };
          if (it_ccm!=ccm_end) {
            ERROR_RETURN("Error - hImageFromCCM: Mismatch between ccm and weights");
          };
          it_im_start=it_im_end;  //first frequency bin of NEXT PIXEL
          it_im_end=it_im_start+nfreq;
          it_w1_start=it_w2_end;  // at the end of w2 comes the next sequence fo weights for the next pixel
          it_w1_end=it_w1_start+nfreq;
          it_w2_end = it_w1_start+nofAntennas*nfreq;
          it_ccm=ccm; //ccm begins at first frequency first baseline again
        };
        it_w2=it_w1_end; //start second antenna pair pointer at antenna 1 - start with baseline 0-1
      };
      it_w1=it_w1_start;  //start over at first frequency bin in weights (with next baseline)
      it_im=it_im_start; //start over at first frequency bin in image (with next baseline)
    };
    if (it_im>=it_im_end) {
      ERROR("hImageFromCCM: Mismatch between image vector, ccm, and weights");
      return;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Coverts a vector of time delays and a vector of frequencies to a corresponding vector of phases (of a complex number).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDelayToPhase
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Output vector returning real phases of complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Input vector with real delays in units of time [second]")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(delays)()("Input vector with real delays in units of time [second]")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The frequencies always run fastest in the output vector.  If input
  vectors are shorter they will be wrapped until the output vector is
  full.

  Usage:
  hDelayToPhase(vec,frequencies,delays) -> vec = [phase(frequencies_0,delays_0),phase(frequencies_1,delays_0),...,phase(frequencies_n,delays_0),phase(frequencies_1,delays_1),...]
*/
template <class Iter>
void  HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
                     const Iter frequencies, const Iter frequencies_end,
                     const Iter delays, const Iter delays_end)
{
  Iter it1=vec;
  Iter it2=frequencies;
  Iter it3=delays;
  HInteger lenVec = vec_end - vec;
  HInteger lenFreq = frequencies_end - frequencies;
  HInteger lenDelay = delays_end - delays;

  if (lenVec <= 0) {
    throw PyCR::ValueError("Incorrect size of output vector (len <= 0).");
    return;
  }
  if (lenFreq <= 0) {
    throw PyCR::ValueError("Incorrect size of frequencies vector (len <= 0).");
    return;
  }
  if (lenDelay <= 0) {
    throw PyCR::ValueError("Incorrect size of delays vector (len <= 0).");
    return;
  }

  while (it1!=vec_end) {
    *it1=hPhase(*it2,*it3);
    ++it1;
    ++it2; if (it2==frequencies_end) {
      it2=frequencies;
      ++it3; if (it3==delays_end) it3=delays; // loop over input vector if necessary ...
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located in a certain direction in farfield (based on L. Bahren).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelayFarField
//-----------------------------------------------------------------------
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(antPosition)()("Cartesian antenna positions (Meters) relative to a reference location (phase center) - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(skyDirection)()("Vector in Cartesian coordinates pointing towards a sky position from the antenna - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(length)()("Length of the ``skyDirection`` vector, used for normalization - provided to speed up calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
HNumber HFPP_FUNC_NAME (
                        const Iter antPosition,
                        const Iter skyDirection,
                        HNumber length
                        )
{
  return - (*skyDirection * *antPosition
            + *(skyDirection+1) * *(antPosition+1)
            + *(skyDirection+2) * *(antPosition+2))/length/CR::lightspeed;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Calculates the time delay in seconds for a signal received at an antenna position relative to a phase center from a source located at a certain 3D space coordinate in nearfield (based on L. Bahren).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelayNearField
//-----------------------------------------------------------------------
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(antPosition)()("Cartesian antenna positions [meter]) relative to a reference location (phase center) - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(skyPosition)()("Vector in Cartesian coordinates [meter] pointing towards a sky location, relative to phase center - vector of length 3")(HFPP_PAR_IS_VECTOR)(STDITFIXED)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(distance)()("Distance of source, i.e. the length of ``skyPosition`` - provided to speed up calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
HNumber HFPP_FUNC_NAME (
                        const Iter antPosition,
                        const Iter skyPosition,
                        const HNumber distance)
{
  return (
          sqrt(
               square(*skyPosition - *antPosition)
               +square(*(skyPosition+1) - *(antPosition+1))
               +square(*(skyPosition+2) - *(antPosition+2))
               ) - distance
          )/CR::lightspeed;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the time delay in seconds for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricDelays
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(delays)()("Output vector containing the delays in seconds for all antennas and positions [antenna index runs fastest: ``(ant1, pos1), (ant2,pos1), ...``] - length of vector has to be number of antennas times positions")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(antPositions)()("Cartesian antenna positions [meter] relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(skyPositions)()("Vector in Cartesian coordinates [meter] pointing towards a sky location, relative to phase center - vector of length number of ``skypositions`` times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (bool)(farfield)()("Calculate in farfield approximation if ``true``, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> result=[]
  >>> plt.clf()
  >>> for j in range(29,30):
  >>>     x=[]
  >>>     y=[]
  >>>     for i in range(177,178):
  >>>         azel[0]=float(i)
  >>>         azel[1]=float(j)
  >>>         hCoordinateConvert(azel,CoordinateTypes.AzElRadius,cartesian,CoordinateTypes.Cartesian,True)
  >>>         hGeometricDelays(delays,antenna_positions,hArray(cartesian),True)
  >>>         delays *= 10**6
  >>>         deviations=delays-obsdelays
  >>>         deviations.abs()
  >>>         sum=deviations.vec().sum()
  >>>         x.append(i)
  >>>         y.append(sum)
  >>>     result.append(y)
  >>>     plt.plot(x,y)
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter delays, const Iter delays_end,
                     const Iter antPositions, const Iter antPositions_end,
                     const Iter skyPositions, const Iter skyPositions_end,
                     const bool farfield
                     )
{
  // Declaration of variables
  HNumber distance;
  Iter ant=antPositions;
  Iter sky=skyPositions;
  Iter del=delays;
  Iter ant_end=antPositions_end-2;
  Iter sky_end=skyPositions_end-2;
  HInteger lenDelay = delays_end - delays;
  HInteger lenAnt = antPositions_end - antPositions;
  HInteger lenSky = skyPositions_end - skyPositions;

  // Sanity check
  if (lenDelay <= 0) {
    throw PyCR::ValueError("Incorrect size of delays vector (len <= 0).");
  }
  if ((lenAnt <= 0) || (lenAnt%3 != 0)) {
    throw PyCR::ValueError("Incorrect size of antenna position vector (len <= 0).");
  }
  if ((lenSky <= 0) || (lenSky%3 != 0)) {
    throw PyCR::ValueError("Incorrect size of sky position vector (len <= 0).");
  }

  // Implementation
  if (farfield) {
    while (del < delays_end) {
      distance = hVectorLength(sky,sky+3);
      *del=hGeometricDelayFarField(ant,sky,distance);
      ++del;
      ant+=3; if (ant>=ant_end) {
        ant=antPositions;
        sky+=3; if (sky>=sky_end) sky=skyPositions;
      };
    };
  } else {
    while (del < delays_end) {
      distance = hVectorLength(sky,sky+3);
      *del=hGeometricDelayNearField(ant,sky,distance);
      ++del;
      ant+=3; if (ant>=ant_end) {
        ant=antPositions;
        sky+=3; if (sky>=sky_end) sky=skyPositions;
      };
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the phase gradients for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricPhases
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(phases)()("Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: ``(nu1, ant1, pos1), (nu2, ant1, pos1), ...``] - length of vector has to be number of antennas times positions time frequency bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Vector of frequencies [Hz] to calculate phases for")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(antPositions)()("Cartesian antenna positions [meter] relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(skyPositions)()("Vector in Cartesian coordinates [meter] pointing towards a sky location, relative to phase center - vector of length number of ``skypositions`` times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (bool)(farfield)()("Calculate in farfield approximation if ``true``, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter phases, const Iter phases_end,
                     const Iter frequencies, const Iter frequencies_end,
                     const Iter antPositions, const Iter antPositions_end,
                     const Iter skyPositions, const Iter skyPositions_end,
                     const bool farfield
                     )
{
  // Declaration of variables
  HNumber distance=0.;
  Iter ant=antPositions;
  Iter freq=frequencies;
  Iter sky=skyPositions;
  Iter phase=phases;
  Iter ant_end=antPositions_end-2;
  Iter sky_end=skyPositions_end-2;
  HInteger lenPhases = phases_end - phases;
  HInteger lenFreq = frequencies_end - frequencies;
  HInteger lenAnt = antPositions_end - antPositions;
  HInteger lenSky = skyPositions_end - skyPositions;

  // Sanity check
  if (lenPhases <= 0) {
    throw PyCR::ValueError("Incorrect size of phase vector (len <= 0).");
  }
  if (lenFreq <= 0) {
    throw PyCR::ValueError("Incorrect size of frequency vector (len <= 0).");
  }
  if ((lenAnt <=0) || (lenAnt%3 != 0)) {
    throw PyCR::ValueError("Incorrect size of antenna position vector (len <= 0, or no multiple of 3).");
  }
  if ((lenSky <= 0) || (lenSky%3 != 0)) {
    throw PyCR::ValueError("Incorrect size of sky position vector (len <= 0, or no multiple of 3).");
  }

  // Implementation
  if (farfield) {
    while (sky < sky_end && phase < phases_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && phase < phases_end) {
        freq=frequencies;
        while (freq < frequencies_end && phase < phases_end) {
          *phase=hPhase(*freq,hGeometricDelayFarField(ant,sky,distance));
          ++phase; ++freq;
        };
        ant+=3;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && phase < phases_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && phase < phases_end) {
        freq=frequencies;
        while (freq < frequencies_end && phase < phases_end) {
          *phase=hPhase(*freq,hGeometricDelayNearField(ant,sky,distance));
          ++phase; ++freq;
        };
        ant+=3;
      };
      sky+=3;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the phase gradients as complex weights for signals received at various antenna positions relative to a phase center from sources located at certain 3D space coordinates in near or far field and for different frequencies.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGeometricWeights
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(weights)()("Output vector containing the phases in radians for all frequencies, antennas and positions [frequency index, runs fastest, then antenna index: ``(nu1, ant1, pos1), (nu2, ant1, pos1), ...``] - length of vector has to be number of antennas times positions time frequency bins")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Vector of frequencies [Hz] to calculate phases for")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(antPositions)()("Cartesian antenna positions [meter] relative to a reference location (phase center) - vector of length number of antennas times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(skyPositions)()("Vector in Cartesian coordinates [meter] pointing towards a sky location, relative to phase center - vector of length number of ``skypositions`` times three")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (bool)(farfield)()("Calculate in farfield approximation if ``true``, otherwise do near field calculation")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class Iter>
void HFPP_FUNC_NAME (const CIter weights, const CIter weights_end,
                     const Iter frequencies, const Iter frequencies_end,
                     const Iter antPositions, const Iter antPositions_end,
                     const Iter skyPositions, const Iter skyPositions_end,
                     const bool farfield
                     )
{
  // Declaration of variables
  HNumber distance;
  Iter ant=antPositions;
  Iter freq=frequencies;
  Iter sky=skyPositions;
  Iter ant_end=antPositions_end-2;
  Iter sky_end=skyPositions_end-2;
  CIter weight=weights;

  HInteger lenWeight = weights_end - weights;
  HInteger lenFreq = frequencies_end - frequencies;
  HInteger lenAnt = antPositions_end - antPositions;
  HInteger lenSky = skyPositions_end - skyPositions;

  // Sanity check
  if (lenWeight <=0) {
    throw PyCR::ValueError("Incorrect size of weight vector (len <= 0).");
  }
  if (lenFreq <= 0) {
    throw PyCR::ValueError("Incorrect size of frequency vector (len <= 0).");
  }
  if ((lenAnt <=0) || (lenAnt%3 != 0)) {
    char error_message[256];
    sprintf(error_message, "Incorrect size of antenna position vector: (len[=%d] <= 0, or no multiple of 3).", int(lenAnt));
    throw PyCR::ValueError(error_message);
  }
  if ((lenSky <= 0) || (lenSky%3 != 0)) {
    char error_message[256];
    sprintf(error_message, "Incorrect size of sky position vector (len[=%d] <= 0, or no multiple of 3).", int(lenSky));
    throw PyCR::ValueError(error_message);
  }

  // Implementation
  if (farfield) {
    while (sky < sky_end && weight < weights_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && weight < weights_end) {
        freq=frequencies;
        while (freq < frequencies_end && weight < weights_end) {
          *weight=polar(1.0, hPhase(*freq,hGeometricDelayFarField(ant,sky,distance))); // exp(HComplex(0.0,hPhase(*freq,hGeometricDelayFarField(ant,sky,distance))));
          ++weight; ++freq;
        };
        ant+=3;
      };
      sky+=3;
    };
  } else {
    while (sky < sky_end && weight < weights_end) {
      distance = hVectorLength(sky,sky+3);
      ant=antPositions;
      while (ant < ant_end && weight < weights_end) {
        freq=frequencies;
        while (freq < frequencies_end && weight < weights_end) {
          *weight=polar(1.0, hPhase(*freq,hGeometricDelayNearField(ant,sky,distance))); // exp(HComplex(0.0,hPhase(*freq,hGeometricDelayNearField(ant,sky,distance))));
          ++weight; ++freq;
        };
        ant+=3;
      };
      sky+=3;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Get pixel coordinates for given vector of world coordinates
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWorld2Pixel
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(pixel)()("Pixel coordinates")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(world)()("World coordinates")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (string)(refcode)()("reference code for coordinate system e.g. ``AZEL``,``J2000``, ...")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (string)(projection)()("the projection used e.g. ``SIN`, ``STG``, ...")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(refLong)()("reference value for longtitude (``CRVAL``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HNumber)(refLat)()("reference value for latitude (``CRVAL``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(incLon)()("incLon increment value for longtitude (``CDELT``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(incLat)()("incLon increment value for latitude (``CDELT``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HNumber)(refX)()("refX reference :math:`x`-pixel (``CRPIX``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_9 (HNumber)(refY)()("refY reference :math:`y`-pixel (``CRPIX``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter pixel, const Iter pixel_end,
                     const Iter world, const Iter world_end,
                     const string refcode, const string projection,
                     const HNumber refLong, const HNumber refLat,
                     const HNumber incLong, const HNumber incLat,
                     const HNumber refX, const HNumber refY
    )
{
  HInteger lenWorld = world_end - world;
  HInteger lenPixel = pixel_end - pixel;

  // Check input
  if (lenPixel != lenWorld)
  {
    throw PyCR::ValueError("Size of pixel and world coordinate vectors are not the same.");
    std::cerr<<"Error, in world2Pixel(): input and output vector not of same length."<<std::endl;
  }

  casa::MDirection::Types type;
  if (casa::MDirection::getType(type, refcode) != true)
  {
    throw PyCR::TypeError("Invalid input reference type.");
    std::cerr<<"Error, in world2Pixel(): input reference type invalid."<<std::endl;
  }

  casa::Projection::Type proj;
  proj=casa::Projection::type(static_cast<casa::String>(projection));
  if (proj==casa::Projection::N_PROJ)
  {
    throw PyCR::ValueError("Invalid projection type.");
    std::cerr<<"Error, in world2Pixel(): input projection type invalid."<<std::endl;
  }

  // Get spatial direction coordinate system
  casa::Matrix<casa::Double> xform(2,2);
  xform = 0.0; xform.diagonal() = 1.0;

  casa::DirectionCoordinate dir(type,
      proj,
      static_cast<casa::Double>(refLong),
      static_cast<casa::Double>(refLat),
      static_cast<casa::Double>(incLong),
      static_cast<casa::Double>(incLat),
      xform,
      static_cast<casa::Double>(refX),
      static_cast<casa::Double>(refY));

  // Get iterators
  Iter world_it=world;
  Iter pixel_it=pixel;

  // Placeholders for conversion
  casa::Vector<casa::Double> cworld(2), cpixel(2);

  // Loop over all world coordinates
  while (world_it!=world_end && pixel_it!=pixel_end)
  {
    // Get world coordinates into casa vector for conversion
    cworld[0]=static_cast<casa::Double>(*world_it);
    ++world_it;
    cworld[1]=static_cast<casa::Double>(*world_it);

    // Convert world to pixel coordinates
    dir.toPixel(cpixel, cworld);

    // Retrieve pixel coordinates
    *pixel_it=static_cast<HNumber>(cpixel[0]);
    ++pixel_it;
    *pixel_it=static_cast<HNumber>(cpixel[1]);

    // Next pixel
    ++pixel_it;
    ++world_it;
  }
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Get world coordinates for given vector of pixel coordinates.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPixel2World
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(world)()("World coordinates")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(pixel)()("Pixel coordinates")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (string)(refcode)()("reference code for coordinate system e.g. ``AZEL``, ``J2000``, ...")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (string)(projection)()("the projection used e.g. ``SIN``, ``STG``, ...")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(refLong)()("reference value for longtitude (``CRVAL``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HNumber)(refLat)()("reference value for latitude (``CRVAL``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(incLon)()("incLon increment value for longtitude (``CDELT``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(incLat)()("incLon increment value for latitude (``CDELT``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HNumber)(refX)()("refX reference :math:`x`- pixel (``CRPIX``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_9 (HNumber)(refY)()("refY reference :math:`y`-pixel (``CRPIX``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_10 (HNumber)(lonPole)()("(``LONPOLE``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_11 (HNumber)(latPole)()("(``LATPOLE``)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter world, const Iter world_end,
                     const Iter pixel, const Iter pixel_end,
                     const string refcode, const string projection,
                     const HNumber refLong, const HNumber refLat,
                     const HNumber incLong, const HNumber incLat,
                     const HNumber refX, const HNumber refY,
                     const HNumber lonPole, const HNumber latPole
    )
{
  HInteger lenWorld = world_end - world;
  HInteger lenPixel = pixel_end - pixel;

  // Check input
  if (lenPixel != lenWorld) {
    throw PyCR::ValueError("Size of pixel and world coordinate vectors are not the same.");
    std::cerr<<"Error, in hPixel2World(): input and output vector not of same length."<<std::endl;
  }

  casa::MDirection::Types type;
  if (casa::MDirection::getType(type, refcode) != true)
  {
    throw PyCR::TypeError("Invalid input reference type.");
    std::cerr<<"Error, in hPixel2World(): input reference type invalid."<<std::endl;
  }

  casa::Projection::Type proj;
  proj=casa::Projection::type(static_cast<casa::String>(projection));
  if (proj==casa::Projection::N_PROJ)
  {
    throw PyCR::ValueError("Invalid projection type.");
    std::cerr<<"Error, in hPixel2World(): input projection type invalid."<<std::endl;
  }

  // Get spatial direction coordinate system
  casa::Matrix<casa::Double> xform(2,2);
  xform = 0.0; xform.diagonal() = 1.0;

  casa::DirectionCoordinate dir(type,
      proj,
      static_cast<casa::Double>(refLong),
      static_cast<casa::Double>(refLat),
      static_cast<casa::Double>(incLong),
      static_cast<casa::Double>(incLat),
      xform,
      static_cast<casa::Double>(refX),
      static_cast<casa::Double>(refY),
      static_cast<casa::Double>(lonPole),
      static_cast<casa::Double>(latPole));

  // Get iterators
  Iter world_it=world;
  Iter pixel_it=pixel;

  // Placeholders for conversion
  casa::Vector<casa::Double> cworld(2), cpixel(2);

  // Loop over all pixels
  while (world_it!=world_end && pixel_it!=pixel_end)
  {
    // Get pixel coordinates into casa vector for conversion
    cpixel[0]=static_cast<casa::Double>(*pixel_it);
    ++pixel_it;
    cpixel[1]=static_cast<casa::Double>(*pixel_it);

    // Convert pixel to world coordinates
    dir.toWorld(cworld, cpixel);

    // Retrieve world coordinates
    *world_it=static_cast<HNumber>(cworld[0]);
    ++world_it;
    *world_it=static_cast<HNumber>(cworld[1]);

    // Next pixel
    ++pixel_it;
    ++world_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Convert array of equatorial J2000 coordinates to horizontal, AZEL coordinates.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hEquatorial2Horizontal
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(hc)()("array with horizontal coordiates [radians] ``(alt, az, alt, az, ...)`` altitude positive eastwards from north.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(ec)()("array with equatorial coordinates [radians] ``(ra, dec, ra, dec, ...)``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(utc)()("UTC as Julian Day.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(ut1_utc)()("difference UT1-UTC (as obtained from IERS bulletin A) if 0 a maximum error of 0.9 seconds is made.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(L)()("longitude of telescope [radians]. ``L`` is observer's longitude (positive east, negative west of Greenwhich).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HNumber)(phi)()("latitude of telescope [radians].")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter hc, const Iter hc_end,
                     const Iter ec, const Iter ec_end,
                     const HNumber utc, const HNumber ut1_utc,
                     const HNumber L, const HNumber phi)
{
  // Variables
  HNumber alpha, delta, A, h, H;

  // Calculate Terestrial Time (TT)
  const HNumber tt = utc + tmf_delta_tt_utc(utc) / SECONDS_PER_DAY;

  // Calculate Universal Time (UT1)
  const HNumber ut1 = utc + ut1_utc / SECONDS_PER_DAY;

  // Calculate Local Apparant Sidereal Time (LAST)
  const HNumber theta_L = tmf_last(ut1, tt, L);

  // Get iterators
  Iter hc_it=hc;
  Iter ec_it=ec;

  // Sanity check
  int lenHC = std::distance(hc,hc_end);
  const int lenEC = std::distance(ec,ec_end);
  if ((lenHC <= 0) && (lenHC%2 != 0)) {
    char error_message[256];
    sprintf(error_message, "Incorrect size of horizontal coordinates vector (len [=%d] <= 0, or not even).", lenHC);
    throw PyCR::ValueError(error_message);
    return;
  }
  if ((lenEC <= 0) && (lenEC%2 != 0)) {
    char error_message[256];
    sprintf(error_message, "Incorrect size of equatorial coordinates vector (len [=%d] <= 0, or not even).",lenEC);
    throw PyCR::ValueError(error_message);
    return;
  }
  if (lenHC != lenEC) {
    throw PyCR::ValueError("Size of horizontal and equatorial coordinates vectors should be the same.");
    return;
  }

  while (hc_it!=hc_end && ec_it!=ec_end)
  {
    // Get equatorial coordinates
    alpha = *ec_it;
    ++ec_it;
    delta = *ec_it;

    // Calculate hour angle
    H = tmf_rad2circle(theta_L - alpha);

    // Convert from equatorial to horizontal coordinates
    tmf_equatorial2horizontal(&A, &h, H, delta, phi);

    // Store output
    *hc_it = A;
    ++hc_it;
    *hc_it = h;

    // Advance iterator
    ++hc_it;
    ++ec_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Beamform image
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBeamformImage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(image)()("Array to store resulting image. Stored as ``[I(x_0, y_0, f_0), I(x_0, y_0, f_1), ... I(x_nx, y_ny, f_nf)]`` e.g. the rightmost (frequency) index runs fastest. This array may contain an existing image.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Array with FFT data of each antenna. Expects data to be stored as ``[f(0,0), f(0,1), ..., f(0,nf), f(1,0), f(1,1), ..., f(1,nf), ..., f(na, 0), f(na,1), ..., f(na,nf)]`` e.g. ``f(i,j)`` where ``i`` is the antenna number and ``j`` is the frequency.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Array with frequencies [Hz] stored as ``[f_0, f_1, ..., f_n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(antpos)()("Array with antenna positions in the local Cartesian frame [meter]. Stored as ``[x_0, y_0, z_0, x_1, y_1, z_1, ... x_na, y_na, z_na]`` where ``na`` is the number of antennas.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(skypos)()("Array with sky positions in the local cartesian frame [Hz]. Stored as ``[x_0, y_0, z_0, x_1, y_1, z_1, ... x_np, y_np, z_np]`` where ``np`` is the number of pixels in the image. Storage order for the pixels is the same as for the image e.g. runs faster in ``y``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class Iter>
void HFPP_FUNC_NAME (const CIter image, const CIter image_end,
    const CIter fftdata, const CIter fftdata_end,
    const Iter frequencies, const Iter frequencies_end,
    const Iter antpos, const Iter antpos_end,
    const Iter skypos, const Iter skypos_end
    )
{
  // Variables
  HNumber norm = 0.0;
  HNumber delay = 0.0;

  // Inspect length of input arrays
  const int Nimage = std::distance(image, image_end);
  const int Nfftdata = std::distance(fftdata, fftdata_end);
  const int Nfrequencies = std::distance(frequencies, frequencies_end);
  const int Nantpos = std::distance(antpos, antpos_end);
  const int Nskypos = std::distance(skypos, skypos_end);

  // Get relevant numbers
  const int Nantennas = Nantpos / 3;
  const int Nskycoord = Nskypos / 3;

  // Indices
  int i, j, k;

  // Sanity checks
  if (Nantpos != Nantennas * 3)
  {
    char error_message[256];
    sprintf(error_message, "Antenna positions array has wrong size: Nantpos [=%d] != 3 x Nantennas [=%d].", Nantpos, Nantennas);
    throw PyCR::ValueError(error_message);
  }
  if (Nskypos != Nskycoord * 3)
  {
    char error_message[256];
    sprintf(error_message, "Sky positions array has wrong size: Nskypos [=%d] != 3 x Nskycoord [=%d].", Nskypos, Nskycoord);
    throw PyCR::ValueError(error_message);
  }
  if (Nfftdata != Nfrequencies * Nantennas)
  {
    char error_message[256];
    sprintf(error_message, "FFT data array has wrong size: Nfftdata[=%d] != Nfrequencies[=%d] * Nantennas[=%d]", Nfftdata, Nfrequencies, Nantennas);
    throw PyCR::ValueError(error_message);
  }
  if (Nimage != Nskycoord * Nfrequencies)
  {
    char error_message[256];
    sprintf(error_message, "Image array has wrong size: Nimage[=%d] != Nskycoord[=%d] * Nfrequencies[=%d]", Nimage, Nskycoord, Nfrequencies);
    throw PyCR::ValueError(error_message);
  }

  // Get iterators
  CIter it_im = image;
  CIter it_im_inner = image;
  CIter it_fft = fftdata;
  Iter it_freq = frequencies;
  Iter it_ant = antpos;
  Iter it_sky = skypos;

  clock_t start = clock(), diff;

  // Loop over pixels (parallel on multi core systems if supported)
#ifdef _OPENMP
  std::cout<<"Running in parallel mode"<<std::endl;
  #pragma omp parallel for private(delay, norm, it_im, it_im_inner, it_fft, it_freq, it_ant, it_sky, j, k)
#else
  std::cout<<"Running in serial mode"<<std::endl;
#endif // _OPENMP
  for (i=0; i<Nskycoord; ++i)
  {
    // Image iterator to start position
    it_im = image + (i * Nfrequencies);

    // Sky coordinate iterator to start position
    it_sky = skypos + (i * 3);

    // Loop over antennas
    it_ant = antpos;
    it_fft = fftdata;

    // Calculate norm of sky vector
    norm = sqrt(*it_sky * *it_sky +
                *(it_sky+1) * *(it_sky+1) +
                *(it_sky+2) * *(it_sky+2));

    for (j=Nantennas; j!=0; --j)
    {
      // Reset image iterator to first frequency of current pixel
      it_im_inner = it_im;

      // Calculate geometric delay
      delay = hGeometricDelayFarField(it_ant, it_sky, norm);

      // Loop over frequencies
      it_freq = frequencies;

      for (k=Nfrequencies; k!=0; --k)
      {
        // Multiply by geometric weight and add to image
        *it_im_inner += (*it_fft) * polar(1.0, (2*M_PI)*((*it_freq) * delay));
        ++it_im_inner;
        ++it_fft;
        ++it_freq;
      }

      // Next antenna position
      it_ant += 3;
    }
  }

  diff = clock() - start;

  std::cout<<"beamforming block done in "<< static_cast<float>(diff) / CLOCKS_PER_SEC<<" s"<<std::endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Beamform image
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBeamformImage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(image)()("Array to store resulting image. Stored as ``[I(x_0, y_0, f_0), I(x_0, y_0, f_1), ... I(x_nx, y_ny, f_nf)]``, e.g. the rightmost (frequency) index runs fastest. This array may contain an existing image.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Array with FFT data of each antenna. Expects data to be stored as ``[f(0,0), f(0,1), ..., f(0,nf), f(1,0), f(1,1), ..., f(1,nf), ..., f(na, 0), f(na,1), ..., f(na,nf)]`` e.g. ``f(i,j)`` where ``i`` is the antenna number and ``j`` is the frequency.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Array with frequencies [Hz] stored as ``[f_0, f_1, ..., f_n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(delays)()("Array containing the delays [s] for all antennas and positions (antenna index runs fastest: ``(ant1, pos1), (ant2, pos1), ...``) - length of vector has to be number of antennas times positions as calculated by :func:`hGeometricDelays`.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class Iter>
void HFPP_FUNC_NAME (const CIter image, const CIter image_end,
    const CIter fftdata, const CIter fftdata_end,
    const Iter frequencies, const Iter frequencies_end,
    const Iter delays, const Iter delays_end
    )
{
  // Inspect length of input arrays
  const int Nimage = std::distance(image, image_end);
  const int Nfftdata = std::distance(fftdata, fftdata_end);
  const int Nfrequencies = std::distance(frequencies, frequencies_end);
  const int Ndelays = std::distance(delays, delays_end);

  // Get relevant numbers
  const int Nskycoord = Nimage / Nfrequencies;
  const int Nantennas = Ndelays / Nskycoord;

  // Indices
  int i, j, k;

  // Sanity checks
  if (Nimage != Nskycoord * Nfrequencies)
  {
    char error_message[256];
    sprintf(error_message, "Image array has wrong size: Nimage[=%d] != Nskycoord[=%d] * Nfrequencies[=%d]", Nimage, Nskycoord, Nfrequencies);
    throw PyCR::ValueError(error_message);
  }
  if (Ndelays != Nantennas * Nskycoord)
  {
    char error_message[256];
    sprintf(error_message, "Delays array has wrong size: Ndelays[=%d] != Nantennas[=%d] * Nskycoord[=%d]", Ndelays, Nantennas, Nskycoord);
    throw PyCR::ValueError(error_message);
  }
  if (Nfftdata != Nfrequencies * Nantennas)
  {
    char error_message[256];
    sprintf(error_message,"FFT data array has wrong size: Nfftdata[=%d] != Nfrequencies[=%d] * Nantennas[=%d]", Nfftdata, Nfrequencies, Nantennas);
    throw PyCR::ValueError(error_message);
  }

  // Get iterators
  CIter it_im = image;
  CIter it_im_inner = image;
  CIter it_fft = fftdata;
  Iter it_freq = frequencies;
  Iter it_delays = delays;

  clock_t start = clock(), diff;

  // Loop over pixels (parallel on multi core systems if supported)
#ifdef _OPENMP
  std::cout<<"Running in parallel mode"<<std::endl;
  #pragma omp parallel for private(it_im, it_im_inner, it_fft, it_freq, it_delays, j, k)
#else
  std::cout<<"Running in serial mode"<<std::endl;
#endif // _OPENMP
  for (i=0; i<Nskycoord; ++i)
  {
    // Image iterator to start position
    it_im = image + (i * Nfrequencies);

    // Delay iterator to start position
    it_delays = delays + (i * Nantennas);

    // Loop over antennas
    it_fft = fftdata;

    for (j=Nantennas; j!=0; --j)
    {
      // Reset image iterator to first frequency of current pixel
      it_im_inner = it_im;

      // Loop over frequencies
      it_freq = frequencies;

      for (k=Nfrequencies; k!=0; --k)
      {
        // Multiply by geometric weight and add to image
        *it_im_inner += (*it_fft) * polar(1.0, (2*M_PI)*((*it_freq) * (*it_delays)));
        ++it_im_inner;
        ++it_fft;
        ++it_freq;
      }

      // Next antenna
      ++it_delays;
    }
  }

  diff = clock() - start;

  std::cout<<"beamforming block done in "<< static_cast<float>(diff) / CLOCKS_PER_SEC<<" s"<<std::endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Beamform image
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBeamformImage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(image)()("Array to store resulting image. Stored as ``[I(x_0, y_0, f_0), I(x_0, y_0, f_1), ... I(x_nx, y_ny, f_nf)]`` e.g. the rightmost (frequency) index runs fastest. This array may contain an existing image.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Array with FFT data of each antenna. Expects data to be stored as ``[f(0,0), f(0,1), ..., f(0,nf), f(1,0), f(1,1), ..., f(1,nf), ..., f(na, 0), f(na,1), ..., f(na,nf)]`` e.g. ``f(i,j)`` where ``i`` is the antenna number and ``j`` is the frequency.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Array with frequencies [Hz] stored as ``[f_0, f_1, ..., f_n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(antpos)()("Array with antenna positions in the local Cartesian frame [meter]. Stored as ``[x_0, y_0, z_0, x_1, y_1, z_1, ... x_na, y_na, z_na]`` where ``na`` is the number of antennas.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(skypos)()("Array with sky positions in the local cartesian frame [Hz]. Stored as ``[x_0, y_0, z_0, x_1, y_1, z_1, ... x_np, y_np, z_np]`` where ``np`` is the number of pixels in the image. Storage order for the pixels is the same as for the image e.g. runs faster in ``y``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HInteger)(step)()("Array with steps to take for stepping though the frequency channels (used to efficiently skip bad channels).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class NIter, class IIter>
void HFPP_FUNC_NAME (const CIter image, const CIter image_end,
    const CIter fftdata, const CIter fftdata_end,
    const NIter frequencies, const NIter frequencies_end,
    const NIter antpos, const NIter antpos_end,
    const NIter skypos, const NIter skypos_end,
    const IIter step, const IIter step_end
    )
{
  // Variables
  HNumber norm = 0.0;
  HNumber delay = 0.0;

  // Inspect length of input arrays
  const int Nimage = std::distance(image, image_end);
  const int Nfftdata = std::distance(fftdata, fftdata_end);
  const int Nfrequencies = std::distance(frequencies, frequencies_end);
  const int Nantpos = std::distance(antpos, antpos_end);
  const int Nskypos = std::distance(skypos, skypos_end);
  const int Nstep = std::distance(step, step_end);

  // Get relevant numbers
  const int Nantennas = Nantpos / 3;
  const int Nskycoord = Nskypos / 3;

  // Indices
  int i, j, k;

  // Sanity checks
  if (Nantpos != Nantennas * 3)
  {
    throw PyCR::ValueError("Antenna positions array has wrong size.");
  }
  if (Nskypos != Nskycoord * 3)
  {
    throw PyCR::ValueError("Sky positions array has wrong size.");
  }
  if (Nfftdata != Nfrequencies * Nantennas)
  {
    throw PyCR::ValueError("FFT data array has wrong size.");
  }
  if (Nimage != Nskycoord * Nfrequencies)
  {
    throw PyCR::ValueError("Image array has wrong size.");
  }

  // Get iterators
  CIter it_im = image;
  CIter it_im_inner = image;
  CIter it_fft = fftdata;
  CIter it_fft_start = fftdata;
  NIter it_freq = frequencies;
  NIter it_ant = antpos;
  NIter it_sky = skypos;
  IIter it_step = step;

  // Loop over pixels (parallel on multi core systems if supported)
#ifdef _OPENMP
  std::cout<<"Running in parallel mode"<<std::endl;
  #pragma omp parallel for private(delay, norm, it_im, it_im_inner, it_fft, it_fft_start, it_freq, it_ant, it_sky, it_step, j, k)
#else
  std::cout<<"Running in serial mode"<<std::endl;
#endif // _OPENMP
  for (i=0; i<Nskycoord; ++i)
  {
    // Image iterator to start position
    it_im = image + (i * Nfrequencies);

    // Sky coordinate iterator to start position
    it_sky = skypos + (i * 3);

    // Loop over antennas
    it_fft_start = fftdata;
    it_fft = fftdata;
    it_ant = antpos;

    for (j=Nantennas; j!=0; --j)
    {
      // Reset image iterator to first frequency of current pixel
      it_im_inner = it_im;

      // Calculate norm of sky vector
      norm = sqrt(*it_sky * *it_sky +
                  *(it_sky+1) * *(it_sky+1) +
                  *(it_sky+2) * *(it_sky+2));

      // Calculate geometric delay
      delay = hGeometricDelayFarField(it_ant, it_sky, norm);

      // Loop over frequencies
      it_fft = it_fft_start;
      it_freq = frequencies;
      it_step = step;

      for (k=Nstep; k!=0; --k)
      {
        // Go to next frequeny channel
        it_im_inner += *it_step;
        it_fft += *it_step;
        it_freq += *it_step;

        // Multiply by geometric weight and add to image
        *it_im_inner += (*it_fft) * polar(1.0, (2*M_PI)*((*it_freq) * delay));

        // Set frequency channel step for next iteration
        ++it_step;
      }

      // Next antenna position
      it_ant += 3;

      // Shift FFT data start position to next antenna
      it_fft_start += Nfrequencies;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Generate shifts for dedispersion
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDedispersionShifts
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(shifts)()("Shifts.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(frequencies)()("Frequencies in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(reference)()("Reference frequency in Hz.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(dm)()("Dispersion Measure.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(dt)()("Time sampling step in seconds.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class IIter, class NIter>
void HFPP_FUNC_NAME (const IIter shifts, const IIter shifts_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber reference, const HNumber dm, const HNumber dt
    )
{
  // dispersion constant (in units of Hz^2 pc^-1 cm^3 s)
  const double k = 4.149e15;

  // inverse square of the reference frequency
  const double rs = 1. / (reference * reference);

  const int Nshifts = std::distance(shifts, shifts_end);
  const int Nfrequencies = std::distance(frequencies, frequencies_end);

  if (Nshifts != Nfrequencies)
  {
    throw PyCR::ValueError("Vector sizes do not match.");
  }

  IIter shifts_it = shifts;
  NIter freq_it = frequencies;

  for (int i=Nshifts; i!=0; --i)
  {
    // Calculate shift in sampling units and round to the nearest integer
    *shifts_it = floor((k * dm / dt) * ((1. / ((*freq_it) * (*freq_it))) - rs) + 0.5);

    ++shifts_it;
    ++freq_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Beamform block
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBeamformBlock
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(out)()("Beamformer output.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Array with FFT data of each antenna. Expects data to be stored as ``[f(0,0), f(0,1), ..., f(0,nf), f(1,0), f(1,1), ..., f(1,nf), ..., f(na, 0), f(na,1), ..., f(na,nf)]`` e.g. ``f(i,j)`` where ``i`` is the antenna number and ``j`` is the frequency.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Array with frequencies [Hz] stored as ``[f_0, f_1, ..., f_n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(antpos)()("Array with antenna positions in the local Cartesian frame [meter]. Stored as ``[x_0, y_0, z_0, x_1, y_1, z_1, ... x_na, y_na, z_na]`` where ``na`` is the number of antennas.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(skypos)()("Array with sky position in the local cartesian frame [Hz]. Stored as ``[x, y, z]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class Iter>
void HFPP_FUNC_NAME (const CIter out, const CIter out_end,
    const CIter fftdata, const CIter fftdata_end,
    const Iter frequencies, const Iter frequencies_end,
    const Iter antpos, const Iter antpos_end,
    const Iter skypos, const Iter skypos_end
    )
{
  // Variables
  HNumber delay = 0.0;
  HInteger i, j;

  // Inspect length of input arrays
  const int Nout = std::distance(out, out_end);
  const int Nfftdata = std::distance(fftdata, fftdata_end);
  const int Nfrequencies = std::distance(frequencies, frequencies_end);
  const int Nantpos = std::distance(antpos, antpos_end);
  const int Nskypos = std::distance(skypos, skypos_end);

  // Get relevant numbers
  const int Nantennas = Nantpos / 3;

  // Sanity checks
  if (Nantpos != Nantennas * 3)
  {
    char error_message[256];
    sprintf(error_message, "Antenna positions array has wrong size: Nantpos [=%d] != 3 x Nantennas [=%d].", Nantpos, Nantennas);
    throw PyCR::ValueError(error_message);
  }
  if (Nskypos != 3)
  {
    char error_message[256];
    sprintf(error_message, "Sky positions array has wrong size: Nskypos [=%d] != 3", Nskypos);
    throw PyCR::ValueError(error_message);
  }
  if (Nfftdata != Nfrequencies * Nantennas)
  {
    char error_message[256];
    sprintf(error_message, "FFT data array has wrong size: Nfftdata[=%d] != Nfrequencies[=%d] * Nantennas[=%d]", Nfftdata, Nfrequencies, Nantennas);
    throw PyCR::ValueError(error_message);
  }
  if (Nout != Nfrequencies)
  {
    char error_message[256];
    sprintf(error_message, "Output array has wrong size: Nout[=%d] != Nfrequencies[=%d]", Nout, Nfrequencies);
    throw PyCR::ValueError(error_message);
  }

  // Get iterators
  CIter it_out = out;
  CIter it_fft = fftdata;
  Iter it_freq = frequencies;
  Iter it_ant = antpos;
  Iter it_sky = skypos;

  clock_t start = clock(), diff;

  // Calculate norm of sky vector
  const HNumber norm = sqrt(*it_sky * *it_sky + *(it_sky+1) * *(it_sky+1) + *(it_sky+2) * *(it_sky+2));

  i = Nantennas;
  while (i--)
  {
    delay = hGeometricDelayFarField(it_ant, it_sky, norm);

    it_out = out;
    it_freq = frequencies;

    j = Nfrequencies;
    while (j--)
    {
      *it_out += (*it_fft) * polar(1.0, (2*M_PI)*((*it_freq) * delay));
      it_fft++;
      it_out++;
      it_freq++;
    }

    it_ant += 3;
  }

  diff = clock() - start;

  std::cout<<"beamforming block done in "<< static_cast<float>(diff) / CLOCKS_PER_SEC<<" s"<<std::endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Beamform block
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBeamformBlock
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(out)()("Beamformer output.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Array with FFT data of each antenna. Expects data to be stored as ``[f(0,0), f(0,1), ..., f(0,nf), f(1,0), f(1,1), ..., f(1,nf), ..., f(na, 0), f(na,1), ..., f(na,nf)]`` e.g. ``f(i,j)`` where ``i`` is the antenna number and ``j`` is the frequency.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Array with frequencies [Hz] stored as ``[f_0, f_1, ..., f_n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(delays)()("Array containing the delays [s] for all antennas")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class Iter>
void HFPP_FUNC_NAME (const CIter out, const CIter out_end,
    const CIter fftdata, const CIter fftdata_end,
    const Iter frequencies, const Iter frequencies_end,
    const Iter delays, const Iter delays,
    )
{
  // Variables
  HInteger i, j;

  // Inspect length of input arrays
  const int Nout = std::distance(out, out_end);
  const int Nfftdata = std::distance(fftdata, fftdata_end);
  const int Nfrequencies = std::distance(frequencies, frequencies_end);

  // Get relevant numbers
  const int Nantennas = Ndelays;

  if (Nfftdata != Nfrequencies * Nantennas)
  {
    char error_message[256];
    sprintf(error_message, "FFT data array has wrong size: Nfftdata[=%d] != Nfrequencies[=%d] * Nantennas[=%d]", Nfftdata, Nfrequencies, Nantennas);
    throw PyCR::ValueError(error_message);
  }
  if (Nout != Nfrequencies)
  {
    char error_message[256];
    sprintf(error_message, "Output array has wrong size: Nout[=%d] != Nfrequencies[=%d]", Nout, Nfrequencies);
    throw PyCR::ValueError(error_message);
  }

  // Get iterators
  CIter it_out = out;
  CIter it_fft = fftdata;
  Iter it_freq = frequencies;
  Iter it_delay = delays;

  clock_t start = clock(), diff;

  i = Nantennas;
  while (i--)
  {
    it_out = out;
    it_freq = frequencies;

    j = Nfrequencies;
    while (j--)
    {
      *it_out += (*it_fft) * polar(1.0, (2*M_PI)*((*it_freq) * (*it_delay)));
      it_fft++;
      it_out++;
      it_freq++;
    }

    it_delay++;
  }

  diff = clock() - start;

  std::cout<<"beamforming block done in "<< static_cast<float>(diff) / CLOCKS_PER_SEC<<" s"<<std::endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#ifdef PYCRTOOLS_WITH_NUMPY

//$DOCSTRING: Calculates the square of the absolute value and add it to output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFrequencyIntegratedImage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (ndarray)(out)()("Numpy vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HComplex)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
*/
template <class Iter>
void HFPP_FUNC_NAME(ndarray out, const Iter in_begin, const Iter in_end)
{
  // Get pointers to memory of numpy array
  double* out_it = numpyBeginPtr<double>(out);
  double* out_end = numpyEndPtr<double>(out);

  const HInteger Nout = std::distance(out_it, out_end);
  const HInteger Nin = std::distance(in_begin, in_end);

  const HInteger Nf = Nin / Nout;

  Iter in_it = in_begin;

  for (HInteger i=0; i<Nout; i++)
  {
    for (HInteger j=0; j<Nf; j++)
    {
      *out_it += static_cast<double>(real(*in_it * conj(*in_it)));
      in_it++;
    }
    out_it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#endif /* PYCRTOOLS_WITH_NUMPY */