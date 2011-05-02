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
#define HFPP_PARDEF_4 (HNumber)(x)()("x coordinate of particle location.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HNumber)(y)()("y coordinate of particle location.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(z)()("z coordinate of particle location.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HNumber)(t)()("time of particle at endpoint.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_8 (HNumber)(n)()("refractive index at endpoint.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_9 (HNumber)(vx)()("x coordinate of particle velocity.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_10 (HNumber)(vy)()("y coordinate of particle velocity.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_11 (HNumber)(vz)()("z coordinate of particle velocity.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_12 (HNumber)(acceleration)()("sign of acceleration -1=deceleration, +1=acceleration.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  hEndPointRadiation(Ex,Ey,Ez,frequency,x,y,z,t,n,vx,vy,vz,acceleration) -> Ex,Ex,Ez = observed E-field components

  Description:

  Does something useful

  See also:

  other useful functions

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
  >>> (x,y,z)=(1.,1.,1.)
  >>> t=2.
  >>> n=1.
  >>> (vx,vy,vz)=(1.,1.,1.)
  >>> acceleration=1.0
  >>> hEndPointRadiation(Ex,Ey,Ez,frequencies,x,y,z,t,n,vx,vy,vz,acceleration)
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
                    const HNumber x, const HNumber y, const HNumber z,
                    const HNumber t, const HNumber n,
                    const HNumber vx, const HNumber vy, const HNumber vz, const HNumber acceleration
  )

{
  //Define the iterators you work with, they point to the current
  //location in the vector.
  Iter Ex_it(Ex),Ey_it(Ey),Ez_it(Ez);
  FIter frequency_it(frequency);

  //An asterisks in front points to the value, i.e. *Ex_it = *Ey_it
  //assigns the location pointed to by the Ex vector iterator the
  //value that is pointed to by the Ey vector iterator.


  //Determine length of frequency vector
  HInteger length=frequency_end-frequency;

  //Some sanity check.
  if (length<=0) throw PyCR::ValueError("hEndPointRadiation: Frequency vector has illegal size.");
  if ((Ex_end-Ex) != length) throw PyCR::ValueError("hEndPointRadiation: Ex vector has different size than frequency vector.");
  if ((Ey_end-Ey) != length) throw PyCR::ValueError("hEndPointRadiation: Ey vector has different size than frequency vector.");
  if ((Ez_end-Ez) != length) throw PyCR::ValueError("hEndPointRadiation: Ez vector has different size than frequency vector.");

  //now loop from beginning until end of frequency vector
  while (frequency_it != frequency_end){
    //add values to the current E-field element
    *Ex_it += x*y*z*t*n*vx*vy*vz*acceleration* (*frequency_it);
    *Ey_it += x*y*z*t*n*vx*vy*vz*acceleration* (*frequency_it);
    *Ez_it += x*y*z*t*n*vx*vy*vz*acceleration* (*frequency_it);
    ++frequency_it; //increase frequency iterator, point to next element
    ++Ex_it; ++Ey_it; ++Ez_it; //increase E-field iterators, point to next element
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
