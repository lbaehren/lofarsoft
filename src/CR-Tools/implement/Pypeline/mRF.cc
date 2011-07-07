/**************************************************************************
 *  RF module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *  Heino Falcke <h.falcke@astro.ru.nl>                                   *
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
//$FILENAME: HFILE=mRF.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "math.h"
#include "mMath.h"
#include "mVector.h"
#include "mFilter.h"
#include "mRF.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: Functions for Radio Frequency data analysis
// ========================================================================

// ========================================================================
//
//  Wrapper functions
//
// ========================================================================

//$DOCSTRING: Calculates the direction of one source using the arrival times of the signal in three antennas returning the result in cartesian coordinates
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDirectionTriangulationCartesian
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(direction)()("3-Vector of X,Y,Z coordindates of the direction vector towards the source.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Error)()("Closure error - if non-zero then there was no valid solution.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(positions_1)()("3-Vector (x,y,z) with cartesian coordinates for the 1st antenna in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(positions_2)()("3-Vector (x,y,z) with cartesian coordinates for the 2nd antenna in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(positions_3)()("3-Vector (x,y,z) with cartesian coordinates for the 3rd antenna in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(time1)()("Arrival times of the signal at 1st antenna in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(time2)()("Arrival times of the signal at 2nd antenna in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(time3)()("Arrival times of the signal at 3rd antenna in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HInteger)(sign_of_solution)()("There can be one or two solutions returned: use +/-1 for choosing one of the two, or 0 for returning both (in AzElEr)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  ``hDirectionTriangulation(az,el,err,[x1,y1,z1],[x1,y1,z1],[x1,y1,z1],t1,t2,t3,+/-1) -> az,el, err``

  Description:

    Given three antenna positions, and (pulse) arrival times for each
    antenna, get a direction of arrival (az, el) assuming a source at
    infinity (plane wave).  From three antennas we get in general two
    solutions for the direction, unless the arrival times are out of
    bounds, i.e. larger than the light-time between two antennas. In
    the latter case a non-zero closure error is returned, which
    contains the magntiude of the error made.

    Usually, when the 3 antennas are in a more or less horizontal
    plane, one of the solutions will appear to come from below the
    horizon (el < 0) and can be discarded. With the parameter
    sign_of_solution one can choose either the positive or the
    negative solution.

    
    Output is returned in the scalar variables az, el, er. Typically
    this does not work in python, so this function should be called
    from within c++. In Python use hDirectionTriangulations, which
    returs the values in vectors.
  
  See also:

  hDirectionTriangulations

*/
#define SPEED_OF_LIGHT 299792458

template <class Iter>
void HFPP_FUNC_NAME(
                    const Iter direction, const Iter direction_end, 
                    HNumber & Error,
                    const Iter positions_1,const Iter positions_1_end,
                    const Iter positions_2,const Iter positions_2_end,
                    const Iter positions_3,const Iter positions_3_end,
                    const HNumber time1, const HNumber time2, const HNumber time3,   
                    const HInteger sign_of_solution
                    )

{

  static std::vector<HNumber> p1(3),p2(3),p3(3),xx(3),yy(3),zz(3);

  hCopy(p1.begin(),p1.end(),positions_1,positions_1_end);
  hCopy(p2.begin(),p2.end(),positions_2,positions_2_end);
  hCopy(p3.begin(),p3.end(),positions_3,positions_3_end);

  // First take by definition *positions_1 = 0, t1 = 0, use translation invariance of the result, t -> ct
  HNumber t2 = (time2 - time1) * SPEED_OF_LIGHT, t3 = (time3 - time1) * SPEED_OF_LIGHT;  //t1=0

  hSub(p2,p1); hSub(p3,p1); hFill(p1,0.0);

  // Now do a coordinate rotation such that z-coordinates z2, z3 are 0, and p2 is on the x-axis (i.e. y2 = 0).
  hDiv(xx,p2,hVectorLength(p2)); // make x-axis by normalizing p2

  // make y-axis by taking the part of p3 that is perpendicular to p2 (cf. Gram-Schmidt)
  hCopy(yy,p3); hMulAdd(yy,p2,-hMulSum(p2,p3)/hMulSum(p2,p2));  //    yy = p3 - p2 * (np.dot(p2, p3) / np.dot(p2, p2)) 
  hDiv(yy,hVectorLength(yy));
  
  // and make z-axis by the vector perpendicular to both p2 and p3.
  hCrossProduct(zz,xx,yy); hDiv(zz,hVectorLength(zz));

  // We use the fact that we preserve lengths and angles in the new coordinate system.

  // We now have to solve for A and B in:
  // t2 = A x2
  // t3 = A x3 + B y3
  // from the general t_i = A x_i + B y_i + C z_i and using our rotated coordinates.

  HNumber a = hMulSum(p3, xx);  //MulSum = Dot Product
  HNumber b = hMulSum(p3, yy);  //MulSum = Dot Product -> np.dot(p3, yy)

  HNumber A = - t2 / hVectorLength(p2);
  HNumber C,B = (1/b) * (-t3 - A * a);

    // Which gives the incoming-signal vector in the rotated coord. frame as (A, B, C).
    // C is free in this coord. system, but follows from imposing length 1 on the signal vector,
    // consistent with (ct)^2 = x^2 + y^2 + z^2. Without above coordinate rotation, the following nonlinear function is part of the system to be solved:

  HNumber square = A*A + B*B;
  
  if (square <= 1.0){
    C = sqrt(1.0 - square);
    Error = 0.0;
  } else {
    C = 0.0;
    Error = sqrt(square-1.0);
  };

// Note! TWO solutions, +/- sqrt(...). No constraints apart from
// elevation > 0 in the end result - otherwise both can apply.

// square>1 does happen because of floating point errors, when time delays
// 'exactly' match elevation = 0 so this correction is needed to
// ensure this function correctly inverts timeDelaysFromDirection(...)

    // Now we have to transform this vector back to normal x-y-z
    // coordinates. This is where the above xx, yy, zz vectors come
    // in (normal notation: x', y', z')

  //    XYZ(signal) =  A * xx + B * yy +/- C * zz

  hMul2(direction,direction_end,xx.begin(),xx.end(),A); hMulAdd(direction,direction_end,yy.begin(),yy.end(),B); hMulAdd(direction,direction_end,zz.begin(),zz.end(),C);

  if ((sign_of_solution>=0) && (*(direction+2) >= 0)) {
    HNumber r=hVectorLength(direction,direction_end);
    Error += abs(r-1.0);
    hDiv2(direction,direction_end,r); 
  } else {
    hMul2(direction,direction_end,xx.begin(),xx.end(),A); hMulAdd(direction,direction_end,yy.begin(),yy.end(),B); hMulAdd(direction,direction_end,zz.begin(),zz.end(),-C);
    HNumber r = hVectorLength(direction,direction_end);
    Error += abs(r-1.0);
    hDiv2(direction,direction_end,r); 
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the direction of one source using the arrival times of the signal in three antennas
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDirectionTriangulation
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(Azimuth)()("Azimuth of the source.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Elevation)()("Elevation of the source.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(Error)()("Closure error - if non-zero then there was no valid solution.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(positions_1)()("3-Vector (x,y,z) with cartesian coordinates for the 1st antenna in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(positions_2)()("3-Vector (x,y,z) with cartesian coordinates for the 2nd antenna in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(positions_3)()("3-Vector (x,y,z) with cartesian coordinates for the 3rd antenna in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HNumber)(time1)()("Arrival times of the signal at 1st antenna in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(time2)()("Arrival times of the signal at 2nd antenna in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HNumber)(time3)()("Arrival times of the signal at 3rd antenna in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_9 (HInteger)(sign_of_solution)()("There can be one or two solutions returned: use +/-1 for choosing one of the two, or 0 for returning both (in AzElEr)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  ``hDirectionTriangulation(az,el,err,[x1,y1,z1],[x1,y1,z1],[x1,y1,z1],t1,t2,t3,+/-1) -> az,el, err``

  Description:

    Given three antenna positions, and (pulse) arrival times for each
    antenna, get a direction of arrival (az, el) assuming a source at
    infinity (plane wave).  From three antennas we get in general two
    solutions for the direction, unless the arrival times are out of
    bounds, i.e. larger than the light-time between two antennas. In
    the latter case a non-zero closure error is returned, which
    contains the magntiude of the error made.

    Usually, when the 3 antennas are in a more or less horizontal
    plane, one of the solutions will appear to come from below the
    horizon (el < 0) and can be discarded. With the parameter
    sign_of_solution one can choose either the positive or the
    negative solution.

    
    Output is returned in the scalar variables az, el, er. Typically
    this does not work in python, so this function should be called
    from within c++. In Python use hDirectionTriangulations, which
    returs the values in vectors.
  
  See also:

  hDirectionTriangulations

*/

template <class Iter>
void HFPP_FUNC_NAME(
                    HNumber & Azimuth, HNumber & Elevation, HNumber & Error,
                    const Iter positions_1,const Iter positions_1_end,
                    const Iter positions_2,const Iter positions_2_end,
                    const Iter positions_3,const Iter positions_3_end,
                    const HNumber time1, const HNumber time2, const HNumber time3,   
                    const HInteger sign_of_solution
                    )

{

  static std::vector<HNumber> p1(3),p2(3),p3(3),xx(3),yy(3),zz(3),xyz(3);

  hCopy(p1.begin(),p1.end(),positions_1,positions_1_end);
  hCopy(p2.begin(),p2.end(),positions_2,positions_2_end);
  hCopy(p3.begin(),p3.end(),positions_3,positions_3_end);

  // First take by definition *positions_1 = 0, t1 = 0, use translation invariance of the result, t -> ct
  HNumber t2 = (time2 - time1) * SPEED_OF_LIGHT, t3 = (time3 - time1) * SPEED_OF_LIGHT;  //t1=0

  hSub(p2,p1); hSub(p3,p1); hFill(p1,0.0);

  // Now do a coordinate rotation such that z-coordinates z2, z3 are 0, and p2 is on the x-axis (i.e. y2 = 0).
  hDiv(xx,p2,hVectorLength(p2)); // make x-axis by normalizing p2

  // make y-axis by taking the part of p3 that is perpendicular to p2 (cf. Gram-Schmidt)
  hCopy(yy,p3); hMulAdd(yy,p2,-hMulSum(p2,p3)/hMulSum(p2,p2));  //    yy = p3 - p2 * (np.dot(p2, p3) / np.dot(p2, p2)) 
  hDiv(yy,hVectorLength(yy));
  
  // and make z-axis by the vector perpendicular to both p2 and p3.
  hCrossProduct(zz,xx,yy); hDiv(zz,hVectorLength(zz));

  // We use the fact that we preserve lengths and angles in the new coordinate system.

  // We now have to solve for A and B in:
  // t2 = A x2
  // t3 = A x3 + B y3
  // from the general t_i = A x_i + B y_i + C z_i and using our rotated coordinates.

  HNumber a = hMulSum(p3, xx);  //MulSum = Dot Product
  HNumber b = hMulSum(p3, yy);  //MulSum = Dot Product -> np.dot(p3, yy)

  HNumber A = - t2 / hVectorLength(p2);
  HNumber C,B = (1/b) * (-t3 - A * a);

    // Which gives the incoming-signal vector in the rotated coord. frame as (A, B, C).
    // C is free in this coord. system, but follows from imposing length 1 on the signal vector,
    // consistent with (ct)^2 = x^2 + y^2 + z^2. Without above coordinate rotation, the following nonlinear function is part of the system to be solved:

  HNumber square = A*A + B*B;
  
  if (square <= 1.0){
    C = sqrt(1.0 - square);
    Error = 0.0;
  } else {
    C = 0.0;
    Error = sqrt(square-1.0);
  };

// Note! TWO solutions, +/- sqrt(...). No constraints apart from
// elevation > 0 in the end result - otherwise both can apply.

// square>1 does happen because of floating point errors, when time delays
// 'exactly' match elevation = 0 so this correction is needed to
// ensure this function correctly inverts timeDelaysFromDirection(...)

    // Now we have to transform this vector back to normal x-y-z
    // coordinates, and then to (az, el) to get our direction.  This
    // is where the above xx, yy, zz vectors come in (normal notation:
    // x', y', z')

  //    signal =  A * xx + B * yy +/- C * zz

  HNumber theta, phi;

  hMul(xyz,xx,A); hMulAdd(xyz,yy,B); hMulAdd(xyz,zz,C);
  HNumber r = hVectorLength(xyz);    

  theta = M_PI_2 - acos(xyz[2]); // in fact, z/R with R = 1
  phi =  M_PI_2 - atan2(xyz[1], xyz[0]); // gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
  if (phi<0.0) phi += 2*M_PI; // to only get positive azmiuth values

  if (sign_of_solution>=0 && theta >= 0) {
    Error += abs(r-1.0);
    hDiv(xyz,r);
    Azimuth=phi; Elevation=theta;
  } else {
    hMul(xyz,xx,A); hMulAdd(xyz,yy,B); hMulAdd(xyz,zz,-C);
    r = hVectorLength(xyz);    
    Error += abs(r-1.0);
    hDiv(xyz,r);
    theta = M_PI_2 - acos(xyz[2]); // in fact, z/R with R = 1
    phi =  M_PI_2 - atan2(xyz[1], xyz[0]); // gets result in [-pi..pi] interval: add pi when needed to go to [0..2 pi]
    if (phi<0.0) phi += 2*M_PI; // to only get positive azmiuth values
    Azimuth=phi; Elevation=theta;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates all the directions of one source using the arrival times of the signal in all possible antenna triangles from a list of antennas as input 
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDirectionTriangulations
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(Azimuth)()("Vector containing the calculated azimuth directions towards the source for all triangles")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Elevation)()("Vector containing the calculated elevation directions towards the source for all triangles")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(errors)()("Vector with closure errors for each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(origins)()("Vector of 3-tuples (x,y,z) with cartesian coordinates of the center point of each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(weights)()("Vector with weights indicating the estimated position accuracy of each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(positions)()("Vector of 3-tuples with cartesian coordinates for all antennas in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HNumber)(times)()("Vector with arrival times of the signal at each antenna in seconds")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_7 (HInteger)(sign_of_solution)()("There can be one or two solutions returned: use +/-1 for choosing one of the two")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  ``hDirectionTriangulations([az],[el],[err],[origins],[weights],[positions],[times],+/-1) -> az,el,err,origins,weights``

  Description:

    This function calculates a list of arrival directions (azimuth and
    elevation) of a pulse/source from all possible triangles of
    (groups of three) antennas and the measured arrival times at these
    antennas.

    For the direction finding it is assumed that the signal arrives as
    a plane wave. If one triangle of antennas does not give a valid
    solution a solution at the horizion is returned with the complex
    part of the solution (the "closure error") returned in *err*.

    To cope with near-field sources for each triangle the coordinates
    of its center are also calculated. This allows one to later look
    for points where the dircetions from all triangles intercept.

    The inverse of the average lengths of the triangle baselines are
    returned in *weights* giving some indication how accurate the
    measurement in principle can be.


    From three antennas we get in general two solutions for the
    direction, unless the arrival times are out of bounds, i.e. larger
    than the light-time between two antennas.  Usually, when the 3
    antennas are in a more or less horizontal plane, one of the
    solutions will appear to come from below the horizon (el < 0) and
    can be discarded. With *sign_of_solution* one can pick either the
    positive of the negative elevation.

    As input one only needs to provide arrival times and a vector with
    the corresponding antenna positions x,y,z (Cartesian coordinates)
    as a 3-tuple.

    If N triangles are provided then the *azimuth*, *elevation*,
    *origins*, and *weights* vectors have N*(N-1)*(N-2)/6 results
    (times number of components of each result, i.e. times three for
    *origins* and times one for the rest.)

    
    Output: ``[az1,az2,...], [el1,el2,...], [err1,err2,...],[(x1,y1,z1),...],[weight1, weights2, ....])`` in radians for azimuth and elevation.

  See also:
  
  hDirectionTriangulation

  Example:

  file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
  file["ANTENNA_SET"]="LBA_OUTER"
  file["BLOCKSIZE"]=2**17
  oddantennas=["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]

  file["SELECTED_DIPOLES"]=oddantennas
  positions=file["ANTENNA_POSITIONS"]
  timeseries_data=file["TIMESERIES_DATA"]
  (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
  start-=16; end=start+int(2**ceil(log(end-start,2)));  
  timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])

  timeseries_data_cut[...].copy(timeseries_data[...,start:end])
  timeseries_data_cut.abs()
  timeseries_data_cut[...].runningaverage(7,hWEIGHTS.GAUSSIAN)
  mx=task("FitMaxima")(timeseries_data_cut,doplot=True,refant=0,sampleinterval=5,plotend=48,peak_width=10,legend=file["SELECTED_DIPOLES"])

  mx.lags *= 10**-9
  
  N=48
  NT=N*(N-1)*(N-2)/6 # Number of Triangles ....
  az=hArray(float,[NT])
  el=hArray(float,[NT])
  er=hArray(float,[NT])
  weights=hArray(float,[NT])
  origins=hArray(float,[NT,3])
  hDirectionTriangulations(az,el,er,origins,weights,positions[-N:],hArray(mx.lags[-N:]),+1);

  az/=deg; el/=deg;
  indx=hArray(int,[NT])
  ngood=indx.findlessthan(er,1e-10)

  scrt=hArray(float,[NT])
  scrt.copy(az,indx,ngood)
  scrt[0:ngood.val()].mean()
  #  -> Vector(float, 1, fill=[147.618])
  scrt.copy(el,indx,ngood)
  mean_el=scrt[0:ngood.val()].mean()
  rms_el=scrt[0:ngood.val()].stddev()

  ngood=indx.findbetween(scrt,mean_el-rms_el,mean_el+rms_el)
  scrt.copy(scrt,indx,ngood)

  az=hArray(float,[NT]);az[...].copy(azel[...,0])
  el=hArray(float,[NT]);el[...].copy(azel[...,1])
  plt.clf(); plt.plot(az,el,marker="o",linestyle='None')
  print "az =",az.vec().mean(),"+/-",az.vec().stddev(),"el =",el.vec().mean(),"+/-",el.vec().stddev()

  el. 86 -> NAN
  make sure +elev is always pos.

closure time: dt1 + dt2 -dt3

  azel -> hArray(float, [4L], fill=[-211.972,-83.0455,-211.822,83.0441]) # len=4 slice=[0:4])
  positions[-3] -> hArray(float, [48L, 3L], fill=[23.568,-27.952,-0.0004]) # len=144 slice=[135:138])
  positions[-2] -> hArray(float, [48L, 3L], fill=[12.106,-39.642,-0.003]) # len=144 slice=[138:141])
  positions[-1] -> hArray(float, [48L, 3L], fill=[-28.833,-25.937,-0.007]) # len=144 slice=[141:144])
  mx.lags[-3:] -> Vector(float, 3, fill=[-2.25e-08,-2.40625e-08,-1.0625e-08])

  import pycrtools.srcfind as srcfind
  p=np.array(positions[-3:].vec())
  t=np.array(mx.lags[-3:].vec())
  (az1,el1,az2,el2)=srcfind.directionFromThreeAntennas(p,t)
*/

template <class Iter>
void HFPP_FUNC_NAME(
                    const Iter Azimuth,const Iter Azimuth_end,
                    const Iter Elevation,const Iter Elevation_end,
                    const Iter Error,const Iter Error_end,
                    const Iter origins,const Iter origins_end,
                    const Iter weights,const Iter weights_end,
                    const Iter positions,const Iter positions_end,
                    const Iter times,const Iter times_end,
                    const HInteger sign_of_solution
                    )

{
  Iter az_it(Azimuth),el_it(Elevation),err_it(Error), origins_it(origins), weights_it(weights);
  Iter positions1_it(positions), positions2_it(positions+3), positions3_it(positions+6);
  Iter times1_it(times), times2_it(times+1), times3_it(times+2);
  Iter times_end_1(times_end-2);

  HInteger azlen = Azimuth_end-Azimuth;
  HInteger poslen = positions_end-positions;
  HInteger originslen = origins_end-origins;
  HInteger timeslen = times_end-times;

  if ((poslen % 3) !=0) ERROR_RETURN("Incorrect size of input vector 'positions' - not a mutliple of 3.");
  if (poslen < 9) ERROR_RETURN("Incorrect size of input vector 'positions' - less than 3 antennas.");
  if ((timeslen*3) != poslen) ERROR_RETURN("Incorrect size of input vector 'times' - does not match antenna position list.");
  if (azlen<0) ERROR_RETURN("Incorrect size of input vector 'Azimuth'.");
  if ((Elevation_end-Elevation)!=azlen) ERROR_RETURN("Incorrect size of input vector 'Elevation'.");
  if ((Error_end-Error)!=azlen) ERROR_RETURN("Incorrect size of input vector 'Error'.");
  if ((weights_end-weights)<0) ERROR_RETURN("Incorrect size of output vector 'weights'.");
  if (originslen<0) ERROR_RETURN("Incorrect size of output vector 'origins'.");
  if ((originslen % 3) !=0) ERROR_RETURN("Incorrect size of input vector 'origins' - not a mutliple of 3.");
 
  while (times1_it != times_end_1 && az_it != Azimuth_end){
    hDirectionTriangulation(*az_it,*el_it,*err_it,positions1_it,positions1_it+3, positions2_it,positions2_it+3, positions3_it,positions3_it+3,*times1_it,*times2_it,*times3_it,sign_of_solution);
    if (weights_it!=weights_end) {
      *weights_it = hVectorLength(positions1_it,positions1_it+3, positions2_it,positions2_it+3) +
        hVectorLength(positions2_it,positions2_it+3, positions3_it,positions3_it+3) +
        hVectorLength(positions3_it,positions3_it+3, positions1_it,positions1_it+3); 
      *weights_it = 1.0/(*weights_it);
      ++weights_it;
    };

    if (origins_it!=origins_end) {
      hAdd(origins_it,origins_it+3,positions1_it,positions1_it+3,positions2_it,positions2_it+3);
      hAdd(origins_it,origins_it+3,positions3_it,positions3_it+3);
      hDiv2(origins_it,origins_it+3,3.0);
      origins_it+=3;
    };

    ++az_it; ++el_it; ++err_it;
    ++times3_it; 
    positions3_it+=3; 
    
    if (times3_it==times_end) {
      ++times2_it; times3_it=times2_it+1;
      positions2_it+=3; positions3_it=positions2_it+3; 
      if (times3_it==times_end) {
        ++times1_it; times2_it=times1_it+1; times3_it=times2_it+1;
        positions1_it+=3; positions2_it=positions1_it+3;  positions3_it=positions2_it+3; 
      };
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates all the directions of one source using the arrival times of the signal in all possible antenna triangles from a list of antennas as input 
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDirectionTriangulationsCartesian
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(directions)()("Vector containing the calculated direction vectors in Cartesian coordinates (3 values: x,y,z) towards the source for all triangles")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(errors)()("Vector with closure errors for each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(origins)()("Vector of 3-tuples (x,y,z) with cartesian coordinates of the center point of each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(weights)()("Vector with weights indicating the estimated position accuracy of each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(positions)()("Vector of 3-tuples with cartesian coordinates for all antennas in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(times)()("Vector with arrival times of the signal at each antenna in seconds")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HInteger)(sign_of_solution)()("There can be one or two solutions returned: use +/-1 for choosing one of the two")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  ``hDirectionTriangulationsCartesian([directions],[errors],[origins],[weights],[positions],[times],+/-1) -> cartesian directions,err,origins,weights``

  Description:

    This function calculates a list of arrival directions in Cartesian
    coordinates of a pulse/source from all possible triangles of
    (groups of three) antennas and the measured arrival times at these
    antennas.

    For the direction finding it is assumed that the signal arrives as
    a plane wave. If one triangle of antennas does not give a valid
    solution a solution at the horizion is returned with the complex
    part of the solution (the "closure error") returned in *err*.

    To cope with near-field sources for each triangle the coordinates
    of its center are also calculated. This allows one to later look
    for points where the dircetions from all triangles intercept.

    The inverse of the average lengths of the triangle baselines are
    returned in *weights* giving some indication how accurate the
    measurement in principle can be.


    From three antennas we get in general two solutions for the
    direction, unless the arrival times are out of bounds, i.e. larger
    than the light-time between two antennas.  Usually, when the 3
    antennas are in a more or less horizontal plane, one of the
    solutions will appear to come from below the horizon (el < 0) and
    can be discarded. With *sign_of_solution* one can pick either the
    positive of the negative elevation.

    As input one only needs to provide arrival times and a vector with
    the corresponding antenna positions x,y,z (Cartesian coordinates)
    as a 3-tuple.

    If N triangles are provided then the *Directions*,
    *Origins*, and *weights* vectors have N*(N-1)*(N-2)/6 results
    (times number of components of each result, i.e. times three for
    *origins* and times one for the rest.)

    
    Output: ``[az1,az2,...], [el1,el2,...], [err1,err2,...],[(x1,y1,z1),...],[weight1, weights2, ....])`` in radians for azimuth and elevation.

  See also:
  
  hDirectionTriangulation,hDirectionTriangulationsCartesian

  Example:

  file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
  file["ANTENNA_SET"]="LBA_OUTER"
  file["BLOCKSIZE"]=2**17
  oddantennas=["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]

  file["SELECTED_DIPOLES"]=oddantennas
  positions=file["ANTENNA_POSITIONS"]
  timeseries_data=file["TIMESERIES_DATA"]
  (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
  start-=16; end=start+int(2**ceil(log(end-start,2)));  
  timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])

  timeseries_data_cut[...].copy(timeseries_data[...,start:end])
  timeseries_data_cut.abs()
  timeseries_data_cut[...].runningaverage(7,hWEIGHTS.GAUSSIAN)
  mx=task("FitMaxima")(timeseries_data_cut,doplot=True,refant=0,sampleinterval=5,plotend=48,peak_width=10,legend=file["SELECTED_DIPOLES"])

  mx.lags *= 10**-9

  N=48
  NT=N*(N-1)*(N-2)/6 # Number of Triangles ....
  directions=hArray(float,[NT,3])
  er=hArray(float,[NT])
  weights=hArray(float,[NT])
  origins=hArray(float,[NT,3])
  hDirectionTriangulationsCartesian(directions,er,origins,weights,positions[-N:],hArray(mx.lags[-N:]),+1);

  indx=hArray(int,[NT])
  ngood=indx.findlessthan(er,1e-10).val()

  scrt=hArray(float,[ngood,3])
  scrt.copyvec(directions,indx,ngood,3)
  meandirection=hArray(float,[3])
  meandirection.mean(scrt)
  meandirection /= meandirection.vectorlength().val()
  #meandirection - > hArray(float, [3L], fill=[0.0824118,-0.111287,0.990365]) # len=3 slice=[0:3])
  #pytmf.spherical2cartesian(1, pi2-81.7932*deg,pi2-143.4092*deg) -> (0.085090553169460376, -0.11461297030462134, 0.98975929639446536)
  sp=pytmf.cartesian2spherical(meandirection[0],meandirection[1],meandirection[2]); azel=(180-(sp[2]+pi2)/deg,90-(sp[1])/deg)
  # azel -> (143.47870717075114, 82.040161833968313)

*/

template <class Iter>
void HFPP_FUNC_NAME(
                    const Iter directions,const Iter directions_end,
                    const Iter error,const Iter error_end,
                    const Iter origins,const Iter origins_end,
                    const Iter weights,const Iter weights_end,
                    const Iter positions,const Iter positions_end,
                    const Iter times,const Iter times_end,
                    const HInteger sign_of_solution
                    )

{
  Iter dir_it(directions),dir_it_end(directions+3),err_it(error), origins_it(origins), weights_it(weights);
  Iter positions1_it(positions), positions2_it(positions+3), positions3_it(positions+6);
  Iter times1_it(times), times2_it(times+1), times3_it(times+2);
  Iter times_end_1(times_end-2);

  HInteger dirlen = directions_end-directions;
  HInteger poslen = positions_end-positions;
  HInteger originslen = origins_end-origins;
  HInteger timeslen = times_end-times;

  if (dirlen<0) ERROR_RETURN("Incorrect size of output vector 'directions'.");
  if ((dirlen % 3) !=0) ERROR_RETURN("Incorrect size of input vector 'directions' - not a mutliple of 3.");
  if ((poslen % 3) !=0) ERROR_RETURN("Incorrect size of input vector 'positions' - not a mutliple of 3.");
  if (poslen < 9) ERROR_RETURN("Incorrect size of input vector 'positions' - less than 3 antennas.");
  if ((timeslen*3) != poslen) ERROR_RETURN("Incorrect size of input vector 'times' - does not match antenna position list.");
  if ((error_end-error)!=dirlen/3) ERROR_RETURN("Incorrect size of input vector 'error'.");
  if ((weights_end-weights)<0) ERROR_RETURN("Incorrect size of output vector 'weights'.");
  if (originslen<0) ERROR_RETURN("Incorrect size of output vector 'origins'.");
  if ((originslen % 3) !=0) ERROR_RETURN("Incorrect size of input vector 'origins' - not a mutliple of 3.");
 
  while (times1_it != times_end_1 && dir_it != directions_end){
    hDirectionTriangulationCartesian(dir_it,dir_it_end,*err_it,positions1_it,positions1_it+3, positions2_it,positions2_it+3, positions3_it,positions3_it+3,*times1_it,*times2_it,*times3_it,sign_of_solution);
    if (weights_it!=weights_end) {
      *weights_it = hVectorLength(positions1_it,positions1_it+3, positions2_it,positions2_it+3) +
        hVectorLength(positions2_it,positions2_it+3, positions3_it,positions3_it+3) +
        hVectorLength(positions3_it,positions3_it+3, positions1_it,positions1_it+3); 
      *weights_it = 1.0/(*weights_it);
      ++weights_it;
    };

    if (origins_it!=origins_end) {
      hAdd(origins_it,origins_it+3,positions1_it,positions1_it+3,positions2_it,positions2_it+3);
      hAdd(origins_it,origins_it+3,positions3_it,positions3_it+3);
      hDiv2(origins_it,origins_it+3,3.0);
      origins_it+=3;
    };

    dir_it=dir_it_end; dir_it_end+=3; ++err_it;
    ++times3_it; 
    positions3_it+=3; 
    
    if (times3_it==times_end) {
      ++times2_it; times3_it=times2_it+1;
      positions2_it+=3; positions3_it=positions2_it+3; 
      if (times3_it==times_end) {
        ++times1_it; times2_it=times1_it+1; times3_it=times2_it+1;
        positions1_it+=3; positions2_it=positions1_it+3;  positions3_it=positions2_it+3; 
      };
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Calculates the point of closest approach of two lines in 3D as distance along the first input vector. Multiple lines can be given.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSkewLinesDistanceToClosestApproach
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(distance)()("Distance of point of closest approach along first line from origin of first line (in Meters).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(baseline)()("Baseline (separation) between the two points of origin (in Meters) - output value.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(origin1)()("3-Vector of Cartesian (X,Y,Z) coordindates of the point of origin of the first lines(normalized) direction vector of the first line.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(direction1)()("3-Vector of Cartesian (X,Y,Z) coordindates of the (normalized) direction vector of the first line.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(origin2)()("3-Vector of Cartesian (X,Y,Z) coordindates of the point of origin of the first lines(normalized) direction vector of the second line.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(direction2)()("3-Vector of Cartesian (X,Y,Z) coordindates of the (normalized) direction vector of the second line.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HNumber)(max_distance)()("Maximum allowed distance to return - is also used to voids divide by zero.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  ``hSkewLinesDistanceToClosestApproach(distance,baseline,origin1,direction1,origin2,direction2,max_distance)`` -> distance, baseline 

  Description:

  Typically three-dimensional lines do not intersect (skew lines),
  however, there is a point of closest approach. In this function four
  vectors are provided as input, which provide the points of origin and
  the normalized (length=1) direction vector of the two lines.
  ``distance``will then contain the distance from the origin of the
  first vector (``origin1``) along its direction vector (``direction1``)
  until the point of closest approach is reached. Per vector 3 Cartesian
  coordinates have to be provided, hence the length of the input vector
  has to be a mutliple of three.

  *Looping behaviour*: The four vectors can contain multiple vectors.
  Calculations proceed until the end of origin2 or direction2 are
  reached. If direction1 and origin1 are shorter they will be wrapped
  und taken repeatedly. For example, by providing a single vector in
  origins1, directions1 and multiple ones in origin2, direction2 one
  obtains a list of closest approach distances along the first vector
  realtive to all the other vectors.

  The Math is taken from  http://www.mathworks.com/matlabcentral/newsreader/view_thread/246420

  In general you can find the two points, A0 and B0, on the respective
  lines A1A2 and B1B2 which are closest together and determine if they
  coincide or else see how far apart they lie.

  The following is a specific formula using matlab for determining
  these closest points A0 and B0 in terms of vector cross products and
  dot products. Assume that all four points are represented by
  three-element column vectors or by three-element row vectors. Do the
  following:

  nA = dot(cross(B2-B1,A1-B1),cross(A2-A1,B2-B1));
  nB = dot(cross(A2-A1,A1-B1),cross(A2-A1,B2-B1));
  d = dot(cross(A2-A1,B2-B1),cross(A2-A1,B2-B1));
  A0 = A1 + (nA/d)*(A2-A1);
  B0 = B1 + (nB/d)*(B2-B1);

  Here we actually use a point of origin and a direction vector, which
  is ``direction1 = A2-A1`` and ``direction2 = B2-B1``. Also, we do not
  calculate nB.

  See also:

  hDirectionTriangulations,hDirectionTriangulationCartesian

  Example:

  #simple example first
  distance=hArray(float,[1])
  error_distance=hArray(float,[1])
  origin1=hArray([0.,0.,0.])
  direction1=hArray([0.,0.,1.])
  origin2=hArray([1.,1.,0.])
  direction2=hArray([-2.**-0.5,0.,2.**-0.5])
  hSkewLinesDistanceToClosestApproach(distance,error_distance,origin1,direction1,origin2,direction2,10.,1.);

  #more invovled example
  file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
  file["ANTENNA_SET"]="LBA_OUTER"
  file["BLOCKSIZE"]=2**17
  oddantennas=["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]

  file["SELECTED_DIPOLES"]=oddantennas
  positions=file["ANTENNA_POSITIONS"]
  timeseries_data=file["TIMESERIES_DATA"]
  (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
  start-=16; end=start+int(2**ceil(log(end-start,2)));  
  timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])

  timeseries_data_cut[...].copy(timeseries_data[...,start:end])
  timeseries_data_cut.abs()
  timeseries_data_cut[...].runningaverage(7,hWEIGHTS.GAUSSIAN)
  mx=task("FitMaxima")(timeseries_data_cut,doplot=True,refant=0,sampleinterval=5,plotend=48,peak_width=10,legend=file["SELECTED_DIPOLES"])

  mx.lags *= 10**-9

  N=48
  NT=N*(N-1)*(N-2)/6 # Number of Triangles ....
  directions=hArray(float,[NT,3])
  er=hArray(float,[NT])
  weights=hArray(float,[NT])
  origins=hArray(float,[NT,3])
  hDirectionTriangulationsCartesian(directions,er,origins,weights,positions[-N:],hArray(mx.lags[-N:]),+1);

  indx=hArray(int,[NT])
  ngood=indx.findlessthan(er,1e-10).val()

  scrt=hArray(float,[ngood,3])
  scrt.copyvec(directions,indx,ngood,3)
  meandirection=hArray(float,[3])
  meandirection.mean(scrt)
  meandirection /= meandirection.vectorlength().val()

  #meandirection - > hArray(float, [3L], fill=[0.0824118,-0.111287,0.990365]) # len=3 slice=[0:3])
  #pytmf.spherical2cartesian(1, pi2-81.7932*deg,pi2-143.4092*deg) -> (0.085090553169460376, -0.11461297030462134, 0.98975929639446536)
  sp=pytmf.cartesian2spherical(meandirection[0],meandirection[1],meandirection[2]); azel=(180-(sp[2]+pi2)/deg,90-(sp[1])/deg)
  # azel -> (143.47870717075114, 82.040161833968313)


  scrt.copyvec(origins,indx,ngood,3)
  meanorigin=hArray(float,[3])
  meanorigin.mean(scrt)


  distance=hArray(float,[NT])
  error_distance=hArray(float,[NT])

  hSkewLinesDistanceToClosestApproach(distance,error_distance,meanorigin,meandirection,origins,directions,10000.,100.);

  scrt2=hArray(float,[ngood])
  scrt2.copyvec(distance,indx,ngood,1)
*/

template <class Iter>
void HFPP_FUNC_NAME(
                    const Iter distance, const Iter distance_end, 
                    const Iter baseline, const Iter baseline_end, 
                    const Iter origin1, const Iter origin1_end, 
                    const Iter direction1, const Iter direction1_end, 
                    const Iter origin2, const Iter origin2_end, 
                    const Iter direction2, const Iter direction2_end,
                    const HNumber max_distance
                    )
{
  static std::vector<HNumber> v1_vector(3),v2_vector(3),offset_vector(3);
  typename vector<HNumber>::iterator v1(v1_vector.begin()),v1_end(v1_vector.end());
  typename vector<HNumber>::iterator v2(v2_vector.begin()),v2_end(v2_vector.end());
  typename vector<HNumber>::iterator offset(offset_vector.begin()),offset_end(offset_vector.end());

  typename vector<HNumber>::iterator it_origin1(origin1),it_origin1_end(origin1+3);
  typename vector<HNumber>::iterator it_origin2(origin2),it_origin2_end(origin2+3);
  typename vector<HNumber>::iterator it_direction2(direction2),it_direction2_end(direction2+3);
  typename vector<HNumber>::iterator it_direction1(direction1),it_direction1_end(direction1+3);
  typename vector<HNumber>::iterator it_baseline(baseline);
  typename vector<HNumber>::iterator it_distance(distance);

  if (origin1_end-origin1 < 3) ERROR_RETURN("Illegal size vector `origin1`.");
  if (direction1_end-direction1 < 3) ERROR_RETURN("Illegal size vector `direction1`.");
  if (origin2_end-origin2 < 3) ERROR_RETURN("Illegal size vector `origin2`.");
  if (direction2_end-direction2 < 3) ERROR_RETURN("Illegal size vector `direction2`.");
  if (baseline_end<=baseline) ERROR_RETURN("Illegal size vector `baseline`.");
  if (distance_end<=distance) ERROR_RETURN("Illegal size vector `distance`.");

  HNumber d,nA;

  while (it_origin2_end<=origin2_end && it_direction2_end<=direction2_end && it_baseline<baseline_end && it_distance<distance_end) {

    hSub(offset,offset_end,it_origin1,it_origin1_end,it_origin2,it_origin2_end);

    hCrossProduct(v1,v1_end,it_direction2,it_direction2_end,offset,offset_end);
    hCrossProduct(v2,v2_end,it_direction1,it_direction1_end,it_direction2,it_direction2_end);

    nA = hDotProduct(v1,v1_end,v2,v2_end); //nA = dot(cross(B2-B1,A1-B1),cross(A2-A1,B2-B1));

    hCrossProduct(v1,v1_end,it_direction1,it_direction1_end,it_direction2,it_direction2_end);
    d = hDotProduct(v1,v1_end,v2,v2_end); //dot(cross(A2-A1,B2-B1),cross(A2-A1,B2-B1));

    *it_baseline = hVectorLength(offset,offset_end);

    if (abs(d) < nA/max_distance) *it_distance=max_distance;
    else *it_distance=nA/d;

    it_direction1=it_direction1_end; 
    if (it_direction1_end>=direction1_end) it_direction1=direction1;
    it_direction1_end=it_direction1+3;

    it_origin1=it_origin1_end; 
    if (it_origin1_end>=origin1_end) it_origin1=origin1;
    it_origin1_end=it_origin1+3;

    it_direction2=it_direction2_end; it_direction2_end+=3;

    it_origin2=it_origin2_end; it_origin2_end+=3;

    ++it_baseline; ++it_distance;
  };                     
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


// //$DOCSTRING: Calculates all the directions of one source using a plane-wave fit to the arrival times of the signal in all possible antenna triangles from a list of antennas as input 
// //$COPY_TO HFILE START --------------------------------------------------
// #define HFPP_FUNC_NAME hPlaneWaveFit
// //-----------------------------------------------------------------------
// #define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
// #define HFPP_PARDEF_0 (HNumber)(directions)()("Vector containing the calculated direction vectors in Cartesian coordinates (3 values: x,y,z) towards the source for all triangles")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_1 (HNumber)(errors)()("Vector with closure errors for each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_2 (HNumber)(origins)()("Vector of 3-tuples (x,y,z) with cartesian coordinates of the center point of each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_3 (HNumber)(weights)()("Vector with weights indicating the estimated position accuracy of each triangle")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_4 (HNumber)(positions)()("Vector of 3-tuples with cartesian coordinates for all antennas in meters")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_5 (HNumber)(times)()("Vector with arrival times of the signal at each antenna in seconds")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
// #define HFPP_PARDEF_6 (HInteger)(sign_of_solution)()("There can be one or two solutions returned: use +/-1 for choosing one of the two")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
// //$COPY_TO END --------------------------------------------------
// /*!
//   \brief $DOCSTRING
//   $PARDOCSTRING

//   Usage:

//   ``hDirectionTriangulationsCartesian([directions],[errors],[origins],[weights],[positions],[times],+/-1) -> cartesian directions,err,origins,weights``

//   Description:


// {
// 	float P,Q,R,S,W,T1,S1,S2,S3,S4,S5,S6 ;
// 	P=Q=R=S=W=T1=S1=S2=S3=S4=S5=S6=0 ;
// 	int counter=0 ;
// 	for(int j=0;j<NO_DETs;j++)
// 	{
// 		weight[j]=1 ;		//Ignoring event weights
// 		if(cdt[j]>=0)
// 		{
// 			counter++ ;
// 			S=S+weight[j]*Det_X[j]*Det_X[j] ;
// 			W=W+weight[j]*Det_Y[j]*Det_Y[j] ;
// 			T1=T1+weight[j]*Det_X[j] ;
// 			S1=S1+weight[j]*Det_X[j]*cdt[j] ;
// 			S2=S2+weight[j]*Det_X[j]*Det_Y[j] ;
// 			S3=S3+weight[j]*Det_Y[j]*cdt[j] ;
// 			S4=S4+weight[j]*Det_Y[j] ;
// 			S5=S5+weight[j]*cdt[j] ;
// 			S6=S6+weight[j] ;
// 		}
// 	}
// 	P=(S1*S2)/S ;
// 	Q=(T1*S2)/S ;
// 	R=-((S2*S2)/S)+W ;

// 	float t0 ;
// 	//Here, "t0" is actually "ct0 in meters"
// 	t0=(T1*S1*R-R*S*S5+T1*P*S2-T1*S2*S3-S*S4*P+S*S4*S3)/(T1*Q*S2-T1*S2*S4+R*T1*T1-S*S4*Q+S*S4*S4-R*S*S6) ;
// 	m=(P-t0*Q-S3+t0*S4)/R ;
// 	l=(-S1/S)-((P*S2)/(R*S))+((t0*Q*S2)/(R*S))+((S2*S3)/(R*S))-((t0*S2*S4)/(R*S))+((t0*T1)/S) ;
// 	n=sqrt(1.0-(l*l+m*m)) ;

// 	THETA=(asin(sqrt(l*l+m*m)))*(180.0/pi) ;	//in degrees (from vertical direction +Z)
// 	PHI=(acos(m/sqrt(l*l+m*m)))*(180.0/pi) ;	//in degrees (Eastward from North)
// 	if(l<0) PHI=360.0-PHI ;

// 	THETA=90.0-THETA ;//in degrees (from the horizon)
// 	PHI=360.0-PHI ;//in degrees (Westward from North)

// 	//printf("THETA=%f\tPHI=%f\tl=%f\tm=%f\tn=%f\tct0=%f\n",THETA,PHI,l,m,n,t0) ;
// }


//$DOCSTRING: Calculates the square root of the power of a complex spectrum and adds it to an output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSpectralPower
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Input vector containing the complex spectrum. (Looping parameter)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  vecout.spectralpower(vecin) -> vecout_i=sqrt(real((vecin_i)*conj(vecin_i)))

  Description:

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an hArray) is here determined by
  the second parameter!
  
  See also:

  hSquareAdd, hSpectralPower2 

  Example:
  >>> spectrum=hArray(float,[1,128])
  >>> cplxfft=hArray(complex,[10,128],fill=1+0j)
  >>> spectrum[...].spectralpower(cplxfft[...])
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  // Declaration of variables
  Iterin itin(vecin);
  Iter itout(vecout);

  // Sanity check
  if ((vecin_end<vecin) || (vecout_end<vecout)) ERROR_RETURN("Incorrect size of input vector.");

  // Calculation of spectral power
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout+=sqrt(real((*itin)*conj(*itin)));
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the power of a complex spectrum and adds it to an output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSpectralPower2
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Input vector containing the complex spectrum. (Looping parameter)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  vecout.spectralpower2(vecin) -> vecout_i=real((vecin_i)*conj(vecin_i))

  Description:

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an hArray) is here determined by
  the second parameter!
  
  See also:

  hSquareAdd, hSpectralPower

  Example:
  >>> spectrum=hArray(float,[1,128])
  >>> cplxfft=hArray(complex,[10,128],fill=1+0j)
  >>> spectrum[...].spectralpower2(cplxfft[...])
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  // Declaration of variables
  Iterin itin(vecin);
  Iter itout(vecout);

  // Sanity check
  if ((vecin_end<vecin) || (vecout_end<vecout)) {
    throw PyCR::ValueError("Incorrect size of input vector.");
    return;
  }

  // Calculation of spectral power
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout+=real((*itin)*conj(*itin));
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

