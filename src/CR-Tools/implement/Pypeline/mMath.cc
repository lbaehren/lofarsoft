/**************************************************************************
 *  Math module for CR Pipeline Python bindings.                          *
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
//$FILENAME: HFILE=mMath.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mArray.h"
#include "mMath.h"

#include "casa/BasicSL/Constants.h"


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
//$SECTION: Math functions
// ========================================================================

//$DOCSTRING: Returns the squared value of the parameter.
//$COPY_TO HFILE START ---------------------------------------------------
//#define HFPP_CLASS_STDIT  ///TEMPORARY FIX TO TEST FORWARD DECLARATION
#define HFPP_FUNC_NAME square
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNC_IS_INLINE HFPP_TRUE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(val)()("Value to be squared")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class T>
inline T square(const T val)
{
  return val*val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the natural logarithm of the value if the value is larger than zero, otherwise return a low number.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME logSave
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNC_IS_INLINE HFPP_TRUE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(val)()("Numerical input value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class T>
inline T HFPP_FUNC_NAME(const T val)
{
  if (val>hfcast<HNumber>(0)) return log(val);
  else return A_LOW_NUMBER;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the square root of the absolute of a nummber
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME sqrtAbs
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNC_IS_INLINE HFPP_TRUE
#define HFPP_FUNCDEF  (HFPP_TEMPLATED)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(val)()("Numerical input value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

Example:

sqrtAbs(-4) -> 2
*/
template <class T>
inline T HFPP_FUNC_NAME(const T val){return sqrt(abs(val));}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Implementation of the Gauss function.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME funcGaussian
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(x)()("Position at which the Gaussian is evaluated")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(sigma)()("Width of the Gaussian")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(mu)()("Mean value of the Gaussian")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HNumber funcGaussian (const HNumber x,
                      const HNumber sigma,
                      const HNumber mu)
{
  return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*casa::C::pi));
};
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the maximum value in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMax
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vec.max() -> maximum value

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  IterValueType val=*it;
  while (it!=vec_end) {
    if (*it > val) val=*it;
    ++it;
  };
  return val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the minimum value in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMin
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vec.max() -> minimum value

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  IterValueType val=*it;
  while (it!=vec_end) {
    if (*it < val) val=*it;
    ++it;
  };
  return val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Return the position of the maximum value in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaxPos
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vec.maxpos() -> i # position of maximum value

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  IterValueType val=*it;
  HInteger ipos(0);
  while (it!=vec_end) {
    if (*it > val) {
      val=*it;
      ipos=(it-vec);
    };
    ++it;
  };
  return ipos;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the position of the minimum value in a vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinPos
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
Usage:
  vec.maxpos() -> i # position of minimum value

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  IterValueType val=*it;
  HInteger ipos(0);
  while (it!=vec_end) {
    if (*it < val) {
      val=*it;
      ipos=(it-vec);
    };
    ++it;
  };
  return ipos;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//========================================================================
//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,logSave,sqrtAbs,sin,sinh,sqrt,square,tan,tanh
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}2(const Iter vecout,const Iter vecout_end, const Iter vecin,const Iter vecin_end)
{
  // Declaration of variables
  Iter itin=vecin;
  Iter itout=vecout;

  //NOTE: In principle one should not check both iterators every time,
  // but do one check in the beginning and then take the shorter
  // vector length

  // Vector operation
  while ((itin!=vecin_end) && (itout !=vecout_end)) {
    *itout=$MFUNC(*itin);
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE



//========================================================================
//$ITERATE MFUNC acos,asin,atan,ceil,floor,round
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec.$MFUNC() -> $MFUNC operation on vec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void h{$MFUNC!CAPS}1(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec2.$MFUNC(vec1) -> return result of $MFUNC operation on vec1 in vec2.
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void h{$MFUNC!CAPS}2(const IterOut vecout, const IterOut vecout_end, const IterIn vecin, const IterIn vecin_end)
{
  // Declaration of variables
  IterIn itin=vecin;
  IterOut itout=vecout;

  // Vector operation
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout=$MFUNC(*itin);
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================

//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the second vector. If the first vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}To
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input  vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operand and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  hMulTo(vecA,vecB) -> vecB=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  hAddTo(vecA,vecB) -> vecB=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  hSubTo(vecA,vecB) -> vecB=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  hDivTo(vecA,vecB) -> vecB=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]

  \brief $DOCSTRING
  $PARDOCSTRING

  Alternative usages are:

  hMul(vecA,vecB) = vecA.mul(vecB)  etc.

*/
template <class Iterin, class Iter>
void HFPP_FUNC_NAME(const Iterin vec1,const Iterin vec1_end, const Iter vec2,const Iter vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin it1=vec1;
  Iter it2=vec2;

  // Vector operation
  while (it2!=vec2_end) {
    *it2 =  hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC (*it2));
    ++it1; ++it2;
    if (it1==vec1_end) it1=vec1;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC between the two vectors, which is returned in the first vector. If the second vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  vecA *= vecB  -> vecA=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  vecA += vecB  -> vecA=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  vecA -= vecB  -> vecA=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  vecA /= vecB  -> vecA=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]

  \brief $DOCSTRING
  $PARDOCSTRING

  Alternative usages are:

  hMul(vecA,vecB) of vecA.mul(vecB)  etc.

*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iter it1=vec1;
  Iterin it2=vec2;

  // Vector operation
  while (it1!=vec1_end) {
    *it1 =  hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC (*it2));
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC between the vector and a scalar (applied to each element), which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(scalar1)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class S>
void h{$MFUNC}2(const Iter vec1,const Iter vec1_end, const S val)
{
      // Declaration of variables
      typedef IterValueType T;
      Iter it=vec1;

      // Vector operation
      while (it!=vec1_end) {
        *it = hfcast<T>(*it HFPP_OPERATOR_$MFUNC val);
        ++it;
      };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, which is returned in the first vector. If the second operand vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;

  // Vector operation
  while ((it1 != vec1_end) && (itout != vec_end)) {
    *itout = hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, and add the result to the first vector which can be of different type. Looping will be done over the first argument, i.e. the input/output vector. If the second operand vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Add
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  \brief $DOCSTRING
  $PARDOCSTRING

Related Functions are hMulAdd, hDivAdd, HSubAdd, hAddAdd.

To loop over the second argument (i.e., vec1) use hMulAdd2, hDivAdd2, HSubAdd2, hAddAdd2.

*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;

  // Vector operation
  while ((it1!=vec1_end)  && (itout !=vec_end)) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, and add the result to the first vector which can be of different type. Looping will be done over the first argument, i.e. the input/output vector. If the second operand vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Add2
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  \brief $DOCSTRING
  $PARDOCSTRING

Related Functions are hMulAdd2, hDivAdd2, HSubAdd2, hAddAdd2.

To loop over the first argument (i.e., vec) use hMulAdd, hDivAdd, HSubAdd, hAddAdd.

*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;

  // Vector operation
  while ((it1!=vec1_end) && (itout!=vec_end)) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
	if (itout==vec_end) itout=vec;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC!LOW between the vector and a scalar, where the result is returned in the first vector (with automatic casting).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(scalar1)()("Scalar value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,vec1,scalar) -> vec = vec1 $MFUNC!LOW scalar
  vec.$MFUNC(vec1,scalar) -> vec = vec1 $MFUNC!LOW scalar

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iterin,  class S>
void h{$MFUNC}2(const Iter vec,const Iter vec_end,  const Iterin vec1,const Iterin vec1_end, const S scalar)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin it1=vec1;
  Iter itout=vec;

  // Vector operation
  while (itout !=vec_end) {
    *itout = hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC scalar);
    ++it1; ++itout;
    if (it1==vec1_end) it1=vec1;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC!LOW operation (i.e., val +/-* vec) in place between a scalar and a vector, where the scalar is the first argument.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Self
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Scalar value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  h$MFUNC(vec,val) -> vec = val $MFUNC!LOW vec
  vec.$MFUNC(val) -> vec = val $MFUNC!LOW vec

  \brief $DOCSTRING
  $PARDOCSTRING

Doesn't really make sense for add and mul, but is useful to get the inverse of a vector.

Example:
vec=Vector([1.,2.,4.])
vec.divself(1) -> [1.0,0.5,0.25]

See also: hMulSelf, hDivSelf, HSubSelf, hAddSelf.
*/
template <class Iter, class S>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const S val)
{
  // Declaration of variables
  Iter it(vec);
  typedef IterValueType T;
  if (it>vec_end) {return;}
  while (it!=vec_end) {
    *it = hfcast<T>(val) HFPP_OPERATOR_$MFUNC (*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE



//========================================================================
//$ITERATE MFUNC Fmod
//========================================================================

//$DOCSTRING: Perform a $MFUNC!LOW with a scalar parameter on the elements of a numeric vector and return the result in the same numeric vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(param)()("Scalar parameter of function $MFUNC")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
		    const HNumber param) {
  Iter it = vec;
  while (it != vec_end) {
    *it = $MFUNC!LOW(*it, param);
    ++it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Perform a $MFUNC!LOW with a scalar parameter from a vector on the elements of the first numeric vector and return the result in the same vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vecpar)()("Parameter vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  h$MFUNC(vec, vecpar) -> vec = $MFUNC!LOW(vec, vecpar)
  vec.$MFUNC(vecpar) -> vec = $MFUNC!LOW(vecin, vecpar)

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
		    const Iter vecpar, const Iter vecpar_end) {
  // Declaration of variables
  Iter it = vec;
  Iter it_par = vecpar;
  HInteger lenIO  = vec_end - vec;
  HInteger lenPar = vecpar_end - vecpar;

  // Sanity check
  if (lenPar < lenIO) {
    cout << "Size of parameter vector is smaller than size of input/output vector: looping over parameter vector." << endl;
  } else if (lenPar > lenIO) {
    throw PyCR::ValueError("Size of parameter vector is larger than size of input/output vector.");
  }

  // Vector operation
  while (it != vec_end) {
    *it = $MFUNC!LOW(*it, *it_par);
    ++it; ++it_par;
    if (it_par == vecpar_end) it_par = vecpar;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Perform a $MFUNC!LOW with scalar parameter on the elements of the second numeric vector and return the result in the first numeric vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(param)()("Scalar parameter of function $MFUNC")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  h$MFUNC(vec, vecin, param) -> vec = $MFUNC!LOW(vecin, param)
  vec.$MFUNC(vecin, param) -> vec = $MFUNC!LOW(vecin, param)

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end,
		    const Iter vecin, const Iter vecin_end,
		    const HNumber scalar) {
  // Declaration of variables
  Iter it_in = vecin;
  Iter it_out = vecout;
  HInteger lenIn = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenIn < lenOut) {
    cout << "Input vector is smaller than output vector: looping over input vector." << endl;
  } else if (lenIn > lenOut) {
    throw PyCR::ValueError("Input vector is larger than output vector.");
  }

  // Vector operation
  while ((it_in != vecin_end) && (it_out != vecout_end)) {
    *it_out = $MFUNC!LOW(*it_in, scalar);
    ++it_out; ++it_in;
    if (it_in == vecin_end) it_in = vecin;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Perform a $MFUNC!LOW with a scalar parameter from a vector on the elements of the second numeric vector and return the result in the first numeric vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vecin)()("Numeric inpuit vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(vecpar)()("Parameter vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  h$MFUNC(vec, vecin, vecpar) -> vec = $MFUNC!LOW(vecin, vecpar)
  vec.$MFUNC(vecin, vecpar) -> vec = $MFUNC!LOW(vecin, vecpar)

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end,
		    const Iter vecin, const Iter vecin_end,
		    const Iter vecpar, const Iter vecpar_end) {
  // Declaration of variables
  Iter it_in = vecin;
  Iter it_out = vecout;
  Iter it_par = vecpar;
  HInteger lenIn = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;
  HInteger lenPar = vecpar_end - vecpar;

  // Sanity check
  if (lenIn != lenOut) {
    throw PyCR::ValueError("Input and output vectors are not of the same size.");
  } else {
    if (lenPar < lenIn) {
      cout << "Size of parameter vector is smaller than size of input vector: looping over parameter vector." << endl;
    } else if (lenPar > lenIn) {
      throw PyCR::ValueError("Size of parameter vector is larger than size of input vector.");
    }
  }

  // Vector operation
  while ((it_in != vecin_end) && (it_out != vecout_end)) {
    *it_out = $MFUNC!LOW(*it_in, *it_par);
    ++it_in; ++it_out; ++it_par;
    if (it_par == vecpar_end) it_par = vecpar;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE



// ========================================================================
//
//  Complex functions
//
// ========================================================================

//$DOCSTRING: Returns the interferometer phase in radians for a given frequency and time.
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hPhase
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(frequency)()("Frequency in Hz")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(time)()("Time in seconds")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HNumber HFPP_FUNC_NAME(const HNumber frequency, const HNumber time)
{
  return CR::_2pi*frequency*time;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Coverts a real phase to a complex number (with amplitude of unity)
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hPhaseToComplex
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HComplex)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(phase)()("Phase of complex number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HComplex HFPP_FUNC_NAME(const HNumber phase)
{
  return polar(1.0, phase); // exp(HComplex(0.0,phase));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Coverts a vector of real phase to a vector of corresponding complex numbers (with amplitude of unity).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPhaseToComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector returning complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(phasevec)()("Input vector with real phases")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hPhaseToComplex(outvec,phase) -> outvec = [exp(i*phase_1),exp(i*phase_2),...,exp(i*phase_n)]
  outvec.phasetocomplex(phase) -> outvec = [exp(i*phase_1),exp(i*phase_2),...,exp(i*phase_n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If input vector is shorter it will be repeated until output vector is full.

*/
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 vec, const Iter1 vec_end, const Iter2 phasevec, const Iter2 phasevec_end)
{
  // Declaraion of variables
  Iter1 it1=vec;
  Iter2 it2=phasevec;
  HInteger lenOut = vec_end - vec;
  HInteger lenPhase = phasevec_end - phasevec;

  // Sanity check
  if (lenPhase < lenOut) {
    cout << "Size of phase vector is smaller than output vector: looping over phase vector." << endl;
  } else if (lenPhase > lenOut) {
    throw PyCR::ValueError("Phase vector is larger than output vector.");
  } else if (lenPhase <= 0) {
    throw PyCR::ValueError("Illegal size of phase vector");
  }

  // Vector operation
  while (it1!=vec_end) {
    *it1=hPhaseToComplex(*it2);
    ++it1;
    ++it2; if (it2==phasevec_end) it2=phasevec; // loop over input vector if necessary ...
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Coverts a real phase and amplitude to a complex number
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hAmplitudePhaseToComplex
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HComplex)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(amplitude)()("Amplitude of complex number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(phase)()("Phase of complex number")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HComplex HFPP_FUNC_NAME(const HNumber amplitude, const HNumber phase)
{
  return polar(amplitude, phase); // exp(HComplex(0.0,phase))
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Coverts a vector of real phases and amplitudes to a vector of corresponding complex numbers.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAmplitudePhaseToComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector returning complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(ampphase)()("Input vector with real amplitudes and phases (2 numbers per entry: [amp_0, phase_0, amp_1, phase_1, ...])")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hAmplitudePhaseToComplex(vec,ampphase) -> vec = [ampphase_0,0*exp(i*ampphase_0,1),ampphase_1,0*exp(i*ampphase_1,1),...,ampphase_n,0*exp(i*ampphase_n,1)]
  vec.amplitudephasetocomplex(ampphase) -> vec = [ampphase_0,0*exp(i*ampphase_0,1),ampphase_1,0*exp(i*ampphase_1,1),...,ampphase_n,0*exp(i*ampphase_n,1)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If input vector is shorter it will be repeated until output vector is full.

*/
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 vec, const Iter1 vec_end,
                     const Iter2 ampphase, const Iter2 ampphase_end)
{
  // Variables
  Iter1 it1=vec;
  Iter2 it2=ampphase;
  HInteger lenOut = vec_end - vec;
  HInteger lenAP = ampphase_end - ampphase;

  // Sanity check
  if (hfodd(lenAP)) {
    throw PyCR::ValueError("AmplitudePhase vector has wrong size. Total number of elements is not even.");
    return;
  } else {
    if (lenAP/2 < lenOut) {
      cout << "Size of AmplitudePhase vector is smaller than output vector: looping over AmplitudePhase vector." << endl;
    } else if (lenAP/2 > lenOut) {
      throw PyCR::ValueError("AmplitudePhase vector is too large for output vector.");
    }
  }

  if (it2+1>=ampphase_end) return;
  while (it1!=vec_end) {
    *it1=hAmplitudePhaseToComplex(*it2,*it2+1);
    ++it1;
    it2+=2; if (it2>=ampphase_end) it2=ampphase; // loop over input vector if necessary ...
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculate the complex conjugate of all elements in the complex vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConj
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void  HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  while (it!=vec_end) {
    *it=conj(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Multiplies the elements of the first vector with the complex conjugate of the elements in the second and returns the results in the first.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCrossCorrelateComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec1)()("Complex input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vec2)()("Second complex vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  fftvec1.crosscorrelatecomplex(fftvec2) -> fftvec1 return FFT of cross correlation between

  \brief $DOCSTRING
  $PARDOCSTRING

 If the second vector is shorter than the first one, the second vector
 will simply wrap around and begin from the start until the end of the
 first vector is reached. If the first vector is shorter, then the
 calculation will simply stop.

 Relation to Cross Correlation:

 If the complex input vectors are the Fourier transformed data of two
 (real) vector f1 & f2, then vec1*conj(vec2) will be the Fourier
 transform of the crosscorrelation between f1 and f2.

 Hence, in order to calculate a cross correlation between f1 & f2, first do
 vec1.fft(f1) and vec2.fft(f2), then vec1.crosscorrelatecomplex(vec2)
 and FFT back through floatvec.invfft(vec1).
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end)
{
  // Declaration of variables
  Iter it1=vec1;
  Iter it2=vec2;
  HInteger len1 = vec1_end - vec1;
  HInteger len2 = vec2_end - vec2;

  // Sanity check
  if (len2 < len1) {
    cout << "Size of second vector is smaller than output vector: looping over second vector." << endl;
  } else if (len2 > len1) {
    throw PyCR::ValueError("Second vector is larger than output vector.");
    return;
  }

  // Vector operation
  while (it1!=vec1_end) {
    *it1 *= conj(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"




//========================================================================
//$ITERATE MFUNC arg,imag,norm,real
//========================================================================

//$DOCSTRING: Take the $MFUNC of all the elements in the complex vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Complex input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec2.$MFUNC(vec1) -> vec2 = [$MFUNC(vec1_0), $MFUNC(vec1_1), ... , $MFUNC(vec1_n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vector is shorter than the output vector it will wrap around
  and start from the beginning until the output vector is fully processed.

  Input and output vector can be identical.

The following functions are available for getting real values from
complex numbers:
  norm - magnitude (length) squared of a complex number, i.e. c * conj(c)
  abs - amplitude of a complex number, i.e. c * conj(c)
  arg - phase angle of a complex number (in radians)
  imag - imaginary part of a complex number
  real - real part of a complex number
*/
template <class Iter, class IterIn>
void h{$MFUNC!CAPS}(const Iter vecout,const Iter vecout_end, const IterIn vecin, const IterIn vecin_end)
{
  // Declaration of variables
  IterIn itin(vecin);
  Iter itout(vecout);
  HInteger lenIn = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenIn < 0) {
    throw PyCR::ValueError("Illegal size of input vector (<=0).");
    return;
  }
  if (lenOut < 0) {
    throw PyCR::ValueError("Illegal size of output vector (<=0).");
    return;
  }
  if (lenIn < lenOut) {
    cout << "Size of input vector is smaller than output vector: looping over input vector." << endl;
  } else if (lenIn > lenOut) {
    throw PyCR::ValueError("Input vector is larger than output vector.");
  }

  // Vector operation
  while (itout != vecout_end) {
    *itout=(IterValueType)$MFUNC(*itin);
    ++itin; ++itout;
    if (itin == vecin_end) itin=vecin;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Take the $MFUNC of all the elements in the complex vector and return results in the same vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex in and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec2.$MFUNC(vec1) -> vec2 = [$MFUNC(vec1_0), $MFUNC(vec1_1), ... , $MFUNC(vec1_n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vector is shorter than the output vector it will wrap around
  and start from the beginning until the output vector is fully processed.

  Input and output vector can be identical.

The following functions are available for getting real values from
complex numbers:
  norm - magnitude (length) squared of a complex number, i.e. c * conj(c)
  abs - amplitude of a complex number, i.e. c * conj(c)
  arg - phase angle of a complex number (in radians)
  imag - imaginary part of a complex number
  real - real part of a complex number
*/
template <class Iter>
void h{$MFUNC!CAPS}(const Iter vecout,const Iter vecout_end)
{
  // Declaration of variables
  Iter itout(vecout);
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of input/output vector (<=0).");
    return;
  }

  // Vector operation
  while (itout != vecout_end) {
    *itout=$MFUNC(*itout);
    ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE



//$DOCSTRING: Set the amplitude of complex numbers to the values provided in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSetAmplitude
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector with amplitudes.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  The function calculates (c.setamplitude(newamplitude)) for each
  element c -> c/|c|*newamplitude.

  If input vector is shorter than output vector it will wrap around
  and start from the beginning until the output vector is fully processed.
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME (const IterOut vecout,const IterOut vecout_end, const IterIn vecin, const IterIn vecin_end)
{
  // Declaration of variables
  IterIn itin(vecin);
  IterOut itout(vecout);
  HInteger lenIn = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenIn <= 0) {
    throw PyCR::ValueError("Illegal size of input vector (<=0).");
    return;
  }
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector (<=0).");
    return;
  }
  if (lenIn < lenOut) {
    cout << "Size of input vector is smaller than output vector: looping over input vector." << endl;
  } else if (lenIn > lenOut) {
    throw PyCR::ValueError("Input vector is larger than output vector.");
  }

  // Vector operation
  while (itout != vecout_end) {
    try {
      *itout *= (*itin)/abs(*itout);
    }
    catch (typename IterOut::value_type i) {
      if (abs(i) < A_LOW_NUMBER)
        cerr << "Division by zero" << endl;
    }
    ++itin; ++itout;
    if (itin == vecin_end) itin=vecin;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the amplitude of complex numbers to a particular value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSetAmplitude
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(amp)()("Amplitude")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  The function calculates (c.setamplitude(amp)) for each
  element c -> c/|c|*amp.
*/
template <class IterOut, class Iter>
void HFPP_FUNC_NAME (const IterOut vecout,const IterOut vecout_end, Iter amp)
{
  // Declaration of variables
  IterOut itout(vecout);
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector (<=0).");
    return;
  }

  // Vector operation
  while (itout != vecout_end) {
    try {
      *itout *= amp/abs(*itout);
    }
    catch (typename IterOut::value_type i){
      if (abs(i) < A_LOW_NUMBER)
        cerr << "Division by zero" << endl;
    }
    ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Multiplies each element in the vector with -1 in place, i.e. the input vector is also the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNegate
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it=vec;
  IterValueType fac = -1;
  while (it!=vec_end) {
    *it=(*it) * fac;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Multiplies all elements in the vector with each other and return the result
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hProduct
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING

 hProduct(vec) -> vec[0]*vec[1]*vec[2]* ... * vec[N]

*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  typedef IterValueType T;
  T prod=1.0;
  Iter it=vec;
  while (it!=vec_end) {prod *= *it; ++it;};
  return prod;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the values in a vector and returns the value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {
    sum+=*it;
    ++it;
  };
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the absolute values in a vector and returns the value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSumAbs
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {
    sum+=abs(*it);
    ++it;
  };
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a sum over the square values in a vector and returns the value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSumSquare
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum=hfnull<T>();
  Iter it=vec;
  while (it!=vec_end) {
    sum+=(*it)*(*it);
    ++it;
  };
  return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Piecewise multiplication of the elements in a vector and summing of the results
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMulSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hMulSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> a*x + b*y + c*z + ....

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
      typedef IterValueType T;
      T sum=hfnull<T>();
      Iter it1=vec1,it2=vec2;
      while (it1!=vec1_end && it2!=vec2_end) {
        sum+=(*it1) * (*it2);
        ++it1; ++it2;
      };
      return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Piecewise subtraction of the elements in a vector and summing of the results
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDiffSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hDiffSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> a-x + b-y + c-z + ....

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
      typedef IterValueType T;
      T sum=hfnull<T>();
      Iter it1=vec1,it2=vec2;
      while ((it1!=vec1_end) && (it2!=vec2_end)) {
        sum+=(*it1) - (*it2);
        ++it1; ++it2;
      };
      return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Piecewise subtraction of the elements in a vector and summing of the square of the results
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDiffSquaredSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hDiffSquaredSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> (a-x)**2 + (b-y)**2 + (c-z)**2 + ....

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
      typedef IterValueType T;
      T sum=hfnull<T>();
      T val;
      Iter it1=vec1,it2=vec2;
      while ((it1!=vec1_end) && (it2!=vec2_end)) {
	val=(*it1) - (*it2);
        sum+=val*val;
        ++it1; ++it2;
      };
      return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Mean of the piecewise subtraction of the elements in a vector and summing of the square of the results
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanDiffSquaredSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hMeanDiffSquaredSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> ((a-x)**2 + (b-y)**2 + (c-z)**2 + ....)/N

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
      typedef IterValueType T;
      T sum=hfnull<T>();
      T val;
      Iter it1=vec1,it2=vec2;
      while ((it1!=vec1_end) && (it2!=vec2_end)) {
	val=(*it1) - (*it2);
        sum+=val*val;
        ++it1; ++it2;
      };
      return sum/hfmin(vec1_end-vec1,vec2_end-vec2);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the chi-squared of two vectors, i.e., (v1,i-v2,i)**2/v2,i
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hChiSquared
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hChiSquared(Vector([a,b,c,...]),Vector([x,y,z,...]) -> (a-x)**2/x + (b-y)**2/y + (c-z)**2/z + ....

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
      typedef IterValueType T;
      T sum=hfnull<T>();
      T val;
      Iter it1=vec1,it2=vec2;
      while ((it1!=vec1_end) && (it2!=vec2_end)) {
	val=(*it1) - (*it2);
        sum+=val*val/(*it2);
        ++it1; ++it2;
      };
      return sum;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the mean chi-squared of two vectors, i.e., mean((v1,i-v2,i)**2/v2,i)
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanChiSquared
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector (observed values)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector (expected values) - must not be zero")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hChiSquared(Vector([a,b,c,...]),Vector([x,y,z,...]) -> ((a-x)**2/x + (b-y)**2/y + (c-z)**2/z + ....)/N

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
      typedef IterValueType T;
      T sum=hfnull<T>();
      T val;
      Iter it1=vec1,it2=vec2;
      while ((it1!=vec1_end) && (it2!=vec2_end)) {
	val=(*it1) - (*it2);
        sum+=val*val/(*it2);
        ++it1; ++it2;
      };
      return sum/(vec2_end-vec2);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Dot product of two vectors
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDotProduct
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecin1)()("Numeric input vector 1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin2)()("Numeric input vector 2")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hDotProduct(Vector([a,b,c]), Vector([x,y,z])) -> a*x + b*y + c*z

  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vecin1, const Iter vecin1_end,
			      const Iter vecin2, const Iter vecin2_end)
{
  // Declaration of variables
  IterValueType dotProduct(0.);
  HInteger len1 = vecin1_end - vecin1;
  HInteger len2 = vecin2_end - vecin2;

  // Sanity check
  if (len1 != len2) {
    throw PyCR::ValueError("Input vectors not of equal size.");
    return 0.;
  }

  // Vector operation
  dotProduct = hMulSum(vecin1, vecin1_end, vecin2, vecin2_end);

  return dotProduct;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Cross product of two 3-dimensional vectors
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCrossProduct
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin1)()("Numeric input vector 1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecin2)()("Numeric input vector 2")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hCrossProduct(Vector([a,b,c]), Vector([r,s,t]), Vector([x,y,z]))
  -> [s*z - t*y, t*x - r*z, r*y - s*z]

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout, const Iter vecout_end,
		     const Iter vecin1, const Iter vecin1_end,
		     const Iter vecin2, const Iter vecin2_end)
{
  // Declaration of variables
  Iter itout(vecout);
  Iter itin1(vecin1);
  Iter itin2(vecin2);
  HInteger lenIn1 = vecin1_end - vecin1;
  HInteger lenIn2 = vecin2_end - vecin2;
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenIn1 != 3) {
    throw PyCR::ValueError("First input vector is not of length 3.");
    return;
  }
  if (lenIn2 != 3) {
    throw PyCR::ValueError("Second input vector is not of length 3.");
    return;
  }
  if (lenOut != 3) {
    throw PyCR::ValueError("Output vector is not of length 3.");
    return;
  }

  // Vector operation
  *(itout)   = *(itin1+1) * *(itin2+2) - *(itin1+2) * *(itin2+1);
  *(itout+1) = *(itin1+2) * *(itin2)   - *(itin1)   * *(itin2+2);
  *(itout+2) = *(itin1)   * *(itin2+1) - *(itin1+1) * *(itin2);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"




// ========================================================================
//
//  Statistical functions
//
// ========================================================================


//========================================================================
//$SECTION:           Statistics Functions
//========================================================================

//$DOCSTRING: Fills a vector with random values between minimum and maximum limits
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandom
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector which will be filled with random numbers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(minimum)()("Random numbers will not go below that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(maximum)()("Random numbers will not exceed that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hRandom(vector,-1,1) -> [-0.5,0.3,0.1, ...]
  vector.random(-1,1) -> [-0.5,0.3,0.1, ...]

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const IterValueType minimum, const IterValueType maximum)
{
  Iter it=vec;
  HNumber scale(abs(maximum-minimum));
  scale /= RAND_MAX;
  while (it!=vec_end) {
    *it=rand()*scale+minimum;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"





//$DOCSTRING: Returns the mean value of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMean
//-----------------------------------------------------------------------
//#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  // Declaration of variables
  HNumber mean = hfcast<HNumber>(hSum(vec,vec_end));
  HInteger len = vec_end - vec;

  // Sanity check
  if (len <= 0) {
    throw PyCR::ValueError("Size of vector is <= 0.");
    return 0.;
  }

  // Vector operation
  mean /= len;

  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of the absolue values of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanAbs
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  // Declaration of variables
  HNumber mean=hSumAbs(vec,vec_end);
  HInteger len = vec_end - vec;

  // Sanity check
  if (len <= 0) {
    throw PyCR::ValueError("Size of vector is <= 0.");
    return 0.;
  }

  // Vector operation
  mean /= len;

  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of the square values of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanSquare
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  // Declaration of variables
  HNumber mean=hSumSquare(vec,vec_end);
  HInteger len = vec_end - vec;

  // Sanity check
  if (len <= 0) {
    throw PyCR::ValueError("Size of vector is <= 0.");
    return 0.;
  }

  // Vector operation
  mean /= len;

  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Sorts a vector in place.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSort
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Attention!!! The vector will be sorted in place. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Sorts a vector in place and returns the median value of the elements.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSortMedian
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Attention!!! The vector will be sorted first. Hence, if you want to
  keep the data in its original order, you need to copy the data first
  to a scratch vector and then call this function with the scratch vector!
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  sort(vec,vec_end);
  if (vec_end!=vec) return *(vec+(vec_end-vec)/2);
  else return hfnull<IterValueType>();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the median value of the elements.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMedian
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
T HFPP_FUNC_NAME(std::vector<T> & vec)
{
  std::vector<T> scratch(vec);
  return hSortMedian(scratch);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the standard deviation around a mean value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDev
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(mean)()("The mean value of the vector calculated beforehand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const T mean)
{
  HNumber scrt,sum=0.0;
  HInteger len=vec_end-vec;
  Iter it=vec;
  while (it<vec_end) {
    scrt = hfcast<HNumber>(*it - mean);
    sum += scrt * scrt;
    ++it;
  };
  if (len>1)
    return sqrt(sum / (len-1));
  else
    return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the standard deviation using only the values below the mean value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDevBelow
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(mean)()("The mean value of the vector calculated beforehand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const T mean)
{
  HNumber scrt,sum(0.0);
  HInteger len(0);
  Iter it(vec);
  while (it<vec_end) {
    if (hfcast<HNumber>(*it) <= mean) {
      scrt = hfcast<HNumber>(*it - mean);
      sum += scrt * scrt;
      ++len;
    };
    ++it;
  };
  if (len>1)
    return sqrt(sum / (len-1));
  else
    return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the standard deviation using only the values below the mean value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDevAbove
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(mean)()("The mean value of the vector calculated beforehand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const T mean)
{
  HNumber scrt,sum(0.0);
  HInteger len(0);
  Iter it(vec);
  while (it<vec_end) {
    if (hfcast<HNumber>(*it) >= mean) {
      scrt = hfcast<HNumber>(*it - mean);
      sum += scrt * scrt;
      ++len;
    };
    ++it;
  };
  if (len>1)
    return sqrt(sum / (len-1));
  else
    return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//========================================================================
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================

//$DOCSTRING: Calculates the mean of all values which are $MFUNC a certain threshold value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMean{$MFUNC}Threshold
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(threshold)()("The threshold above which elements are taken into account")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec, const Iter vec_end, const IterValueType threshold)
{
  Iter it(vec);
  IterValueType sum(0),extremum(*it);
  HInteger n(0);
  while (it != vec_end) {
    if (*it HFPP_OPERATOR_$MFUNC extremum) extremum = *it;
    if (*it HFPP_OPERATOR_$MFUNC threshold) {
      sum += *it; ++n;
    };
    ++it;
  };

  if (n > 0) {
    return sum/n;
  } else {
    return extremum;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE



//$DOCSTRING: Calculates the mean of all values which are below or above the mean plus 'nsigma' standard deviations
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanThreshold
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(mean)()("The mean of the values in the input vector provided as input.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(rms)()("The rms value of the vector - also calculated beforehand.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(nsigma)()("Only consider values that are below (positive) or above (negative) mean+nsigma+sigma the mean")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  This simply call hMeanGreaterThanThreshold or hMeanLessThanThreshold with a
  threshold value of mean+nsigma*rms.
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec, const Iter vec_end, HNumber mean, HNumber rms, HNumber nsigma)
{
  IterValueType threshold = mean + nsigma * rms;

  if (nsigma >= 0) {
    return hMeanLessThanThreshold(vec, vec_end, threshold);
  } else {
    return hMeanGreaterThanThreshold(vec, vec_end, threshold);
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the mean of the inverse of all values and then return the inverse of that value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanInverse
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  vec.meaninverse() -> N/sum(1/vec_0+1/vec_1+...+1/vec_N)

  This is useful to calculate the mean value of very spiky data. Large
  spikes will appear as zero and hence will not have a strong effect
  on the average (or the sum) if only a small number of channels are
  affected, while a single very large spike could well dominate the
  normal average. Only works well if all values are positive (or
  negative). If th mean is around zero it becomes unstable, of
  course.

 */
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum(hfnull<T>());
  Iter it(vec);

  while (it!=vec_end) {
    try {
      sum+=1.0/(*it);
    }
    catch (T i) {
      if (abs(i) < A_LOW_NUMBER)
        cerr << "hMeanInverse: Division by zero at location " << (it-vec) << endl;
    }
    ++it;
  };

  if (abs(sum) > A_LOW_NUMBER) {
    return (vec_end-vec)/sum;
  } else {
    if (sum>0) return static_cast<IterValueType>(A_HIGH_NUMBER);// sign(sum)*(vec_end-vec)/A_LOW_NUMBER;
    else return static_cast<IterValueType>(-A_HIGH_NUMBER);//(vec_end-vec)/A_LOW_NUMBER;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates the standard deviation of a vector of values.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hStdDev
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber hStdDev (const Iter vec,const Iter vec_end)
{
  return hStdDev(vec,vec_end,hMean(vec,vec_end));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the first sample that equals the input values and return its position. Return -1 if not found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector to search through.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(value)()("Value to search for.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: findgreaterthan, findgreaterequal, findlessthan, findlessequal, findbetween, findoutside, findoutsideorequal, findbetweenorequal
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vecin , const Iter vecin_end,
			 const IterValueType value)
{
  // Declaration of variables
  Iter itin(vecin);
  // Sanity check
  if (vecin_end < vecin) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return -1;
  }

  while (itin != vecin_end) {
    if (*itin == value) {return itin-vecin;}
    ++itin;
  };
  return -1;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

// ========================================================================
//
//$Section:  Find & Seach
//
// ========================================================================


//========================================================================
//$ITERATE MFUNC Between,BetweenOrEqual,Outside,OutsideOrEqual
//========================================================================

//$DOCSTRING: Count the samples that are $MFUNC a certain threshold value and returns the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(lower_limit)()("Lower limit to search for.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(upper_limit)()("Upper limit to search for.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const T lower_limit, const T upper_limit)
{
  Iter it(vec);
  HInteger count(0);
  while (it<vec_end) {
    if  ({$MFUNC}(*it,lower_limit,upper_limit)) ++count;
    ++it;
  };
  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Find the first sample that is $MFUNC the two input values and return its position. Return -1 if not found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector to search through.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(lower_limit)()("Lower limit to search for.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(upper_limit)()("Upper limit to search for.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: findgreaterthan, findgreaterequal, findlessthan, findlessequal, findbetween, findoutside, findoutsideorequal, findbetweenorequal
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vecin , const Iter vecin_end,
			 const IterValueType lower_limit, const IterValueType upper_limit)
{
  // Declaration of variables
  Iter itin(vecin);
  // Sanity check
  if (vecin_end < vecin) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return -1;
  }

  while (itin != vecin_end) {
    if ({$MFUNC}(*itin,lower_limit,upper_limit)) {return itin-vecin;}
    ++itin;
  };
  return -1;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Find the samples that are $MFUNC upper and lower threshold values and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector which are between the limits.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(lower_limit)()("The lower limit of values to find.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_TYPE)(upper_limit)()("The upper limit of values to find.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: find, findgreaterthan, findgreaterequal, findlessthan, findlessequal, findbetween, findoutside, findoutsideorequal, findbetweenorequal

*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const IterValueType lower_limit,
			 const IterValueType upper_limit)
{
  // Declaration of variables
  Iter itin = vecin;
  typename vector<HInteger>::iterator itout=vecout;

  // Sanity check
  if (((vecin_end - vecin)<0) || ((vecout_end - vecout)<0)) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return 0.;
  }

  // Function operation
  while (itin != vecin_end) {
    if ({$MFUNC}(*itin,lower_limit,upper_limit)) {
      if (itout != vecout_end) {
	*itout=(itin-vecin);
	++itout;
      };
    };
    ++itin;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Find and return a list containing slice indices (i.e., beginning and end+1 position) of sequences of (almost) consecutive values in a vector which are {$MFUNC} the two threshold values
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFindSequence{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of 3 tuples of position, length, and mean value for each sequence found.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(lower_limit)()("Threshold value - lower_limit if values $MFUNC this, they can belong to a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_TYPE)(upper_limit)()("Threshold value - upper-limit if values are $MFUNC this, they can belong to a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(maxgap)()("The maximum gap (in between sample numbers) between two samples to still belong to one sequence. 0=no gaps allowed, i.e. consecutive.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(minlength)()("The minimum length of a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: find, findsequencegreaterthan, findsequencegreaterequal, findsequencelessthan, findsequencelessequal, findsequencebetween, findsequenceoutside, findsequenceoutsideorequal, findsequencebetweenorequal

Example:

#Make a test time series data set for 4 antennas and some peaks at various locations
data=hArray(float,[4,512],name="Random series with peaks")
data.random(-1024,1024); data[...] += Vector([-128.,256., 385.,-50.])
for i in range(4): data[i,[2,3,32,64,65,67],...]=Vector([4096.,5097,-4096,4096,5099,3096])

nsigma=5
datapeaks=hArray(int,[4,256,2],name="Location of peaks")
datamean=data[...].mean()
datathreshold2 = data[...].stddev(datamean)
datathreshold2 *= nsigma
datathreshold1 = datathreshold2*(-1)
datathreshold1 += datamean
datathreshold2 += datamean

maxgap=Vector(int,len(datamean),fill=10)
minlength=Vector(int,len(datamean),fill=1)
npeaks=datapeaks[...].findsequenceoutside(data[...],datathreshold1,datathreshold2,maxgap,minlength)
npeaks -> Vec(int,4)=[3,3,3,3]
datapeaks -> hArray(int,[4, 256, 2], name="Location of peaks") # len=2048, slice=[0:512], vec -> [2,4,32,33,64,68,0,0,...]

*/
template <class Iter, class IterI>
HInteger HFPP_FUNC_NAME (const IterI vecout, const IterI vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const IterValueType lower_limit,
			 const IterValueType upper_limit,
			 const HInteger maxgap,
			 const HInteger minlength
			 )
{
  Iter itlast(vecin), itfirst(vecin), itnow(vecin);
  IterI itout(vecout);
  HInteger nsequence(0);
  HBool sequenceon(false);
  if ((itnow>=vecin_end) || (itout+1>=vecout_end)) return 0;
  while (itnow < vecin_end) {
    if ({$MFUNC}(*itnow,lower_limit,upper_limit)) {
      if (!sequenceon) {sequenceon=true; itfirst=itnow;}; //Start of new sequence
      ++itnow;
      itlast=itnow; //continue old sequence
    } else {
      ++itnow;
      if (sequenceon) {
	if ((itnow-itlast)>maxgap) { //end of sequence
	  sequenceon=false;
	  if (itlast-itfirst>=minlength) {//Previous sequence long enough, start new sequence and advance output vector
	    *itout=(itfirst-vecin); ++itout; //Store result, begin
	    if (itout!=vecout_end) {*itout=(itlast-vecin); ++nsequence; ++itout;}; //Store result, begin
	    if (itout==vecout_end) return nsequence; //Sorry, end of output vector, return
	  };
	};
      };
    };
  };
  //Include last sequence if necessary
  if (sequenceon && (itlast-itfirst>=minlength)) {
    *itout=(itfirst-vecin); ++itout; //Store result, begin
    if (itout!=vecout_end) {*itout=(itlast-vecin); ++nsequence; ++itout;}; //Store result, begin
    if (itout==vecout_end) return nsequence; //Sorry, end of output vector, return
  };
  return nsequence;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$ENDITERATE


//========================================================================
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================

//$DOCSTRING: Find the first sample that is $MFUNC the threshold value and return its position. Return -1 if not found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNC_VARIANT 1
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input vector to search through.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(threshold)()("Threshold value to search for.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: findgreaterthan, findgreaterequal, findlessthan, findlessequal, findbetween, findoutside, findoutsideorequal, findbetweenorequal
*/
template <class Iter, class T>
HInteger hFind{$MFUNC}1 (const Iter vecin,const Iter vecin_end,
			 const T threshold)
{
  // Declaration of variables
  Iter itin(vecin);
  // Sanity check
  if (vecin_end < vecin) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return -1;
  }

  while (itin != vecin_end) {
    if (*itin HFPP_OPERATOR_$MFUNC threshold) {return itin-vecin;}
    ++itin;
  };
  return -1;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples that are $MFUNC a certain threshold value and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in the input vector which satisfy the threshold condition.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: find, findgreaterthan, findgreaterequal, findlessthan, findlessequal, findbetween, findoutside, findoutsideorequal, findbetweenorequal
*/
template <class Iter, class T>
HInteger hFind{$MFUNC}2 (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const T threshold
			 )
{
  // Declaration of variables
  Iter itin(vecin);
  typename vector<HInteger>::iterator itout(vecout);

  // Function operation
  while (itin != vecin_end) {
    if (*itin HFPP_OPERATOR_$MFUNC threshold) {
      if (itout != vecout_end) {
	*itout=(itin-vecin);
	++itout;
      };
    };
    ++itin;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples whose ABSOLUTE values are $MFUNC a certain threshold value and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}Abs
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector which are above threshold")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vecin)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HInteger HFPP_FUNC_NAME (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const T threshold
			 )
{
  // Declaration of variables
  Iter itin(vecin);
  typename vector<HInteger>::iterator itout(vecout);
  HInteger lenIn = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;

  // Sanity check
  if (lenIn != lenOut) {
    throw PyCR::ValueError("Input and output vectors are not of the same size.");
    return 0.;
  }

  // Function operation
  while (itin != vecin_end) {
    if (abs(*itin) HFPP_OPERATOR_$MFUNC threshold) {
      if (itout != vecout_end) {
	*itout=(itin-vecin);
	++itout;
      };
    };
    ++itin;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Count the samples that are $MFUNC a certain threshold value and returns the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const T threshold)
{
  Iter it(vec);
  HInteger count(0);
  while (it<vec_end) {
    if (*it HFPP_OPERATOR_$MFUNC threshold) ++count;
    ++it;
  };
  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples whose absolute values are $MFUNC a certain threshold value and returns the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount{$MFUNC}Abs
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(threshold)()("The threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class T>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const T threshold)
{
  Iter it(vec);
  HInteger count(0);
  while (it<vec_end) {
    if (abs(*it) HFPP_OPERATOR_$MFUNC threshold) ++count;
    ++it;
  };
  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find and return a list containing slice indices (i.e., beginning and end+1 position) of sequences of (almost) consecutive values in a vector which are {$MFUNC} a threshold value
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFindSequence{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of 3 tuples of position, length, and mean value for each sequence found.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vecin)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(threshold)()("Threshold value - if values are $MFUNC this, they can belong to a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(maxgap)()("The maximum gap (in between sample numbers) between two samples to still belong to one sequence. 0=no gaps allowed, i.e. consecutive.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(minlength)()("The minimum length of a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: find, findsequencegreaterthan, findsequencegreaterequal, findsequencelessthan, findsequencelessequal, findsequencebetween, findsequenceoutside, findsequenceoutsideorequal, findsequencebetweenorequal

Example:

#Make a test time series data set for 4 antennas and some peaks at various locations
data=hArray(float,[4,512],name="Random series with peaks")
data.random(0,1024); data[...] += Vector([128.,256., 385.,50.])
for i in range(4): data[i,[2,3,32,64,65,67],...]=Vector([4096.,5097,4096,4096,5099,3096])

datapeaks=hArray(int,[4,256,2],name="Location of peaks")
datamean=data[...].mean()
datathreshold = data[...].stddev(datamean)
datathreshold *= 5
datathreshold += datamean

maxgap=Vector(int,len(datamean),fill=10)
minlength=Vector(int,len(datamean),fill=1)
npeaks=datapeaks[...].findsequencegreaterthan(data[...],datathreshold,maxgap,minlength)
npeaks -> Vec(int,4)=[3,3,3,3]
datapeaks -> hArray(int,[4, 256, 2], name="Location of peaks") # len=2048, slice=[0:512], vec -> [2,4,32,33,64,68,0,0,...]

*/
template <class Iter, class IterI, class T>
HInteger HFPP_FUNC_NAME (const IterI vecout, const IterI vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const T threshold,
			 const HInteger maxgap,
			 const HInteger minlength
			 )
{
  Iter itlast(vecin), itfirst(vecin), itnow(vecin);
  IterI itout(vecout);
  HInteger nsequence(0);
  HBool sequenceon(false);
  if ((itnow>=vecin_end) || (itout+1>=vecout_end)) return 0;
  while (itnow < vecin_end) {
    if (*itnow HFPP_OPERATOR_$MFUNC threshold) {
      if (!sequenceon) {sequenceon=true; itfirst=itnow;}; //Start of new sequence
      ++itnow;
      itlast=itnow; //continue old sequence
    } else {
      ++itnow;
      if (sequenceon) {
	if ((itnow-itlast)>maxgap) { //end of sequence
	  sequenceon=false;
	  if (itlast-itfirst>=minlength) {//Previous sequence long enough, start new sequence and advance output vector
	    *itout=(itfirst-vecin); ++itout; //Store result, begin
	    if (itout!=vecout_end) {*itout=(itlast-vecin); ++nsequence; ++itout;}; //Store result, begin
	    if (itout==vecout_end) return nsequence; //Sorry, end of output vector, return
	  };
	};
      };
    };
  };
  //Include last sequence if necessary
  if (sequenceon && (itlast-itfirst>=minlength)) {
    *itout=(itfirst-vecin); ++itout; //Store result, begin
    if (itout!=vecout_end) {*itout=(itlast-vecin); ++nsequence; ++itout;}; //Store result, begin
    if (itout==vecout_end) return nsequence; //Sorry, end of output vector, return
  };
  return nsequence;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

// ========================================================================
//
//$Section: Statistics
//
// ========================================================================

//$DOCSTRING: Downsample the input vector to a smaller output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Downsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout, const Iter vecout_end,
		     const Iter vecin, const Iter vecin_end)
{
  // Declaration of variables
  HInteger lenIn = vecin_end-vecin;
  HInteger lenOut = vecout_end-vecout;
  HNumber blen;
  HInteger nblock(1);
  Iter it1=vecin, it2;
  Iter itout=vecout, itout_end=vecout_end-1;

  // Sanity check
  if (lenIn <= 0) {
    throw PyCR::ValueError("Size of input vector <= 0.");
    return;
  }
  if (lenOut <= 0) {
    throw PyCR::ValueError("Size of output vector <= 0.");
    return;
  }

  blen = max((((HNumber)lenIn)/((HNumber)lenOut)),1.0);
  //use max to avoid infinite loops if output vector is too large
  //only produce the first N-1 blocks in the output vector
  while ((it1<vecin_end) && (itout<itout_end)) {
    //    it2=min(it1+blen,vecin_end);
    it2=vecin+hfmin((HInteger)(nblock*blen),lenIn);
    *itout=hMean(it1,it2);
    it1=it2;
    ++itout; ++nblock;
  }
  *itout=hMean(it2,vecin_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"




//$DOCSTRING: Downsample the input vector by a certain factor and return a new vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(downsample_factor)()("Factor by which to reduce original size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a new smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.
*/

template <class T>
std::vector<T> HFPP_FUNC_NAME (
		  std::vector<T> & vec,
		  HNumber downsample_factor
		  )
{
  std::vector<T> newvec(floor(vec.size()/downsample_factor+0.5));
  hDownsample(newvec,vec);
  return newvec;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Interpolate a vector between two end points, which are part also start and end points of the new vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInterpolate2P
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Interpolated output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(startvalue)()("Starting value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(endvalue)()("End value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Example:
v=hArray(float, [10])
v.interpolate2p(0.,9.)  #-> [0,1,2,3,4,5,6,7,8,9]
v.interpolate2psubpiece(0.,10.) #-> [0,1,2,3,4,5,6,7,8,9]
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vec, const Iter vec_end,
		     const IterValueType startvalue,
		     const IterValueType endvalue
		     )
{
  HInteger count(0);
  HInteger len = vec_end - vec;
  Iter it(vec);
  HNumber slope = 0.;

  if ( len == 1) {
    *vec=startvalue;
    return;
  } else if (len > 1) {
    slope = (endvalue-startvalue)/(len-1);
    while (it != vec_end) {
      *it=(IterValueType)(startvalue + count*slope);
      ++it; ++count;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Interpolate a vector between two end points, where the 2nd end point marks the last element of the output vector plus one. Useful for piecing interpolations together.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInterpolate2PSubpiece
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Interpolated output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(startvalue)()("Starting value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(endvalue)()("End value of the output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Example:
v=hArray(float, [10])
v.interpolate2p(0.,9.) #-> [0,1,2,3,4,5,6,7,8,9]
v.interpolate2psubpiece(0.,10.) #-> [0,1,2,3,4,5,6,7,8,9]
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,
		     const Iter vec_end,
		     const IterValueType startvalue,
		     const IterValueType endvalue
		     )
{
  HInteger count(0);
  HInteger len = vec_end - vec;
  Iter it(vec);
  HNumber slope = 0.;

  if (len==1) {
    *vec = startvalue;
    return;
  } else if (len > 1) {
    slope = (endvalue-startvalue)/len;
    while (it != vec_end) {
      *it=(IterValueType)(startvalue + count*slope);
      ++it; ++count;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Downsample the input vector to a smaller output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Downsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecrms)()("Output vector containing the RMS in each bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.

     As a second output the root mean square (RMS, standard deviation)
     of the mean in each downsampled bin is returned.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout, const Iter vecout_end,
		     const Iter vecrms, const Iter vecrms_end,
		     const Iter vecin,  const Iter vecin_end)
{
  // Declaration of variables
  HInteger lenIn  = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;
  HInteger lenRMS = vecrms_end - vecrms;
  HNumber blen = 0.;
  HInteger nblock(1);
  Iter it1(vecin), it2;
  Iter itrms(vecrms);
  Iter itout(vecout), itout_end(vecout_end-1);

  // Sanity check
  if (lenIn <= 0) {
    throw PyCR::ValueError("Size of input vector <= 0.");
    return;
  }
  if (lenOut <= 0) {
    throw PyCR::ValueError("Size of output vector <= 0.");
    return;
  }
  if (lenRMS <= 0) {
    throw PyCR::ValueError("Size of rms vector <= 0.");
    return;
  }

  // Function implementation
  blen = max((((HNumber)lenIn)/((HNumber)lenOut)),1.0);
  //use max to avoid infinite loops if output vector is too large
  //only produce the first N-1 blocks in the output vector
  while ((it1<vecin_end) && (itout<itout_end) && (itrms<vecrms_end)) {
    it2=vecin+hfmin((HInteger)(nblock*blen),lenIn);
    *itout=hMean(it1,it2);
    *itrms=hStdDev(it1,it2,*itout);
    it1=it2;
    ++itout;++itrms; ++nblock;
    }
  *itout=hMean(it2,vecin_end);
  *itrms=hStdDev(it2,vecin_end,*itout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Upsample the input vector to a larger output vector by linear interpolation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hUpsample
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Upsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Shorter vector to be upsampled.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Example:
x=hArray(float, [31])
v=hArray(float, [10])
v.interpolate2p(0.,9.) #-> [0,1,2,3,4,5,6,7,8,9]
x.upsample(v)
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,
		     const Iter vecout_end,
		     const Iter vecin,
		     const Iter vecin_end
		     )
{
  // Declaration of variables
  HInteger lenIn  = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;
  HNumber blen = 0.;
  HInteger count(1);
  Iter it1(vecout), it2(vecout),  itin(vecin), itin_end(vecin_end-2);

  if (lenIn <= 1) {
    throw PyCR::ValueError("Size of input vector should be > 1.");
  }
  if (lenOut < lenIn) {
    throw PyCR::ValueError("Size of output vector < size of input vector.");
  }

  blen = max(lenOut/(lenIn-1.0),1.0);
  //use max to avoid infinite loops if input vector is too large
  it2 = it1 + (HInteger)blen;
  //only produce the first N-1 blocks in the input vector
  while ((it2 < vecout_end) && (itin < itin_end)) {
    hInterpolate2PSubpiece(it1,it2,*itin,*(itin+1));
    ++count; it1=it2; it2=vecout+(HInteger)(count*blen); ++itin;
  }
  hInterpolate2P(it1,vecout_end,*itin,*itin+1);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Downsample the input vector to a smaller output vector trying to exclude spikes in the data.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDownsampleSpikyData
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecout)()("Downsampled output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecrms)()("Output vector containgin the RMS in each bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(nsigma)()("Only consider values for averaging that are nsigma above (positive) or below (negative) the mean.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

 Downsample the input vector to a smaller output vector, by replacing
     subsequent blocks of values by their mean value. The block size
     is automatically chosen such that the input vector fits exactly
     into the output vector. All blocks have the same length with a
     possible exception of the last block.

     As a second output the root mean square (RMS, standard deviation)
     of the mean in each downsampled bin is returned.

     The mean here is taken only of data that is nsigma below
     (negative) or above (positive) the RMS in the bin.
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout, const Iter vecout_end,
		     const Iter vecrms, const Iter vecrms_end,
		     const Iter vecin,  const Iter vecin_end,
		     HNumber nsigma
		     )
{
  // Declaration of variables
  HInteger lenIn  = vecin_end - vecin;
  HInteger lenOut = vecout_end - vecout;
  HInteger lenRMS = vecrms_end - vecrms;
  HNumber blen = 0.;
  HInteger nblock(1);
  Iter it1(vecin), it2(vecin);
  Iter itrms(vecrms);
  Iter itout(vecout), itout_end(vecout_end-1);
  HNumber mean = 0.;

  // Sanity check
  if (lenIn <= 0) {
    throw PyCR::ValueError("Size of input vector <= 0.");
    return;
  }
  if (lenOut <= 0) {
    throw PyCR::ValueError("Size of output vector <= 0.");
    return;
  }
  if (lenRMS <= 0) {
    throw PyCR::ValueError("Size of rms vector <= 0.");
    return;
  }

  // Function implementation
  blen = max(((HNumber)lenIn)/((HNumber)lenOut),1.0);
  //use max to avoid infinite loops if output vector is too large
  //only produce the first N-1 blocks in the output vector
  while ((it1<vecin_end) && (itout<itout_end) && (itrms<vecrms_end)) {
    it2=vecin+hfmin((HInteger)(nblock*blen),lenIn);
    mean=hMean(it1,it2);
    *itrms=hStdDev(it1,it2,mean);
    *itout=hMeanThreshold(it1,it2,mean,*itrms,nsigma);
    it1=it2;
    ++itout;++itrms; ++nblock;
  }
  mean=hMean(it2,vecin_end);
  *itrms=hStdDev(it2,vecin_end,mean);
  *itout=hMeanThreshold(it2,vecin_end,mean,*itrms,nsigma);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Finds the location (i.e., returns integer) in a monotonically increasing vector, where the input search value is just above or equal to the value in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFindLowerBound
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Sorted numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(value)()("value to search for")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

Finds -- through a binary search and interpolation -- the location in
  a monotonically increasing vector, where the search value is just
  above or equal to the value in the vector.

This requires random access iterators, in order to have an optimal search result.

*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec, const Iter vec_end,
			 const IterValueType value)
//iterator_traits<Iter>::value_type value)
{
  HNumber value_n=hfcast<HNumber>(value); //This also works for Complex then
  HInteger niter=0;
  if (vec==vec_end) return 0;
  HInteger maxpos=vec_end-vec-1,posguess;
  Iter it1=vec,it2=vec_end-1,it0;
  if (value_n<=hfcast<HNumber>(*it1)) return 0;
  if (value_n>=hfcast<HNumber>(*it2)) return maxpos;
  posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*maxpos;
  it0=vec+posguess;

  if (it0 < it1) {
    throw PyCR::ValueError("Not a monotonically increasing vector.");
    return hfcast<HInteger>(it1-vec);
  }
  if (it0 >= it2) {
    throw PyCR::ValueError("Not a monotonically increasing vector.");
    return hfcast<HInteger>(it2-vec);
  }
  //  cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
  while (!((value_n < hfcast<HNumber>(*(it0+1))) && (value_n >= hfcast<HNumber>(*it0)))) {
    if (value_n > hfcast<HNumber>(*it0)) {
      it1=it0;
    } else {
      it2=it0;
    };
    it0=it1+(it2-it1)/2;
    if (*it0>value_n) it2=it0; //Binary search step
    else it1=it0;
    posguess=(value_n-hfcast<HNumber>(*it1))/(hfcast<HNumber>(*it2)-hfcast<HNumber>(*it1))*(it2-it1)+(it1-vec);
    it0=vec+posguess;
    ++niter;
    //cout << "hFindLowerBound(" << value_n << "): niter=" << niter << ", posguess=" << posguess << ", val=" << *it0 << " vals=(" << hfcast<HNumber>(*(it0)) << ", " << hfcast<HNumber>(*(it0+1)) << "), bracket=(" << it1-vec << ", " << it2-vec <<")" <<endl;
    if (it0 < it1) {
      throw PyCR::ValueError("Not a monotonically increasing vector.");
      return it1-vec;
    }
    if (it0 > it2) {
      throw PyCR::ValueError("Not a monotonically increasing vector.");
      return it2-vec;
    }
  };
  return posguess;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by hRunningAverageT.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFlatWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  // Sanity Check
  if (wlen <= 0) {
    throw PyCR::ValueError("Length of weight vector should be > 0.");
  }

  std::vector<HNumber> weights(wlen,1.0/wlen);
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING:  Returns vector of weights of length wlen with linearly rising and decreasing weights centered at len/2.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

The vector is normalized to give a sum of unity. Can be used by
hRunningAverage.
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  // Sanity check
  if (wlen <= 0) {
    throw PyCR::ValueError("Length of weight vector should be > 0.");
  }

  std::vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;

  for (i=0; i<wlen; i++) {
    f=1.0-abs(middle-i)/(middle+1.0);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  if (abs(sum) < A_LOW_NUMBER) {
    throw PyCR::ZeroDivisionError("Sum value is 0.");
  } else {
    for (i=0; i<wlen; i++) weights[i] /= sum;
  }
  return weights;
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Returns vector of weights of length wlen with Gaussian distribution centered at len/2 and sigma=len/4 (i.e. the Gaussian extends over 2 sigma in both directions).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGaussianWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>


std::vector<HNumber> HFPP_FUNC_NAME (HInteger wlen) {
  // Sanity check
  if (wlen <= 0) {
    throw PyCR::ValueError("Length of weight vector should be > 0.");
  }

  vector<HNumber> weights(wlen,0.0);
  HInteger i,middle=wlen/2;
  HNumber f,sum=0.0;

  for (i=0; i<wlen; i++) {
    f=funcGaussian(i,max(wlen/4.0,1.0),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  if (abs(sum) < A_LOW_NUMBER) {
    throw PyCR::ZeroDivisionError("Sum value is 0.");
  } else {
    for (i=0; i<wlen; i++) weights[i] /= sum;
  }

  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a normalized weight vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
vector<HNumber> HFPP_FUNC_NAME (const HInteger wlen, const hWEIGHTS wtype){
  vector<HNumber> weights;
  HInteger wlen2(wlen);
  if (wlen2<1) wlen2=1;
  switch (wtype) {
  case WEIGHTS_LINEAR:
    weights=hLinearWeights(wlen2);
    break;
  case WEIGHTS_GAUSSIAN:
    weights=hGaussianWeights(wlen2);
    break;
  default: //  WEIGHTS_FLAT:
    weights=hFlatWeights(wlen2);
  };
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Calculate the running average of an input vector using a weight vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(weights)()("Weight vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class DataIter, class NumVecIter>
void HFPP_FUNC_NAME (const DataIter  odata,
		     const DataIter  odata_end,
		     const DataIter  idata,
		     const DataIter  idata_end,
		     const NumVecIter weights,
		     const NumVecIter weights_end)
{
  HInteger l=(weights_end-weights);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  HInteger middle=l/2;
  /* To avoid too many rounding errors with Integers */
  //  typename DataIter::value_type fac = l*10;
  HNumber temp;

  DataIter  dit;
  DataIter  dit2;
  DataIter  din(idata);
  DataIter  dout(odata);
  NumVecIter wit;

  while (din<idata_end && dout<odata_end) {
    dit=din-middle; //data iterator set to the first element to be taken into account (left from current element)
    wit=weights; // weight iterators set to beginning of weights
    temp=0.0;
    while (wit<weights_end) {
      if (dit<idata) dit2=idata;
      else if (dit>=idata_end) dit2=idata_end-1;
      else dit2=dit;
      temp=temp+(*dit2)*(*wit);
      ++dit; ++wit;
    };
    *dout=temp;
    ++dout; ++din; //point to the next element in data input and output vector
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Overloaded function to automatically calculate weights.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(idata)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(wlen)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (hWEIGHTS)(wtype)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:

  in_array.runningaverage(array_out,7,hWEIGHTS.GAUSSIAN)

  Available Weights:

  FLAT:  all have the same value
  LINEAR: linearly rising, peaking at the center (i.e., /\)
  GAUSSIAN: Gaussian distribution falling of to 2 sigma at the ends
*/
template <class DataIter>
void HFPP_FUNC_NAME (const DataIter odata,
		     const DataIter odata_end,
		     const DataIter idata,
		     const DataIter idata_end,
		     const HInteger wlen,
		     const hWEIGHTS wtype)
{
  vector<HNumber> weights = hWeights(wlen, wtype);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (odata,
							odata_end,
							idata,
							idata_end,
							weights.begin(),
							weights.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the square of the absolute value and add to output vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAbsSquareAdd
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Input vector containing complex values. (Looping parameter)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an hArray) is here determined by
  the second parameter!

Example:
spectrum=hArray(float,[1,128])
cplxfft=hArray(complex,[10,128],fill=1+0j)
spectrum[...].spectralpower(cplxfft[...])
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  // Declaration of variables
  Iterin itin(vecin);
  Iter itout(vecout);

  // Calculation of spectral power
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout+=real((*itin)*conj(*itin));
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Compares two vectors for equality
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hEqual
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec0)()("Primary vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec1)()("Secondary vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
HBool HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end)
{
  // Are both referencing the same vector?
  if (vec0 == vec1 && vec0_end == vec1_end)
  {
    return true;
  }

  // Are they the same length?
  const int N0 = std::distance(vec0, vec0_end);
  const int N1 = std::distance(vec1, vec1_end);

  if (N0 != N1)
  {
    return false;
  }

  // Declaration of variables
  Iter it0(vec0);
  Iter it1(vec1);

  // Compare values
  while (it0 != vec0_end)
  {
    if (*it0 != *it1)
    {
      return false;
    }

    ++it0;
    ++it1;
  }

  return true;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Compares two vectors for equality up to a given number of decimal places.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAlmostEqual
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec0)()("Primary vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec1)()("Secondary vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(decimal)()("Desired number of decimal places for equality.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
HBool HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end, const HInteger decimal)
{
  // Are both referencing the same vector?
  if (vec0 == vec1 && vec0_end == vec1_end)
  {
    return true;
  }

  // Are they the same length?
  const int N0 = std::distance(vec0, vec0_end);
  const int N1 = std::distance(vec1, vec1_end);

  if (N0 != N1)
  {
    return false;
  }

  // Declaration of variables
  Iter it0(vec0);
  Iter it1(vec1);

  const double cmp = 0.5 * pow(10.0, -1.0 * decimal);

  // Compare values
  while (it0 != vec0_end)
  {
    if (abs((*it0) - (*it1)) >= cmp)
    {
      return false;
    }

    ++it0;
    ++it1;
  }

  return true;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

