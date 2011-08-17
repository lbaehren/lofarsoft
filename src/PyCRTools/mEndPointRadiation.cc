/*****************************************************************************
 *                                                                           *
 *  Endpoint radiation module for the CR Pipeline Python Bindings            *
 *                                                                           *
 *  Copyright (c) 2011                                                       *
 *                                                                           *
 *  Martin van den Akker <martinva@astro.ru.nl>                              *
 *                                                                           *
 *  This library is free software: you can redistribute it and/or modify it  *
 *  under the terms of the GNU General Public License as published by the    *
 *  Free Software Foundation, either version 3 of the License, or (at your   *
 *  option) any later version.                                               *
 *                                                                           *
 *  This library is distributed in the hope that it will be useful, but      *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of               *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *  General Public License for more details.                                 *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License along  *
 *  with this library. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                           *
 *****************************************************************************/


// ========================================================================
//
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mEndPointRadiation.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Included header files
//
// ========================================================================

// ________________________________________________________________________
//                                                     System include files


// ________________________________________________________________________
//                                                    Project include files

#include "core.h"
#include "mModule.h"
#include "mEndPointRadiation.h"
#include "mVector.h"
#include "mMath.h"

// ________________________________________________________________________
//                                                      Other include files



// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;

#undef HFPP_FILETYPE
// --------------------
#define HFPP_FILETYPE CC
// --------------------


//$DOCSTRING: Calculates the radiation produced by a charged particle being instantaneously accelerated or decelerated.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hEndPointRadiation
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(Ex)()("x- component of the complex electric field spectrum received at the observer location")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(Ey)()("y- component of the complex electric field spectrum received at the observer location")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HComplex)(Ez)()("z- component of the complex electric field spectrum received at the observer location")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(frequency)()("Vector with frequency values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(xq)()("Vector of particle location.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(vq)()("Vector of particle velocity.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HNumber)(t)()("time of particle at endpoint.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(n)()("refractive index at endpoint.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HNumber)(xobs)()("Vector of observer location.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_9 (HNumber)(acceleration)()("sign of acceleration -1=deceleration, +1=acceleration.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  hEndPointRadiation(Ex,Ey,Ez,frequency,x_q v_q,t,n,x_obs,acceleration) -> Ex,Ex,Ez = observed E-field components

  Description:

  Calculates the electric fields generated by a single acceleration or deceleration event, using the end point formulation.

  Example:

  >>> #plt.EDP64bug=True # use True if your system crashes for plotting semilog axes
  >>>
  >>> n_frequency_channels=1024;
  >>> frequencies=hArray(float,[n_frequency_channels],name="E-field (x-component)",units=('','Hz'))
  >>> frequencies.fillrange(10**8,10*6)
  >>> Ex=hArray(complex,[n_frequency_channels],name="E-field (x-component)",xvalues=frequencies)
  >>> Ey=hArray(complex,[n_frequency_channels],name="E-field (y-component)",xvalues=frequencies)
  >>> Ez=hArray(complex,[n_frequency_channels],name="E-field (z-component)",xvalues=frequencies)
  >>> total_power=hArray(float,[n_frequency_channels],name="Spectral Power",xvalues=frequencies,par=dict(logplot="y"))
  >>> xq=hArray(float, 3, [1.,1.,1.])
  >>> t=2.
  >>> n=1.
  >>> vq=hArray(float, 3, [1.,1.,1.])
  >>> xobs=hArray(float, 3, [0.,0.,0.])
  >>> acceleration=1.0
  >>> hEndPointRadiation(Ex,Ey,Ez,frequencies,xq,vq,n,xobs,acceleration)
  >>> total_power.spectralpower2(Ex) #add square of complex spectrum to total power
  >>> total_power.spectralpower2(Ey)
  >>> total_power.spectralpower2(Ez)
  >>> total_power.plot()

*/
template <class Iter, class FIter>
void HFPP_FUNC_NAME(const Iter Ex,const Iter Ex_end,
                    const Iter Ey,const Iter Ey_end,
                    const Iter Ez,const Iter Ez_end,
                    const FIter frequency,const FIter frequency_end,
                    const FIter xq, const FIter xq_end,
                    const FIter vq, const FIter vq_end,
                    const HNumber t,
                    const HNumber n,
                    const FIter xobs, const FIter xobs_end,
                    const HNumber acceleration
  )

{
  // Define the iterators you work with, they point to the current
  // location in the vector.
  Iter Ex_it(Ex),Ey_it(Ey),Ez_it(Ez);
  FIter frequency_it(frequency);

  // An asterisks in front points to the value, i.e. *Ex_it = *Ey_it
  // assigns the location pointed to by the Ex vector iterator the
  // value that is pointed to by the Ey vector iterator.

  static std::vector<HNumber> v_tmp(3);
  static std::vector<HNumber> pol(3);
  static std::vector<HNumber> temp1(3);
  static std::vector<HNumber> r(3);
  static std::vector<HNumber> beta(3);


  // Determine length of frequency vector
  HInteger length = frequency_end - frequency;

  //Some sanity check.
  if (length <= 0) throw PyCR::ValueError("hEndPointRadiation: Frequency vector has illegal size.");
  if ((Ex_end - Ex) != length) throw PyCR::ValueError("hEndPointRadiation: Ex vector has different size than frequency vector.");
  if ((Ey_end - Ey) != length) throw PyCR::ValueError("hEndPointRadiation: Ey vector has different size than frequency vector.");
  if ((Ez_end - Ez) != length) throw PyCR::ValueError("hEndPointRadiation: Ez vector has different size than frequency vector.");

  // Needs to calculate x_q-x_O and R and n and beta and
  if ((xq_end-xq) != 3) throw PyCR::ValueError("x_q should have length 3");
  if ((vq_end-vq) != 3) throw PyCR::ValueError("v_q should have length 3");
  if ((xobs_end-xobs) != 3) throw PyCR::ValueError("x_obs should have length 3");

  //--------------------------------------------------------------------------------------
  //Initial calculations

  hSub(temp1.begin(),temp1.end(), xobs,xobs_end,xq,xq_end); // Vector pointing from charge to obs
  HNumber R = hVectorLength(temp1); // Length of vector between charge and observer
  if (abs(R) < 0.00001) {
    R = 0.00001;
  }

  hDiv(r, temp1, R);//unit vector between charge and observer
  hFill(beta, 0.);
  hDivAdd(beta.begin(),beta.end(),vq,vq_end,(HNumber)SPEED_OF_LIGHT);

  HNumber eta = 0.;
  eta = (1 - n * hDotProduct(beta.begin(),beta.end(),r.begin(),r.end())); // Doppler factor
  HNumber ksi = 0.;
  ksi= 2 * M_PI * (-t + (n * R) / (HNumber)SPEED_OF_LIGHT); // Phase for the endpoint (well this needs to multiplied with omega for that. Minus sign! Times 2 pi.

  //-------------------------------------------------------------------------------
  //Crossproduct (r cross (r cross beta)) gives the vector behaviour of E

  hCrossProduct(v_tmp.begin(),v_tmp.end(),r.begin(),r.end(),beta.begin(),beta.end());
  hCrossProduct(pol.begin(),pol.end(),r.begin(),r.end(),v_tmp.begin(),v_tmp.end());
  HNumber amplitude = 0;
  if(abs(eta) == 0.) {
    eta = 0.00001;
  }
  amplitude = acceleration * alpha / (eta * R);

  HNumber phase(0.);

  // Loop from beginning until end of frequency vector
  while (frequency_it != frequency_end) {
    // Add values to the current E-field element
    phase = *frequency_it * ksi;
    *Ex_it += amplitude * pol[0] * hPhaseToComplex(phase);
    *Ey_it += amplitude * pol[1] * hPhaseToComplex(phase);
    *Ez_it += amplitude * pol[2] * hPhaseToComplex(phase);
    ++frequency_it; // Increase frequency iterator, point to next element
    ++Ex_it; ++Ey_it; ++Ez_it; // Increase E-field iterators, point to next element
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
