/**************************************************************************
 *  Math module for CR Pipeline Python bindings.                          *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
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
#include "mVector.h"
#include "mMath.h"

#include <iostream>

//#include "casa/BasicSL/Constants.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;
//using namespace casa;

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


//$DOCSTRING: Returns the square root of the absolute of a number.
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
  >>> sqrtAbs(-4)
  2
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
  return exp(-(x-mu)*(x-mu)/(2*sigma*sigma))/(sigma*sqrt(2*M_PI));
};
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Return the maximum value in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMax
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.max() -> maximum value
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  if (vec_end-vec <= 0) ERROR_RETURN_VALUE("Illegal size of input vector (<=0)",0);
  Iter it(vec);
  IterValueType val=*it;
  while (it!=vec_end) {
    if (*it > val) val=*it;
    ++it;
  };
  return val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the maximum difference between values in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaxDiff
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.max() -> maximum value
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  IterValueType min, max;

  Iter it(vec);

  min = *it;
  max = *it;

  it++;
  while (it != vec_end)
  {
    if (*it > max)
    {
      max = *it;
    }
    else if (*it < min)
    {
      min = *it;
    }

    it++;
  }

  return max - min;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Return the minimum value in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMin
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.max() -> minimum value
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  if (vec_end-vec <= 0) ERROR_RETURN_VALUE("Illegal size of input vector (<=0)",0);
  Iter it(vec);
  IterValueType val=*it;
  while (it!=vec_end) {
    if (*it < val) val=*it;
    ++it;
  };
  return val;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: For each element in a vector replace its value by the maximum of the element value and an input value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMax
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(max_value)()("Maximum value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.max(max_value) -> all values in vec are larger (or equal) than max_value
  v=hArray(range(5))
  v.max(3) -> [3,4]
*/
template <class Iter, class T>
void hMax(const Iter vec,const Iter vec_end, const T max_value)
{
  Iter it(vec);
  IterValueType val(hfcast<IterValueType>(max_value));
  while (it!=vec_end) {
    if (*it < val) *it=val;
    ++it;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: For each element in a vector replace its value by the minimum of the element value and an in input value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMin
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(min_value)()("Minimum value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.min(min_value) -> all values in vec are smaller (or equal) than min_value

  Example:
  >>> v=hArray(range(5))
  >>> v.min(3)
  [0,1,2,3]
*/
template <class Iter, class T>
void hMin(const Iter vec,const Iter vec_end, const T min_value)
{
  Iter it(vec);
  IterValueType val(hfcast<IterValueType>(min_value));
  while (it!=vec_end) {
    if (*it > val) *it=val;
    ++it;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: For each element in a vector replace its value by the minimum of the element value in the vector itself and the element value in the second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinimum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec0)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end)
{
  Iter it0(vec0);
  Iter it1(vec1);

  while (it0!=vec0_end && it1!=vec1_end) {
    if (*it1 < *it0) *it0=*it1;
    ++it0; ++it1;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: For each element in a vector replace its value by the minimum of the element value in the vector itself and the element value in the second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinimum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec0)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_1)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end, const Iter vec2, const Iter vec2_end)
{
  Iter it0(vec0);
  Iter it1(vec1);
  Iter it2(vec2);

  while (it0!=vec0_end && it1!=vec1_end && it2!=vec2_end) {
    *it0 = *it2 < *it1 ? *it2 : *it1;

    ++it0; ++it1; ++it2;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: For each element in two input vectors get the minimum and maximum in the output vectors
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinimumAndMaximum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec0)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_1)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_1)(vec3)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end, const Iter vec2, const Iter vec2_end, const Iter vec3, const Iter vec3_end)
{
  Iter it0(vec0);
  Iter it1(vec1);
  Iter it2(vec2);
  Iter it3(vec3);

  while (it0!=vec0_end && it1!=vec1_end && it2!=vec2_end && it3!=vec3_end) {
    if (*it3 < *it2)
    {
      *it0 = *it3;
      *it1 = *it2;
    }
    else
    {
      *it0 = *it2;
      *it1 = *it3;
    }
    ++it0; ++it1; ++it2; ++it3;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: For each element in a vector replace its value by the maximum of the element value in the vector itself and the element value in the second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaximum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec0)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_1)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end, const Iter vec2, const Iter vec2_end)
{
  Iter it0(vec0);
  Iter it1(vec1);
  Iter it2(vec2);

  while (it0!=vec0_end && it1!=vec1_end && it2!=vec2_end) {
    *it0 = *it2 > *it1 ? *it2 : *it1;

    ++it0; ++it1; ++it2;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: For each element in a vector replace its value by the maximum of the element value in the vector itself and the element value in the second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaximum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec0)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec0, const Iter vec0_end, const Iter vec1, const Iter vec1_end)
{
  Iter it0(vec0);
  Iter it1(vec1);

  while (it0!=vec0_end && it1!=vec1_end) {
    if (*it1 > *it0) *it0=*it1;
    ++it0; ++it1;
  };
  return;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the position of the maximum value in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaxPos
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.maxpos() -> i # position of maximum value
*/
template <class Iter>
HInteger HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  if (vec_end-vec <= 0) ERROR_RETURN_VALUE("Illegal size of input vector (<=0)",0);
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


//$DOCSTRING: Return the position of the minimum value in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinPos
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.maxpos() -> i # position of minimum value
*/
template <class Iter>
HInteger HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  if (vec_end-vec <= 0) ERROR_RETURN_VALUE("Illegal size of input vector (<=0)",0);
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.$MFUNC() -> $MFUNC operation on vec.
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec2.$MFUNC(vec1) -> return result of $MFUNC operation on vec1 in vec2.
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


//$DOCSTRING: Raises the values in the vector to a power N
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPow
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(exponent)()("Value containing the power")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:

  vec.pow(N) -> [vec_0**N, vec_1**N, ....]

  Example:

  a=hArray(float,5,fill=[0,1,2,3,4])
  a.pow(3)
  a  # -> hArray(float, [5L], fill=[0,1,8,27,64]) # len=5 slice=[0:5])
*/
template <class Iter, class S>
void hPow(const Iter vec1,const Iter vec1_end, const S exponent)
{
      // Declaration of variables
      Iter it=vec1;

      // Vector operation
      while (it!=vec1_end) {
        *it = pow(*it,exponent);
        ++it;
      };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================

//$DOCSTRING: Performs a $MFUNC between the two vectors, returning the output in the second vector (non-standard behaviour). If the first vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}To
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input  vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operand and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hMulTo(vecA,vecB) -> vecB=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  hAddTo(vecA,vecB) -> vecB=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  hSubTo(vecA,vecB) -> vecB=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  hDivTo(vecA,vecB) -> vecB=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]


  Example:
  ::
      #Example is for adding operation, mul, div, sub are similar
      a1=hArray(int,[3,5],fill=range(5))
      #a1->hArray(int, [3L, 5L], fill=[0,1,2,3,4,0,1,2,3,4,0,1,2,3,4]) # len=15 slice=[0:15])

      a2=hArray(int,[5],fill=0)
      a1[...].addto(a2)

      #a2 -> hArray(int, [5L], fill=[0,3,6,9,12]) # len=5 slice=[0:5])

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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vecA *= vecB  -> vecA=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  vecA += vecB  -> vecA=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  vecA -= vecB  -> vecA=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  vecA /= vecB  -> vecA=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]

  Example:
  >>> hMul(vecA,vecB) = vecA.mul(vecB)
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

//$DOCSTRING: Performs a $MFUNC (in-place) between the two vectors, which is returned in the first vector. If the first vector is shorter it will be wrapped unitl the end of the second vector is reached. Can, e.g., be used to calculate sums/products of vectors.

//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Vec
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vecA *= vecB  -> vecA=[ vecA[0]*vecB[0], vecA[1]*vecB[1], ..., vecA[n]*vecB[n] ]
  vecA += vecB  -> vecA=[ vecA[0]+vecB[0], vecA[1]+vecB[1], ..., vecA[n]+vecB[n] ]
  vecA -= vecB  -> vecA=[ vecA[0]-vecB[0], vecA[1]-vecB[1], ..., vecA[n]-vecB[n] ]
  vecA /= vecB  -> vecA=[ vecA[0]/vecB[0], vecA[1]/vecB[1], ..., vecA[n]/vecB[n] ]

  hMul(vecA,vecB) = vecA.mul(vecB)

  Example:

    v1=hArray(float,[3],fill=0)
    v2=hArray(float,[3,3],fill=range(9))
    v1.addvec(v2)

*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iterin vec2,const Iterin vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iter it1=vec1;
  Iterin it2=vec2;

  if ((vec1_end-vec1) <= 0) ERROR_RETURN("Size of 2nd vector is <= 0.");
  if ((vec2_end-vec2) < 0) ERROR_RETURN("Invalid size of 1st vector.");

  // Vector operation
  while (it2!=vec2_end) {
    *it1 =  hfcast<T>(*it1 HFPP_OPERATOR_$MFUNC (*it2));
    ++it1; ++it2;
    if (it1==vec1_end) it1=vec1;
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



//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:

  Performs a $MFUNC!LOW between the last two vectors, which is
  returned in the first vector. If the second operand vector is
  shorter it will be applied multiple times.

  Usage:
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
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


//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, and add the result to the first vector which can be of different types.
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Performs a $MFUNC!LOW between the last two vectors, and add the
  result to the first vector which can be of different types. Looping
  will be done over the first argument, i.e. the input/output
  vector. If the second operand vector is shorter it will be applied
  multiple times.

  To loop slices over the second argument (i.e., ``vec1``) use
  ``h$MFUNC!CAPSAdd2``. To repeatedly add to the output vector, use
  ``h$MFUNC!CAPSAddSum``.

  Usage:
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  See also:
  h$MFUNC!CAPSAdd2, h$MFUNC!CAPSAddSum
*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;

  if (itout>=vec_end) return;
  if (it1>=vec1_end) return;
  if (it2>=vec2_end) return;

  // Vector operation
  while ((it1!=vec1_end)  && (itout !=vec_end)) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a $MFUNC!LOW between the input vector and a scalar, and add the result to the first vector which can be of different type. Looping will be done over the first argument, i.e. the output vector. If the second operand vector is shorter it will be applied multiple times.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Add
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(val)()("Scalar value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  To loop slicec over the second argument (i.e., ``vec1``) use
  ``hMulAdd2``, ``hDivAdd2``, ``hSubAdd2``, ``hAddAdd2``. To repeatedly
  add to the output vector, use `hMulAddSum``.

  Usage:
  h$MFUNC(vec,vec1,val) -> vec = vec1 $MFUNC!LOW val
  vec.$MFUNC(vec1,val) -> vec = vec1 $MFUNC!LOW val

  See also:
  hMulAdd2, hDivAdd2, hSubAdd2, hAddAdd2,  hMulAddSum, hDivAddSum, hSubAddSum, hAddAddSum
*/
template <class Iter, class Iterin1, class S>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const S val)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iter itout=vec;

  if (itout>=vec_end) return;
  if (it1>=vec1_end) return;

  // Vector operation
  while (itout !=vec_end) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (val));
    ++it1; ++itout;
    if (it1==vec1_end) it1=vec1;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, and add the result to the first vector which can be of different type.
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  Description:

  Slice looping will be done over the second argument, i.e. vec1. If
  the second operand vector (vec2) is shorter it will be applied
  multiple times.  To loop over the first argument (i.e., ``vec``) use
  ``h$MFUNC!CAPSAdd``. To repeatedly add to the output vector, use
  ``h$MFUNC!CAPSAddSum``.

  See also:
  h$MFUNC!CAPSAdd, h$MFUNC!CAPSAddSum
*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;

  if (itout>=vec_end) return;
  if (it1>=vec1_end) return;
  if (it2>=vec2_end) return;

  // Vector operation
  while ((it1!=vec1_end) && (itout!=vec_end)) {
    *itout += hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
    if (it2==vec2_end) it2=vec2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs a $MFUNC!LOW between the last two vectors, and add the result to the first vector which can be of different type. Wrap and repeatedly add to input array if shorter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}AddSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector containing the result of operation")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec1)()("Vector containing the first operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the second operand")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec,vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2
  vec.$MFUNC(vec1,vec2) -> vec = vec1 $MFUNC!LOW vec2

  Description:

  Slice looping will be done over the first argument, i.e. the output
  vector. If the output vector is shorter than the first operand
  vector it will be wrapped, i.e. the additional slices in vec1 will
  repeatedly be added to the output vector. Also the second operand
  will be wrapped. To not loop over the first argument (i.e., ``vec``)
  use ``h$MFUNC!CAPSAdd``.

  See also:
  h$MFUNC!CAPSAdd2
*/
template <class Iter, class Iterin1, class Iterin2>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iterin1 vec1,const Iterin1 vec1_end, const Iterin2 vec2,const Iterin2 vec2_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iterin1 it1=vec1;
  Iterin2 it2=vec2;
  Iter itout=vec;

  if (itout>=vec_end) return;
  if (it1>=vec1_end) return;
  if (it2>=vec2_end) return;

  // Vector operation
  while ((it1!=vec1_end)) {
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec,vec1,scalar) -> vec = vec1 $MFUNC!LOW scalar
  vec.$MFUNC(vec1,scalar) -> vec = vec1 $MFUNC!LOW scalar
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


//$DOCSTRING: Performs a ``$MFUNC!LOW`` operation (i.e., val ``+/-*`` ``vec``) in place between a scalar and a vector, where the scalar is the first argument.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}Self
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Scalar value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec,val) -> vec = val $MFUNC!LOW vec
  vec.$MFUNC(val) -> vec = val $MFUNC!LOW vec

  Description:
  It does not really make sense for ``add`` and ``mul``, but is useful to get the inverse of a vector.

  See also:
  h$MFUNC!CAPSSelf, hInverse

  Example:
  >>> vec=Vector([1.,2.,4.])
  >>> vec.divself(1)
  [1.0,0.5,0.25]
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
//$ITERATE MFUNC Mul,Add
//========================================================================


//$DOCSTRING: Creates a power law of the form amplitude*x_i^/alpha and multiplies or adds it (here $MFUNC) to a vector, where the x_i's are given in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPowerLaw{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric output vector containing the powerlaw")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(xvec)()("The x values to be potentiated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(amplitude)()("Amplitude of the powerlaw")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_4)(exponent)()("Exponent of the powerlaw")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
      vec.powerlawmul(xvec,A,alpha) -> vec=[vec_0*A*xvec_0^alpha,vec_1*A*xvec_1^alpha,...,vec_n*A*xvec_n^alpha ]
      vec.powerlawadd(xvec,A,alpha) -> vec=[vec_0+A*xvec_0^alpha,vec_1+A*xvec_1^alpha,...,vec_n+A*xvec_n^alpha ]

  Description:
  If the second vector is shorter, it will wrap around.

  See also:
  hPowerlawMul, hPowerlawAdd, hLinearFunctionMul, hLinearFunctionAdd, hLogLinearFunctionMul, hLogLinearFunctionAdd

  Example:
  ary=hArray(float,5,fill=1); x=hArray(range(5)); ary.powerlawmul(x,0.5,2)

  #ary -> hArray(float, [5L], fill=[0,0.5,2,4.5,8]) # len=5 slice=[0:5]
*/
template <class Iter,class Iter2, class S, class T>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const Iter2 xvec, const Iter2 xvec_end, const S amplitude, const T exponent)
{
      // Declaration of variables
      typedef IterValueType U;
      Iter it=vec;
      Iter2 itx=xvec;

      // Sanity check
      HInteger l=vec_end - vec;
      if (l<0) ERROR_RETURN("Illegal size of vec");
      if (l==0) return;

      l=xvec_end - xvec;
      if (l<0) ERROR_RETURN("Illegal size of xvec");
      if (l==0) return;

      // Vector operation
      while (it != vec_end) {
        *it HFPP_OPERATOR_INPLACE_$MFUNC hfcast<U>(amplitude*pow(*itx,exponent));  // operator_inplace is *= or += here
        ++it; ++itx;
        if (itx==xvec_end) itx=xvec;
      };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Creates a linear function of the form a*x_i+b and multiplies or adds it (here $MFUNC) to a vector, where the x_i's are given in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFunction{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric output vector containing the powerlaw")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(xvec)()("The x values to be potentiated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(a)()("Offset along y-axis of function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_4)(m)()("Slope of linear function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
      vec.linearfunctionmul(xvec,a,m) -> vec=[vec_0*(a+xvec_0*m),vec_1*(a+xvec_1*m),...,vec_n*(a+xvec_n*m)]
      vec.linearfunctionadd(xvec,a,m) -> vec=[vec_0+a+xvec_0*m,vec_1+a+xvec_1*m,...,vec_n+a+xvec_n*m]

  Description:
  If the second vector is shorter, it will wrap around.

  See also:
  hPowerlawMul, hPowerlawAdd, hLinearFunctionMul, hLinearFunctionAdd, hLogLinearFunctionMul, hLogLinearFunctionAdd

  Example:
  ary=hArray(float,5,fill=1); x=hArray(range(5)); ary.powerlawmul(x,0.5,2)

  #ary -> hArray(float, [5L], fill=[0,0.5,2,4.5,8]) # len=5 slice=[0:5]
*/
template <class Iter,class Iter2, class S, class T>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const Iter2 xvec, const Iter2 xvec_end, const S a, const T m)
{
      // Declaration of variables
      typedef IterValueType U;
      Iter it=vec;
      Iter2 itx=xvec;

      // Sanity check
      HInteger l=vec_end - vec;
      if (l<0) ERROR_RETURN("Illegal size of vec");
      if (l==0) return;

      l=xvec_end - xvec;
      if (l<0) ERROR_RETURN("Illegal size of xvec");
      if (l==0) return;

      // Vector operation
      while (it != vec_end) {
        *it HFPP_OPERATOR_INPLACE_$MFUNC hfcast<U>(a+m*(*itx));  // operator_inplace is *= or += here
        ++it; ++itx;
        if (itx==xvec_end) itx=xvec;
      };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Creates a log linear function of the form a*log(x_i)+b and multiplies or adds it (here $MFUNC) to a vector, where the x_i's are given in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLogLinearFunction{$MFUNC}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric output vector containing the powerlaw")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(xvec)()("The x values to be potentiated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(a)()("Offset along y-axis of function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HFPP_TEMPLATED_4)(m)()("Slope of linear function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
      vec.linearfunctionmul(xvec,a,m) -> vec=[vec_0*(a+xvec_0*m),vec_1*(a+xvec_1*m),...,vec_n*(a+xvec_n*m)]
      vec.linearfunctionadd(xvec,a,m) -> vec=[vec_0+a+xvec_0*m,vec_1+a+xvec_1*m,...,vec_n+a+xvec_n*m]

  Description:

  This is quite useful for plotting a log linear graph of a powerlaw:
  log10(a*\nu^\alpha) = (ln(a)+\alpha*ln(\nu))/ln(10.)


  If the second vector is shorter, it will wrap around.


  See also:
  hPowerlawMul, hPowerlawAdd, hLinearFunctionMul, hLinearFunctionAdd, hLogLinearFunctionMul, hLogLinearFunctionAdd

  Example:
  ary=hArray(float,5,fill=1); x=hArray(range(5)); ary.powerlawmul(x,0.5,2)

  #ary -> hArray(float, [5L], fill=[0,0.5,2,4.5,8]) # len=5 slice=[0:5]
*/
template <class Iter,class Iter2, class S, class T>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const Iter2 xvec, const Iter2 xvec_end, const S a, const T m)
{
      // Declaration of variables
      typedef IterValueType U;
      Iter it=vec;
      Iter2 itx=xvec;

      // Sanity check
      HInteger l=vec_end - vec;
      if (l<0) ERROR_RETURN("Illegal size of vec");
      if (l==0) return;

      l=xvec_end - xvec;
      if (l<0) ERROR_RETURN("Illegal size of xvec");
      if (l==0) return;

      // Vector operation
      while (it != vec_end) {
        *it HFPP_OPERATOR_INPLACE_$MFUNC hfcast<U>(a+m*log(*itx));  // operator_inplace is *= or += here
        ++it; ++itx;
        if (itx==xvec_end) itx=xvec;
      };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE


//$DOCSTRING: Take the inverse of the values in a vector, i.e. 1/vec
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInverse
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hInverse(vec) -> 1/vec
  vec.inverse(val) -> 1/vec

  See also:
  hMulSelf, hDivSelf, hSubSelf, hAddSelf

  Example:
  >>> vec=Vector([1.,2.,4.])
  >>> vec.inverse()
  [1.0,0.5,0.25]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  Iter it(vec);
  if (it>vec_end) {return;}
  while (it!=vec_end) {
    *it = 1/(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec, vecpar) -> vec = $MFUNC!LOW(vec, vecpar)
  vec.$MFUNC(vecpar) -> vec = $MFUNC!LOW(vecin, vecpar)
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec, vecin, param) -> vec = $MFUNC!LOW(vecin, param)
  vec.$MFUNC(vecin, param) -> vec = $MFUNC!LOW(vecin, param)
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  h$MFUNC(vec, vecin, vecpar) -> vec = $MFUNC!LOW(vecin, vecpar)
  vec.$MFUNC(vecin, vecpar) -> vec = $MFUNC!LOW(vecin, vecpar)
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
  return 2*M_PI*frequency*time;
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
  return polar(NUMBER_ONE, phase); // exp(HComplex(0.0,phase));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Converts a vector of real phase to a vector of corresponding complex numbers (with amplitude of unity).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPhaseToComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector returning complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(phasevec)()("Input vector with real phases")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  If input vector is shorter it will be repeated until the output vector is filled.

  Usage:
  hPhaseToComplex(outvec,phase) -> outvec = [exp(i*phase_1),exp(i*phase_2),...,exp(i*phase_n)]
  outvec.phasetocomplex(phase) -> outvec = [exp(i*phase_1),exp(i*phase_2),...,exp(i*phase_n)]
*/
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 vec, const Iter1 vec_end, const Iter2 phasevec, const Iter2 phasevec_end)
{
  // Declaration of variables
  Iter1 it1=vec;
  Iter2 it2=phasevec;
  HInteger lenOut = vec_end - vec;
  HInteger lenPhase = phasevec_end - phasevec;

  // Sanity check
  if ((lenPhase <= 0) || (lenOut <= 0)) {
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


//$DOCSTRING: Extract the phase of a complex number (i.e. get phi from z = r exp(i phi) )
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hComplexToPhase
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(z)()("Complex number to extract phase from")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
 */
HNumber HFPP_FUNC_NAME(const HComplex z)
{
    return arg(z);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Converts a vector of real phase to a vector of corresponding complex numbers (with amplitude of unity).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hComplexToPhase
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(phasevec)()("Output vector returning (real) phases")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vec)()("Input vector with complex numbers")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

 Description:
 If input vector is shorter it will be repeated until the output vector is filled.

 Usage:
 hComplexToPhase(outvec, complex_in) -> outvec = [arg(z_0), arg(z_1),..., arg(z_{n-1}]
 outvec.complextophase(complex_in) -> outvec = [arg(z_0), arg(z_1),..., arg(z_{n-1})]
 */
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 phasevec, const Iter1 phasevec_end, const Iter2 vec, const Iter2 vec_end)
{
    // Declaration of variables
    Iter1 it1 = phasevec;
    Iter2 it2 = vec;
    HInteger lenOut = phasevec_end - phasevec;
    HInteger lenPhase = vec_end - vec;

    // Sanity check
    if ((lenPhase <= 0) || (lenOut <= 0)) {
        throw PyCR::ValueError("Illegal size of phase vector");
    }

    // Vector operation
    while (it1 != phasevec_end) {
        *it1=hComplexToPhase(*it2);
        ++it1;
        ++it2; if (it2==vec_end) it2=vec; // loop over input vector if necessary ...
    };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Extract the phase of a complex number (i.e. get phi from z = r exp(i phi) )
//$COPY_TO HFILE START ---------------------------------------------------
#define HFPP_FUNC_NAME hPhaseWrap
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(phase)()("Phase to wrap into (-pi, pi]")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
 */
HNumber HFPP_FUNC_NAME(const HNumber phase)
{ // Wrap positive or negative phase into interval (-pi, pi] by subtracting multiples of 2*pi
    return phase - 2*M_PI * floor( (phase + M_PI) / (2*M_PI) );
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Converts a vector of real phase to a vector of corresponding complex numbers (with amplitude of unity).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPhaseWrap
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(wrappedvec)()("Output vector returning wrapped phases")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vec)()("Input vector with phases")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING

 Description:
 If input vector is shorter it will be repeated until the output vector is filled.

 Usage:
 hPhaseWrap(outvec, phase_in) -> outvec = [wrap(phi_0), wrap(phi_1),..., wrap(phi_{n-1}]
 outvec.phasewrap(phase_in) -> outvec = [wrap(phi_0), wrap(phi_1),..., wrap(phi_{n-1})]
 */
template <class Iter1,class Iter2>
void  HFPP_FUNC_NAME(const Iter1 wrappedvec, const Iter1 wrappedvec_end, const Iter2 vec, const Iter2 vec_end)
{
    // Declaration of variables
    Iter1 it1 = wrappedvec;
    Iter2 it2 = vec;
    HInteger lenOut = wrappedvec_end - wrappedvec;
    HInteger lenPhase = vec_end - vec;

    // Sanity check
    if ((lenPhase <= 0) || (lenOut <= 0)) {
        throw PyCR::ValueError("Illegal size of phase vector");
    }

    // Vector operation
    while (it1 != wrappedvec_end) {
        *it1 = hPhaseWrap(*it2);
        ++it1;
        ++it2; if (it2==vec_end) it2=vec; // loop over input vector if necessary ...
    };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Coverts a real phase and amplitude to a complex number.
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
#define HFPP_PARDEF_1 (HNumber)(ampphase)()("Input vector with real amplitudes and phases (2 numbers per entry: ``[amp_0, phase_0, amp_1, phase_1, ...]``)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  If input vector is shorter it will be repeated until output vector is filled.

  Usage:
  hAmplitudePhaseToComplex(vec,ampphase) -> vec = [ampphase_0,0*exp(i*ampphase_0,1),ampphase_1,0*exp(i*ampphase_1,1),...,ampphase_n,0*exp(i*ampphase_n,1)]
  vec.amplitudephasetocomplex(ampphase) -> vec = [ampphase_0,0*exp(i*ampphase_0,1),ampphase_1,0*exp(i*ampphase_1,1),...,ampphase_n,0*exp(i*ampphase_n,1)]
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
#define HFPP_PARDEF_2 (HBool)(fftw)()("Was the FFT calculated with FFTw definition? Then muliply one fft vector by -1,1,-1,...")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  fftvec1.crosscorrelatecomplex(fftvec2,True) -> fftvec1 return FFT(w) of cross correlation between

  Description:

  If the complex input vectors are the Fourier transformed data of two
  (real) vector ``f1`` and ``f2``, then ``vec1*conj(vec2)`` will be
  the Fourier transform of the crosscorrelation between ``f1`` and
  ``f2``.  Hence, in order to calculate a cross correlation between
  ``f1`` and ``f2``, first do ``vec1.fftw(f1)`` and ``vec2.fftw(f2)``,
  then ``vec1.crosscorrelatecomplex(vec2,True)`` and FFT back through
  ``floatvec.invfft(vec1)``.

  For zero lag (e.g. autocorrelation), the cross-correlation will peak
  in the middle of the float vector.

  If the real input vector has length ``N`` then the complex FFT
  vectors have dimension ``N/2+1``.

  There is a difference in defintion between ffts (here fftw and
  fftcasa). If the fftw definition is used, then one of the FFT data
  vectors still needs to be multiplied by -1,1,-1,... to get the peak
  of the crosscorrelation for lag = 0 in the middle of floatvec. This
  makes sense since the lag can be positive or negative.

  Note:

  If the second vector is shorter than the first one, the second vector
  will simply wrap around and begin from the start until the end of the
  first vector is reached. If the first vector is shorter, then the
  calculation will simply stop.

  See also:

  hFFTw,  hInvFFTw,  hSaveInvFFTw, :class:`tasks.pulsecal.CrossCorrelateAntennas`

  Example:
  ::
      file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
      file["BLOCKSIZE"]=131072
      file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
      timeseries_data=file["TIMESERIES_DATA"]
      timeseries_data_cut=hArray(float,[48,64]) # just look at the region with a pulse
      timeseries_data_cut[...].copy(timeseries_data[...,65806:65870])
      fftdata=hArray(complex,[48,64/2+1])
      fftdata[...].fftw(timeseries_data_cut[...])
      fftdata[1:,...].crosscorrelatecomplex(fftdata[0],True) # to make sure the reference data is not overwritten start at index 1
      fftdata[0:1,...].crosscorrelatecomplex(fftdata[0],True) # only now do the autocorrelation
      crosscorrelation=hArray(properties=timeseries_data_cut)
      crosscorrelation[...].invfftw(fftdata[...])
      crosscorrelation /= 64
      crosscorrelation.abs()
      crosscorrelation[...].runningaverage(15,hWEIGHTS.GAUSSIAN)
      crosscorrelation[0:4,...].plot()

*/

template <class Iter>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end, const HBool fftw)
{
  // Declaration of variables
  Iter it1=vec1;
  Iter it2=vec2;
  HInteger len1 = vec1_end - vec1;
  HInteger len2 = vec2_end - vec2;

  HInteger signfactor(-1);

  // Sanity check
  if (len2 > len1) ERROR_RETURN("Second vector is larger than output vector.");

  // Vector operation
  while (it1!=vec1_end) {
    if (fftw) signfactor=-signfactor;
    *it1 *= signfactor*conj(*it2);
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  If the input vector is shorter than the output vector it will wrap around
  and start from the beginning until the output vector is fully processed.

  Input and output vector can be identical.

  The following functions are available for getting real values from
  complex numbers:

    ===== ======================================================================
    norm  magnitude (length) squared of a complex number, i.e. ``c * conj(c)``
    abs   amplitude of a complex number, i.e. ``c * conj(c)``
    arg   phase angle of a complex number (in radians)
    imag  imaginary part of a complex number
    real  real part of a complex number
    ===== ======================================================================

    See also:

    hVectorLength

  Usage:
  vec2.$MFUNC(vec1) -> vec2 = [$MFUNC(vec1_0), $MFUNC(vec1_1), ... , $MFUNC(vec1_n)]
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
  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vector is shorter than the output vector it will wrap around
  and start from the beginning until the output vector is fully processed.

  Input and output vector can be identical.

  The following functions are available for getting real values from
  complex numbers:

    ===== ======================================================================
    norm  magnitude (length) squared of a complex number, i.e. ``c * conj(c)``
    abs   amplitude of a complex number, i.e. ``c * conj(c)``
    arg   phase angle of a complex number (in radians)
    imag  imaginary part of a complex number
    real  real part of a complex number
    ===== ======================================================================

  Usage:
  vec2.$MFUNC(vec1) -> vec2 = [$MFUNC(vec1_0), $MFUNC(vec1_1), ... , $MFUNC(vec1_n)]
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

//$DOCSTRING: Set all values which are above or below a certain limit to random value between those limits
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandomizePeaks
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(lower_limit)()("Lower limit.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(upper_limit)()("Upper limit.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven, hRandomizePhase, hSetAmplitude

  Example:
  >>> c=hArray(float,[10],fill=range(10))
  hArray(float, [10], fill=[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]) # len=10 slice=[0:10])
  >>> c.randomizepeaks(3,7)
  hArray(float, [10], fill=[3.87583674531, 3.18817846486, 5.71545886747, 3.0, 4.0, 5.0, 6.0, 7.0, 5.71718562335, 6.73877158376]) # len=10 slice=[0:10])
*/
template <class Iter,class T>
HInteger HFPP_FUNC_NAME (const Iter vecin , const Iter vecin_end,
			 const T lower_limit, const T upper_limit)
{
  // Declaration of variables
  Iter itin(vecin);
  // Sanity check
  if (vecin_end < vecin) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return -1;
  }

  HNumber scale(abs(hfcast<HNumber>(upper_limit)-hfcast<HNumber>(lower_limit)));
  scale /= RAND_MAX;
  srand(0xDEADBEEF); // (AC) Put in the same random number seed on each call, in order to ensure reproducible behavior.

  while (itin != vecin_end) {
    if (Outside(*itin,lower_limit,upper_limit)) {
      *itin=hfcast<IterValueType>(rand()*scale+hfcast<HNumber>(lower_limit));
    }
    ++itin;
  };
  return 0;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Randomize the phases of certain elements in a complec vector and set the amplitude.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandomizePhase
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Vector in which to randomize the phase.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(indexlist)()("Index list containing the positions of the elements to be set, (e.g. [0,2,4,...] will set every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(amplitude)()("Amplitude to assign to the indexed elements.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The elements are specified in an indexlist.

  **(NOT QUITE CORRECT: SPECIFY PROPER WEIGHTING TO RAND GENERATOR)**

  Usage:
  vec.randomizephase(indexlist,amplitude) -> elements in vec at positions provided in indexlist are set to a complex number with random phase and amplitude 'amplitude'

  See also:
  hSetAmplitude

  Example:
  >>> c=hArray(complex,[10],fill=range(10))
  hArray(complex, [10], fill=[0j, (1+0j), (2+0j), (3+0j), (4+0j), (5+0j), (6+0j), (7+0j), (8+0j), (9+0j)]) # len=10 slice=[0:10])
  >>> c.randomizephase(hArray([1,3,5,7]),1.)
  hArray(complex, [10], fill=[0j, (0.661930692225-0.749565046337j), (2+0j), (-0.930855778945-0.365386807102j), (4+0j), (-0.893076729911+0.449904383721j), (6+0j), (0.0594003866703+0.998234238074j), (8+0j), (9+0j)]) # len=10 slice=[0:10])
  >>> c.abs()
  hArray(complex, [10], fill=[0j, (1+0j), (2+0j), (1+0j), (4+0j), (1+0j), (6+0j), (1+0j), (8+0j), (9+0j)]) # len=10 slice=[0:10])
*/
template <class Iter, class IterI, class T>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const IterI index, const IterI index_end, const T amplitude)
{
  // Sanity check
  if (index >= index_end) return;

  // Variables
  Iter it;
  IterI itidx(index);
  HNumber amplitude_t=hfcast<HNumber>(amplitude);
  HNumber real_part, imag_part, scale(2.0/RAND_MAX);
  HInteger r;
  srand(0xDEADBEEF); // (AC) Put in the same random number seed on each call, in order to ensure reproducible behavior.

  while (itidx != index_end) {
    it = vec + *itidx;
    if ((it < vec_end) && (it >= vec)) {
      r=rand();
      real_part=(r*scale-NUMBER_ONE) * amplitude_t; // does this needs some arccos or so ...?
      if (r % 2) imag_part=sqrt(pow(amplitude_t,2)-pow(real_part,2));
      else imag_part=-sqrt(pow(amplitude_t,2)-pow(real_part,2));
      *it=HComplex(real_part,imag_part);
    };
    ++itidx;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Randomize the phases of certain elements in a complec vector and set the amplitude from a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandomizePhase
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Vector in which to randomize the phase.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(indexlist)()("Index list containing the positions of the elements to be set, (e.g. [0,2,4,...] will set every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(amplitude_vec)()("Amplitude to assign to the indexed elements.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The elements are specified in an indexlist.

  **(NOT QUITE CORRECT: SPECIFY PROPPER WEIGHTING TO RAND GENERATOR)**

  Usage:
  vec.randomizephase(indexlist,amplitude) -> elements in vec at positions provided in indexlist are set to a complex number with random phase and an amplitude picked from the amplitude vector at the same location.

  See also:
  hSetAmplitude

  Example:
  >>> baseline=hArray(float,[10],fill=range(90,100))
  >>> c=hArray(complex,[10],fill=range(10))
  hArray(complex, [10], fill=[0j, (1+0j), (2+0j), (3+0j), (4+0j), (5+0j), (6+0j), (7+0j), (8+0j), (9+0j)]) # len=10 slice=[0:10])
  >>> c.randomizephase(hArray([1,3]),baseline)
  hArray(complex, [10], fill=[(0,0),(-21.2026,-88.4955),(2,0),(3.61145,92.9299),(4,0),(5,0),(6,0),(7,0),(8,0),(9,0)]) # len=10 slice=[0:10])
  >>> c.abs()
  hArray(complex, [10], fill=[(0,0),(91,0),(2,0),(93,0),(4,0),(5,0),(6,0),(7,0),(8,0),(9,0)]) # len=10 slice=[0:10])
*/
template <class Iter, class IterI, class IterA>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const IterI index, const IterI index_end, const IterA amplitude_vec, const IterA amplitude_vec_end)
{
  // Sanity check
  if (index >= index_end) return;

  // Variables
  Iter it;
  IterI itidx(index);
  IterA  amplitude;
  HNumber real_part, amplitude_t, imag_part, scale(2.0/RAND_MAX);
  HInteger r;
  srand(0xDEADBEEF); // (AC) Put in the same random number seed on each call, in order to ensure reproducible behavior.

  while (itidx != index_end) {
    it = vec + *itidx;
    amplitude = amplitude_vec + *itidx;
    if ((it < vec_end) && (it >= vec) && (amplitude < amplitude_vec_end) && (amplitude >= amplitude_vec)) {
      amplitude_t=hfcast<HNumber>(*amplitude);
      r=rand();
      real_part=(r*scale-NUMBER_ONE) * amplitude_t; // does this needs some arccos or so ...?
      if (r % 2) imag_part=sqrt(pow(amplitude_t,2)-pow(real_part,2));
      else imag_part=-sqrt(pow(amplitude_t,2)-pow(real_part,2));
      *it=HComplex(real_part,imag_part);
    };
    ++itidx;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


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

  Description:
  The function calculates ``c.setamplitude(newamplitude)`` for each
  element ``c -> c/|c|*newamplitude``.

  If the input vector is shorter than output vector it will wrap around
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

  Description:
  The function calculates ``c.setamplitude(amp)`` for each
  element ``c -> c/|c|*amp``.
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


//$DOCSTRING: Multiplies all elements in the vector with each other and return the result.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hProduct
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
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


//$DOCSTRING: Piecewise multiplication of the elements in a vector and summing of the results.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMulSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hMulSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> a*x + b*y + c*z + ....
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


//$DOCSTRING: Piecewise subtraction of the elements beteen two vector and summing of the results.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDiffSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hDiffSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> a-x + b-y + c-z + ....
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

//$DOCSTRING: Calculate the first-difference of a vector (difference of subsequent elements) and store in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDifferentiate
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector of length N")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric output vector of length N-1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
\brief $DOCSTRING
$PARDOCSTRING

Usage:
hDifferentiate(Vector([a,b,c,...]), outputVector) -> [b-a,c-b,...]

Will stop if the end of either input or output vector is reached.

See Also:
 hDiffSum, hDiff
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vec1,const Iter vec1_end,const Iter vec2,const Iter vec2_end)
{
  Iter it1a=vec1,it1b=vec1+1,it2=vec2;
  if (it1b > vec1_end) return;
  while ((it1b!=vec1_end) && (it2!=vec2_end)) {
      (*it2) = (*it1b) - (*it1a);
      ++it1a; ++it1b; ++it2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Piecewise subtraction of the elements in a vector and summing of the square of the results.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDiffSquaredSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hDiffSquaredSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> (a-x)**2 + (b-y)**2 + (c-z)**2 + ....
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


//$DOCSTRING: Mean of the piecewise subtraction of the elements in a vector and summing of the square of the results.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanDiffSquaredSum
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector 1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector 2")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hMeanDiffSquaredSum(Vector([a,b,c,...]),Vector([x,y,z,...]) -> ((a-x)**2 + (b-y)**2 + (c-z)**2 + ....)/N
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


//$DOCSTRING: Calculate the :math:`\\chi^2` of two vectors.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hChiSquared
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(v1)()("Numeric input vector 1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(v2)()("Numeric input vector 2")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Calculate the :math:`\\chi^2` of two vectors, i.e., :math:`\\frac{(v1_i-v2_i)^2}{v2_i}`.

  Usage:
  hChiSquared(Vector([a,b,c,...]),Vector([x,y,z,...]) -> (a-x)**2/x + (b-y)**2/y + (c-z)**2/z + ....
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


//$DOCSTRING: Calculate the mean :math:`\\chi^2` of two vectors.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanChiSquared
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(v1)()("Numeric input vector (observed values)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(v2)()("Numeric input vector (expected values) - must not be zero")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Calculate the mean :math:`\\chi^2` of two vectors., i.e.

  .. math::

    \\frac{1}{n}\\sum_i \\frac{(v1_i - v2_i)^2}{v2_i}

  where :math:`n` is the length of the vector.

  Usage:
  hChiSquared(Vector([a,b,c,...]),Vector([x,y,z,...]) -> ((a-x)**2/x + (b-y)**2/y + (c-z)**2/z + ....)/N
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hDotProduct(Vector([a,b,c]), Vector([x,y,z])) -> a*x + b*y + c*z
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vecin1, const Iter vecin1_end,
			      const Iter vecin2, const Iter vecin2_end)
{
  // Sanity check
  if ((vecin1_end - vecin1) != (vecin2_end - vecin2)) {ERROR_RETURN_VALUE("Input vectors not of equal size.",0);};

  // Vector operation
  return hMulSum(vecin1, vecin1_end, vecin2, vecin2_end);
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
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hCrossProduct(Vector([a,b,c]), Vector([r,s,t]), Vector([x,y,z]))
  -> [s*z - t*y, t*x - r*z, r*y - s*z]
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
  if (lenIn1 != 3) ERROR_RETURN("First input vector is not of length 3.");
  if (lenIn2 != 3) ERROR_RETURN("Second input vector is not of length 3.");
  if (lenOut != 3) ERROR_RETURN("Output vector is not of length 3.");


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

//$DOCSTRING: Fills a vector with random values between minimum and maximum limits.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandom
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector which will be filled with random numbers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(minimum)()("Random numbers will not go below that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(maximum)()("Random numbers will not exceed that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> hRandom(vector,-1,1)
  [-0.5,0.3,0.1, ...]
  >>> vector.random(-1,1)
  [-0.5,0.3,0.1, ...]
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

//$DOCSTRING: Fills a vector with random values between minimum and maximum limits.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRandomComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector which will be filled with random numbers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(minimum)()("Random numbers will not go below that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(maximum)()("Random numbers will not exceed that value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> hRandom(vector,-1,1)
  [-0.5,0.3,0.1, ...]
  >>> vector.random(-1,1)
  [-0.5,0.3,0.1, ...]
*/
template <class CIter>
void HFPP_FUNC_NAME(const CIter vec,const CIter vec_end, const HNumber minimum, const HNumber maximum)
{
  CIter it=vec;
  HNumber scale(abs(maximum-minimum));
  scale /= RAND_MAX;
  while (it!=vec_end) {
    *it=std::complex<double>(rand()*scale+minimum, rand()*scale+minimum);
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

  See also:

  hMeanAbove, hMeanSquare, hMeanAbs, hMeanChiSquared

*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  // Declaration of variables
  HInteger len = vec_end - vec;

  // Sanity check
  if (len < 0) ERROR_RETURN_VALUE("Size of vector is < 0.",0.0);

  HNumber mean = hfcast<HNumber>(hSum(vec,vec_end));

  // Vector operation
  mean /= len;

  return mean;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a mean vector of an array of vectors
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMean
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(outvec)()("Numeric output vector of length Nel containing the mean values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(invec)()("Numeric input vector of size Nvec*Nel")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:

  Assume you have a 2D array of dimensions [Nvec,Nel], i.e. Nvec
  vectors of length Nel, then return the mean vector of all vectors in
  the input array. Input and output vectors can be of different types
  (with all consequences of rounding, of course!).

  See also:

  hMeanAbove, hMeanSquare, hMeanAbs, hMeanChiSquared

  Example:

  v1=hArray(float,[3],fill=0)
  v2=hArray(float,[3,3],fill=range(9))
  v1.mean(v2) # v1 -> hArray(float, [3L], fill=[3,4,5]) # len=3 slice=[0:3])
*/
template <class Iter, class Iter2>
void HFPP_FUNC_NAME (const Iter outvec,const Iter outvec_end, const Iter2 invec,const Iter2 invec_end)
{
  // Declaration of variables
  HInteger Nel = outvec_end - outvec;

  if (Nel <= 0) ERROR_RETURN("Size of output vector is <= 0.");

  HInteger Nvec = (invec_end - invec)/Nel;
  if (Nvec < 0) ERROR_RETURN("Size of input vector is < 0.");

  IterValueType null(0);
  hFill(outvec,outvec_end,null);
  hAddVec(outvec,outvec_end,invec,invec_end);
  hDiv2(outvec,outvec_end,Nvec);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of all elements in a vector which are above a certain threshold.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanAbove
//-----------------------------------------------------------------------
//#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(threshold)()("Threshold value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:

  hMeanAbove, hMeanSquare, hMeanAbs, hMeanChiSquared

*/
template <class Iter, class T>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const T threshold)
{
  // Declaration of variables
  HInteger len = vec_end - vec;

  // Sanity check
  if (len < 0) ERROR_RETURN_VALUE("Size of vector is < 0.",0.);

  len = 0;

  HNumber sum = 0.;

  Iter it=vec;
  while (it!=vec_end) {
    if (*it > threshold) {
      sum+=hfcast<HNumber>(*it);
      ++len;
    };
    ++it;
  };

  if (len>0) return sum/len;
  else return 0.0;

}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the mean value of the absolue values of all elements in a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMeanAbs
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:

  hMeanAbove, hMeanSquare, hMeanAbs, hMeanChiSquared

*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  // Declaration of variables
  HInteger len = vec_end - vec;

  // Sanity check
  if (len < 0) ERROR_RETURN_VALUE("Size of vector is < 0.",0.);

  HNumber mean=hfcast<HNumber>(hSumAbs(vec,vec_end));

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

  See also:

  hMeanAbove, hMeanSquare, hMeanAbs, hMeanChiSquared

*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  // Declaration of variables
  HInteger len = vec_end - vec;

  // Sanity check
  if (len < 0) ERROR_RETURN_VALUE("Size of vector is < 0.",0.);

  HNumber mean=hSumSquare(vec,vec_end);

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
//#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  .. warning::

    The vector will be sorted in place. Hence, if you want to
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

  Description:

  .. warning::

    The vector will be sorted first. Hence, if you want to keep the
    data in its original order, you need to copy the data first to a
    scratch vector and then call this function with the scratch
    vector!
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
//#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
//T HFPP_FUNC_NAME(std::vector<T> & vec)
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  std::vector<IterValueType> scratch(vec,vec_end);
  return hSortMedian(scratch);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the root mean square.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRMS
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  HNumber rms = 0.0;

  int n = std::distance(vec, vec_end);

  Iter it = vec;
  for (int i=0; i<n; i++)
  {
    // Less efficient but prevents overflow
    rms += (*it * *it) / n;
    it++;
  }

  return sqrt(rms);
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

//$DOCSTRING: Calculates the mean of all values which are $MFUNC a certain threshold value.
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


//$DOCSTRING: Calculates the mean of all values which are below or above the mean plus :math:`n \\sigma` standard deviations
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

  Description:
  This simply call ``hMeanGreaterThanThreshold`` or ``hMeanLessThanThreshold`` with a
  threshold value of ``mean+nsigma*rms``.

  See also:
  hMeanLessThanThreshold, hMeanGreaterThanThreshold
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


//$DOCSTRING: Calculates the mean of the inverse of all values and then return the inverse of that value.
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

  Description:
  This is useful to calculate the mean value of very spiky data. Large
  spikes will appear as zero and hence will not have a strong effect
  on the average (or the sum) if only a small number of channels are
  affected, while a single very large spike could well dominate the
  normal average. Only works well if all values are positive (or
  negative). If the mean is around zero it becomes unstable, of
  course.

  Usage:
  vec.meaninverse() -> N/sum(1/vec_0+1/vec_1+...+1/vec_N)
 */
template <class Iter>
IterValueType HFPP_FUNC_NAME (const Iter vec, const Iter vec_end)
{
  typedef IterValueType T;
  T sum(hfnull<T>());
  Iter it(vec);

  while (it!=vec_end) {
    try {
      sum+=NUMBER_ONE/(*it);
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


//$DOCSTRING: Find the minimum standard deviation of a vector within blocks of a certain length.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinStdDev
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(blocklen)()("Length of the blocks for which to calculate the RMS")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.minstddev(blocklen) -> minium rms (float)
  hMinStdDevS(vec,blocklen) -> minium rms (float)

  Description:
  Subdivide the input vector into smaller blocks of len ``blocklen`` and
  calculate for each block the standard deviation. The standard
  deviation of the block with the smallest value will be returned.
  All blocks have the same length with a possible exception of the
  last block, which is ignored if it is shorter.

  Used to find the RMS in the cleanest part of a spiky data set.
 */

template <class Iter>
HNumber HFPP_FUNC_NAME (const Iter vecin, const Iter vecin_end, const HInteger blocklen)
{
  // Declaration of variables
  HInteger lenIn = vecin_end-vecin;
  HInteger blen(blocklen);
  HNumber rms;

  // Sanity check
  if (lenIn <= 0) {
    throw PyCR::ValueError("Size of input vector <= 0.");
    return 0.0;
  }
  if (blen>lenIn) blen=lenIn;

  Iter it1(vecin), it2(vecin+blocklen);
  HNumber minrms(hStdDev(it1,it2));

  it1=it2;
  it2+=blen;

  while (it2<vecin_end) {
    rms=hStdDev(it1,it2);
    if (rms < minrms) minrms=rms;
    it1=it2;
    it2+=blen;
  }
  return minrms;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Find the block of the smallest standard deviation in a vector and return mean and RMS.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMinStdDevBlock
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(blocklen)()("Length of the blocks for which to calculate the RMS")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(minrms)()("Standard deviation in block with minimum standard devitation (RMS)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(minmean)()("Average value of values in block with minimum standard devitation (RMS)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.minstddevblock(blocklen,minrms,minmean) -> block number of minimum rms (int)
  hMinStdDevBlock(vec,blocklen,minrms,minmean) -> block number of minimum rms (int)

  Description:
  Subdivide the input vector into smaller blocks of len ``blocklen`` and
  calculate for each block the standard deviation. The block
  number of the block with the smallest standard deviation will be
  returned. The values of mean and standard deviation will be
  returned in the variables ``minrms`` and ``minmean``. In Python
  both variables should be vectors, since scalars are not returned
  (in this case also blocklen needs to be a vector).

  All blocks have the same length with a possible exception of the last
  block, which is ignored if it is shorter.

  Used to find the the cleanest part of a spiky data set.


 */

template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vecin, const Iter vecin_end, const HInteger blocklen, HNumber & minrms, HNumber & minmean)
{
  // Declaration of variables
  HInteger lenIn = vecin_end-vecin;
  HInteger nblock(0), blen(blocklen);
  HNumber rms,mean;

  // Sanity check
  if (lenIn <= 0) {
    ERROR_RETURN_VALUE("Size of input vector <= 0.",0.0);
  }
  if (blen>lenIn) blen=lenIn;

  Iter it1(vecin), it2(vecin+blocklen);
  HInteger minblock(0);
  minmean=hMean(it1,it2);
  minrms=hStdDev(it1,it2,minmean);

  it1=it2;
  it2+=blen;

  while (it2<=vecin_end) {
    ++nblock;
    mean=hMean(it1,it2);
    rms=hStdDev(it1,it2,mean);
    if (rms < minrms) {
      minrms=rms;
      minmean=mean;
      minblock=nblock;
    };
    it1=it2;
    it2+=blen;
  }
  return minblock;
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
HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  return hStdDev(vec,vec_end,hMean(vec,vec_end));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Count the samples that are equal to a certain value and return the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector to search through.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(val)()("The value to find an count.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end, const IterValueType val)
{
  Iter it(vec);
  HInteger count(0);
  while (it<vec_end) {
    if (*it == val) ++count;
    ++it;
  };
  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the first sample that equals the input values and return its position.
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

  Description:
  Return -1 if ``value`` is not found.

  See also:
  findgreaterthan, findgreaterequal, findlessthan, findlessequal, findbetween, findoutside, findoutsideorequal, findbetweenorequal
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


//$DOCSTRING: Find the samples whose values equal a certain value and return the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in input vector matching the values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(value)()("The value to search")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end,
			 const IterValueType value
			 )
{
  // Declaration of variables
  Iter itin(vecin);
  typename vector<HInteger>::iterator itout(vecout);

  if (vecin_end < vecin) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return -1;
  }

  // Function operation
  while (itin != vecin_end) {
    if (*itin == value) {
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


// ========================================================================
//
//$Section:  Find & Search
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


//$DOCSTRING: Find the first sample that is $MFUNC the two input values and return its position.
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

  Description:
  Return -1 if not found.

  See also:
  hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven
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

  See also:
  hFind, hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven
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


//$DOCSTRING: Find and return a list containing slice indices (i.e., beginning and end+1 position) of sequences of (almost) consecutive values in a vector which are {$MFUNC} the two threshold values.
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

  See also:
  hFind, hFindSequenceGreaterThan, hFindSequenceGreaterEqual, hFindSequenceLessThan, hFindSequenceLessEqual,
  hFindSequenceBetween, hFindSequenceOutside, hFindSequenceOutsideOrEqual, hFindSequenceBetweenOrEqual,
  hMaxInSequences, hMinInSequences, hSumInSequences,hMeanInSequences, hStdDevInSequences

  Example:
  >>> # Make a test time series data set for 4 antennas and some peaks at various locations
  >>> data=hArray(float,[4,512],name="Random series with peaks")
  >>> data.random(-1024,1024); data[...] += Vector([-128.,256., 385.,-50.])
  >>> for i in range(4):
  ...     data[i,[2,3,32,64,65,67],...] = Vector([4096.,5097,-4096,4096,5099,3096])
  >>> nsigma=5
  >>> datapeaks=hArray(int,[4,256,2],name="Location of peaks")
  >>> datamean=data[...].mean()
  >>> datathreshold2 = data[...].stddev(datamean)
  >>> datathreshold2 *= nsigma
  >>> datathreshold1 = datathreshold2*(-1)
  >>> datathreshold1 += datamean
  >>> datathreshold2 += datamean

  >>> maxgap=Vector(int,len(datamean),fill=10)
  >>> minlength=Vector(int,len(datamean),fill=1)
  >>> npeaks=datapeaks[...].findsequenceoutside(data[...],datathreshold1,datathreshold2,maxgap,minlength)

  >>> npeaks
  Vec(int,4)=[3,3,3,3]

  >>> datapeaks
  hArray(int,[4, 256, 2], name="Location of peaks") # len=2048, slice=[0:512], vec -> [2,4,32,33,64,68,0,0,...]
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
//$ITERATE MFUNC Max,Min,Sum,Mean,StdDev
//========================================================================

//$DOCSTRING: Find the $MFUNC in a list of sequences provided by an indexlist of start/end values.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h{$MFUNC}InSequences
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Numeric output vector containing the maxima of each sequence")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Numeric input vector containing the values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(indexlist)()("Integer list containing 2-tuples with begin and end index for each sequence")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(nsequences)()("The maximum number of sequences to take (all if <0 or >vector length).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  maxima.maxinsequences(datavector,indexlist,npeaks) -> return maxima in each sequence indicated by indexlist

  indexlist=Vector(int,[npeaks,2],fill=[seq0_start,seq0_end,seq1_start,seq1_end, ...])

  See also:
  hMaxInSequences, hMinInSequences, hSumInSequences,hMeanInSequences, hStdDevInSequences, hFind,
  hFindSequenceGreaterThan, hFindSequenceGreaterEqual, hFindSequenceLessThan, hFindSequenceLessEqual,
  hFindSequenceBetween, hFindSequenceOutside, hFindSequenceOutsideOrEqual, hFindSequenceBetweenOrEqual


  Example:
  >>> npeaks=3
  >>> datavector = hArray(float,[10],fill=range(10))
  >>> indexlist = hArray(int,[npeaks,2],fill=[0,2,3,5,5,9])
  >>> maxima = hArray(float,[npeaks])
  >>> maxima.maxinsequences(datavector,indexlist,npeaks)
  >>> maxima
  hArray(float, [3], fill=[1,4,8]) # len=3 slice=[0:3])
*/

template <class Iter,class Iter2,class IterI>
void HFPP_FUNC_NAME (const Iter vecout, const Iter vecout_end, const Iter2 vecin, const Iter2 vecin_end,const IterI indexlist, const IterI indexlist_end, const HInteger nsequences)
{
  // Sanity check
  if (vecin_end-vecin < 0) throw PyCR::ValueError((HString)(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME)) + ": Size of input vector < 0.");
  if (vecout_end-vecout < 0) throw PyCR::ValueError((HString)(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME)) + ": Size of input vector < 0.");
  if (indexlist_end-indexlist < 0) throw PyCR::ValueError((HString)(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME)) + ": Size of input vector < 0.");

  Iter itout(vecout);
  IterI iti1(indexlist),iti2(indexlist+1);
  Iter2 itin1,itin2;
  HInteger ncount(0);

  if (iti2>indexlist_end) return;

  while (iti2<=indexlist_end && itout != vecout_end && ncount<nsequences) {
    itin1=vecin+*iti1; itin2=vecin+*iti2;
    if ((itin1>=vecin_end) || (itin1<vecin)) throw PyCR::ValueError((HString)(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME))+ ": Sequence start pointing outside vector.");
    if ((itin2>vecin_end) || (itin2<=vecin)) throw PyCR::ValueError((HString)(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME)) + ": Sequence end pointing outside vector.");
    if (itin2<=itin1) throw PyCR::ValueError((HString)(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME)) + ": Invalid sequence specified (length<0).");
    *itout = hfcast<IterValueType>(h{$MFUNC}(itin1,itin2));
    ++ncount; iti1+=2; iti2+=2; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//========================================================================
//$ITERATE MFUNC GreaterThan,GreaterEqual,LessThan,LessEqual
//========================================================================

//$DOCSTRING: Find the first sample that is $MFUNC the threshold value and return its position.
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

  Description:
  Return -1 if not found.

  See also:
  hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven
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

  See also:
  hFind, hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven
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


//$DOCSTRING: Find the samples that are $MFUNC the corresponding threshold values and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC}Vec
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in the input vector which satisfy the threshold condition.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(threshold)()("Vector with threshold values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hFind, hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual
  hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven
  hFindGreaterThanVec, hFindGreaterEqualVec, hFindLessThanVec, hFindLessEqualVec
*/
template <class Iter, class Iter2>
HInteger HFPP_FUNC_NAME (
                        const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
                        const Iter vecin , const Iter vecin_end,
                        const Iter2 threshold , const Iter2 threshold_end
			 )
{
  // Declaration of variables
  Iter itin(vecin);
  Iter2 itthreshold(threshold);
  typename vector<HInteger>::iterator itout(vecout);

  if (itthreshold >= threshold_end) return 0;

  // Function operation
  while (itin != vecin_end) {
    if (*itin HFPP_OPERATOR_$MFUNC (*itthreshold)) {
      if (itout != vecout_end) {
        *itout=(itin-vecin);
        ++itout;
      };
    };
    ++itin;
    ++itthreshold;
    if (itthreshold==threshold_end) itthreshold=threshold;
  };
  return (itout-vecout);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples whose *absolute* values are $MFUNC a certain threshold value and returns the number of samples found and the positions of the samples in a second vector.
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
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of 2 tuples of position and length for each sequence found.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vecin)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(threshold)()("Threshold value - if values are $MFUNC this, they can belong to a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(maxgap)()("The maximum gap (in between sample numbers) between two samples to still belong to one sequence. 0=no gaps allowed, i.e. consecutive.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(minlength)()("The minimum length of a sequence.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)

//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
  Usage:
  npeaks=indexlist.findsequencegreaterthan(vecin,threshold,maxgap,minlength)

  See also:
  hFind, hFindSequenceGreaterThan, hFindSequenceGreaterEqual, hFindSequenceLessThan, hFindSequenceLessEqual
  hFindSequenceBetween, hFindSequenceOutside, hFindSequenceOutsideOrEqual, hFindSequenceBetweenOrEqual

  Example:
  >>> # Make a test time series data set for 4 antennas and some peaks at various locations
  >>> data=hArray(float,[4,512],name="Random series with peaks")
  >>> data.random(0,1024); data[...] += Vector([128.,256., 385.,50.])
  >>> for i in range(4):
  ...     data[i,[2,3,32,64,65,67],...] = Vector([4096.,5097,4096,4096,5099,3096])

  >>> datapeaks = hArray(int,[4,256,2], name="Location of peaks")
  >>> datamean = data[...].mean()
  >>> datathreshold = data[...].stddev(datamean)
  >>> datathreshold *= 5
  >>> datathreshold += datamean

  >>> maxgap = Vector(int,len(datamean),fill=10)
  >>> minlength = Vector(int,len(datamean),fill=1)
  >>> npeaks=datapeaks[...].findsequencegreaterthan(data[...],datathreshold,maxgap,minlength)
  >>> npeaks
  Vec(int,4)=[3,3,3,3]
  >>> datapeaks
  hArray(int,[4, 256, 2], name="Location of peaks") # len=2048, slice=[0:512], vec -> [2,4,32,33,64,68,0,0,...]
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


//========================================================================
//$ITERATE MFUNC odd,even
//========================================================================

//$DOCSTRING: Find the first sample that is $MFUNC and return its position.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_AND_STRING_TYPES
#define HFPP_FUNC_VARIANT 1
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input vector to search through.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Return -1 if not found.

  See also:
  hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven, hFindOdd, hFindEven
*/
template <class Iter>
HInteger hFind{$MFUNC!CAPS}1 (const Iter vecin,const Iter vecin_end)
{
  // Declaration of variables
  Iter itin(vecin);
  // Sanity check
  if (vecin_end < vecin) {
    throw PyCR::ValueError("Illegal input our output vector size.");
    return -1;
  }

  while (itin != vecin_end) {
    if (hf{$MFUNC}(*itin)) {return itin-vecin;}
    ++itin;
  };
  return -1;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Find the samples that are $MFUNC and returns the number of samples found and the positions of the samples in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFind{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_AND_STRING_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vecout)()("Output vector - contains a list of positions in the input vector which satisfy the threshold condition.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hFind, hFindGreaterThan, hFindGreaterEqual, hFindLessThan, hFindLessEqual, hFindBetween, hFindOutside, hFindOutsideOrEqual, hFindBetweenOrEqual, hFindOdd, hFindEven
*/
template <class Iter>
HInteger hFind{$MFUNC!CAPS} (const typename vector<HInteger>::iterator vecout, const typename vector<HInteger>::iterator vecout_end,
			 const Iter vecin , const Iter vecin_end)
{
  // Declaration of variables
  Iter itin(vecin);
  typename vector<HInteger>::iterator itout(vecout);

  // Function operation
  while (itin != vecin_end) {
    if (hf{$MFUNC}(*itin)) {
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

//$DOCSTRING: Counts the samples that are $MFUNC and returns the number of samples found.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCount{$MFUNC!CAPS}
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_NUMERIC_AND_STRING_TYPES
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector to search through")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec , const Iter vec_end)
{
  Iter it(vec);
  HInteger count(0);
  while (it<vec_end) {
    if (hf{$MFUNC}(*it)) ++count;
    ++it;
  };
  return count;
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

  Description:
  Downsample the input vector to a smaller output vector, by replacing
  subsequent blocks of values by their mean value. The block size is
  automatically chosen such that the input vector fits exactly into
  the output vector. All blocks have the same length with a possible
  exception of the last block.
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

  blen = hfmax((((HNumber)lenIn)/((HNumber)lenOut)),NUMBER_ONE);
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

  Description:
  Downsample the input vector to a new smaller output vector, by
  replacing subsequent blocks of values by their mean value. The block
  size is automatically chosen such that the input vector fits exactly
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



//$DOCSTRING: Interpolate a vector between two end points, which are part also start and end points of the new vector (adds to output vector!)
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

  Description:
  Note, the interpolated data is added to the output vector, so the
  array needs to be initialized with zero.

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
      *it+=(IterValueType)(startvalue + count*slope);
      ++it; ++count;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Interpolate a vector between two end points, where the 2nd end point marks the last element of the output vector plus one.
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

  Description:
  Note, the interpolated data is added to the output vector, so the
  array needs to be initialized with zero.
  Useful for piecing interpolations together (adds to output vector!)

  Example:
  >>> v = hArray(float, [10])
  >>> v.interpolate2p(0.,9.) #-> [0,1,2,3,4,5,6,7,8,9]
  >>> v.interpolate2psubpiece(0.,10.) #-> [0,1,2,3,4,5,6,7,8,9]
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
      *it+=(IterValueType)(startvalue + count*slope);
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

  Description:
  Downsample the input vector to a smaller output vector, by replacing
  subsequent blocks of values by their mean value. The block size is
  automatically chosen such that the input vector fits exactly into
  the output vector. All blocks have the same length with a possible
  exception of the last block.

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
  blen = max((((HNumber)lenIn)/((HNumber)lenOut)),NUMBER_ONE);
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


//$DOCSTRING: Upsample the input vector to a larger output vector by linear interpolation and add to output vector.
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

  Description:
  Note, the interpolated data is added to the output vector, so the
  array needs to be initialized with zero.

  Example:
  >>> x = hArray(float, [31])
  >>> v = hArray(float, [10])
  >>> v.interpolate2p(0.,9.) #-> [0,1,2,3,4,5,6,7,8,9]
  >>> x.upsample(v)
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

  blen = max(lenOut/(lenIn-NUMBER_ONE),NUMBER_ONE);
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

  Description:
  Downsample the input vector to a smaller output vector, by replacing
  subsequent blocks of values by their mean value. The block size
  is automatically chosen such that the input vector fits exactly
  into the output vector. All blocks have the same length with a
  possible exception of the last block.

  As a second output the root mean square (RMS, standard deviation)
  of the mean in each downsampled bin is returned.

  The mean here is taken only of data that is :math:`n \\sigma` below
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
  blen = max(((HNumber)lenIn)/((HNumber)lenOut),NUMBER_ONE);
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

  Description:
  Finds -- through a binary search and interpolation -- the location in
  a monotonically increasing vector, where the search value is just
  above or equal to the value in the vector.

  This requires random access iterators, in order to have an optimal search result.
*/
template <class Iter>
HInteger HFPP_FUNC_NAME (const Iter vec, const Iter vec_end,
			 const IterValueType value)
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



//$DOCSTRING: Returns vector of weights of length len with constant weights normalized to give a sum of unity. Can be used by ``hRunningAverageT``.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFlatWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(len)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger len) {
  // Sanity Check
  if (len <= 0) {
    throw PyCR::ValueError("Length of weight vector should be > 0.");
  }

  std::vector<HNumber> weights(len, NUMBER_ONE/len);
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING:  Returns vector of weights of length ``len`` with linearly rising and decreasing weights centered at ``len/2``.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(len)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The vector is normalized to give a sum of unity. Can be used by
  ``hRunningAverage``.

  See also:
  hRunningAverage
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger len) {
  // Sanity check
  if (len <= 0) {
    throw PyCR::ValueError("Length of weight vector should be > 0.");
  }

  std::vector<HNumber> weights(len, 0.0);
  HInteger i, middle=len/2;
  HNumber f, sum=0.0;

  for (i=0; i<len; i++) {
    f=NUMBER_ONE-abs(middle-i)/(middle+NUMBER_ONE);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  if (abs(sum) < A_LOW_NUMBER) {
    throw PyCR::ZeroDivisionError("Sum value is 0.");
  } else {
    for (i=0; i<len; i++) weights[i] /= sum;
  }
  return weights;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Returns vector of wieghts with Gaussian distribution.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGaussianWeights
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(len)()("Lengths of weights vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:

  Returns vector of weights of length ``len`` with a Gaussian
  distribution centered at :math:`\\mu=` ``len/2`` and
  :math:`\\sigma=` ``len/4`` (i.e. the Gaussian extends over :math:`2
  \\sigma` in both directions).
*/
 //template <class T>

std::vector<HNumber> HFPP_FUNC_NAME (HInteger len) {
  // Sanity check
  if (len <= 0) {
    throw PyCR::ValueError("Length of weight vector should be > 0.");
  }

  vector<HNumber> weights(len,0.0);
  HInteger i, middle=len/2;
  HNumber f, sum=0.0;

  for (i=0; i<len; i++) {
    f=funcGaussian(i,max(len/((HNumber)4.0),NUMBER_ONE),middle);
    weights[i]=f;
    sum+=f;
  };
  //Normalize to unity
  if (abs(sum) < A_LOW_NUMBER) {
    throw PyCR::ZeroDivisionError("Sum value is 0.");
  } else {
    for (i=0; i<len; i++) weights[i] /= sum;
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
#define HFPP_PARDEF_0 (HInteger)(len)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (hWEIGHTS)(type)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
vector<HNumber> HFPP_FUNC_NAME (const HInteger len, const hWEIGHTS type){
  vector<HNumber> weights;
  HInteger len2(len);
  if (len2<1) len2=1;
  switch (type) {
  case WEIGHTS_LINEAR:
    weights=hLinearWeights(len2);
    break;
  case WEIGHTS_GAUSSIAN:
    weights=hGaussianWeights(len2);
    break;
  default: //  WEIGHTS_FLAT:
    weights=hFlatWeights(len2);
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
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Input and Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(weights)()("Weight vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Will return a vector where each element is replaced by the sum of its
  neighbours multiplied with the weights vector. The middle of the
  weightsvector (usually its peak) is at ``len(weights)/2``. It is assumed
  that the wights are normalized (i.e. ``sum(weights)=1``).
*/

template <class Iter, class Iter2>
void HFPP_FUNC_NAME (const Iter  vec,
                     const Iter  vec_end,
                     const Iter2 weights,
                     const Iter2 weights_end)
{
  HInteger len=(weights_end-weights);
  /* Index of the central element of the weights vector (i.e., where it
     typically would peak) */
  HInteger middle=len/2;
  HNumber temp;

  std::vector<IterValueType> buffer_vec(len); // will temporarily store input data
  Iter buffer(buffer_vec.begin()),buffer_start(buffer_vec.begin()),buffer_end(buffer_vec.end());

  Iter  it(vec), last(vec_end-1), it_plus_middle(vec+middle);
  Iter2 wit;

  if (vec>=vec_end) return;
  if (weights>=weights_end) return;

  //  hCopy(&(*(buffer_start+middle)),&(*buffer_end),&(*vec),&(*(vec+len-middle))); //initialize scratch vector
  //hFill(&(*buffer_start),&(*(buffer_start+middle)),*it); //initialize scratch vector
  hCopy(buffer_start+middle,buffer_end,vec,vec+len-middle); //initialize scratch vector
  hFill(buffer_start,buffer_start+middle,*it); //initialize scratch vector
  while (it!=vec_end) {
    wit=weights; // weight iterators set to beginning of weights
    temp = 0.0;
    while (wit!=weights_end) {
      temp += hfcast<HNumber>((*wit) * (*buffer));
      ++wit; ++buffer;
      if (buffer==buffer_end) buffer=buffer_start;
    };
    *it=hfcast<IterValueType>(temp);
    ++it;
    if (it_plus_middle != last) ++it_plus_middle;
    *buffer = *it_plus_middle;
    ++buffer; //advance ring buffer
    if (buffer==buffer_end) buffer=buffer_start; //wrap if necessary
  };
  return;
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
#define HFPP_PARDEF_2 (HInteger)(len)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (hWEIGHTS)(type)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Available Weights:

    ============= ====================================================================
    ``FLAT``      All have the same value.
    ``LINEAR``    Linearly rising, peaking at the center (i.e., ``/\``).
    ``GAUSSIAN``  Gaussian distribution falling off to :math:`2 \\sigma` at the ends.
    ============= ====================================================================

  Example:
  >>> in_array.runningaverage(array_out,7,hWEIGHTS.GAUSSIAN)
  >>> x = hArray([0.,1.,0.,3.,1.,3.,0.,2.,1.])
  >>> x.runningaverage(3,hWEIGHTS.FLAT)

  >>> x
  [0.333333,0.333333,1.33333,1.33333,2.33333,1.33333,1.66667,1,1.33333]
*/
template <class DataIter>
void HFPP_FUNC_NAME (const DataIter odata,
		     const DataIter odata_end,
		     const DataIter idata,
		     const DataIter idata_end,
		     const HInteger len,
		     const hWEIGHTS type)
{
  vector<HNumber> weights = hWeights(len, type);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (odata,
							odata_end,
							idata,
							idata_end,
							weights.begin(),
							weights.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the running average of an input vector using different weighting schemes.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRunningAverage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(odata)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(len)()("Length of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (hWEIGHTS)(type)()("Type of weight vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Available Weights:

    ============= ====================================================================
    ``FLAT``      All have weight the same value
    ``LINEAR``    Linearly rising, peaking at the center (i.e., ``/\``)
    ``GAUSSIAN``  Gaussian distribution falling off to :math:`2 \\sigma` at the ends
    ============= ====================================================================

  Example:
  >>> in_array.runningaverage(7,hWEIGHTS.GAUSSIAN)
*/
template <class DataIter>
void HFPP_FUNC_NAME (const DataIter odata,
		     const DataIter odata_end,
		     const HInteger len,
		     const hWEIGHTS type)
{
  vector<HNumber> weights = hWeights(len, type);
  hRunningAverage<DataIter, vector<HNumber>::iterator> (odata,
							odata_end,
							weights.begin(),
							weights.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the square of the input vector and add it to the values in the output vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSquareAdd
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Input vector containing complex values. (Looping parameter)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Calculate :math:`a^2` for each element :math:`a` in the input vector
  and add the result to the output vector. Note that (unlike for
  hSpectralPower) for complex numbers the result can also be negative!

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an ``hArray``) is here determined by
  the second parameter!

  If the vectors have different lengths, the shortest one is taken to
  determine for how many elements this is calculated.

  See also:

  hSpectralPower, hSpectralPower2

  Example:
  >>> spectrum = hArray(float,[1,128])
  >>> cplxfft = hArray(complex,[10,128],fill=1+0j)
  >>> spectrum[...].squareadd(cplxfft[...])
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  // Declaration of variables
  Iterin itin(vecin);
  Iter itout(vecout);

  // Calculation of spectral power
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout+=hfcast<IterValueType>((*itin)*(*itin));
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#ifdef PYCRTOOLS_WITH_NUMPY

//$DOCSTRING: Calculates the square of the absolute value and add it to output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSquareAdd
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (ndarray)(out)()("Numpy vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Calculate :math:`|a|^2` for each element :math:`a` in the input vector
  and add the result to the output vector.
*/
template <class NIter>
void HFPP_FUNC_NAME(ndarray out, const NIter in_begin, const NIter in_end)
{
  NIter in_it = in_begin;

  // Get pointers to memory of numpy array
  double* out_it = numpyBeginPtr<double>(out);
  double* out_end = numpyEndPtr<double>(out);

  // Copy and cast to correct type
  while (out_it != out_end && in_it != in_end)
  {
    *out_it += static_cast<double>((*in_it) * (*in_it));

    ++out_it;
    ++in_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#endif /* PYCRTOOLS_WITH_NUMPY */

#ifdef PYCRTOOLS_WITH_NUMPY

//$DOCSTRING: Calculates the square of the absolute value and add it to output vector while shifting last dimension to first.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSquareAddTransposed
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (ndarray)(out)()("Numpy vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HNumber)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(dimsize)()("Size of last dimension in input vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Calculate :math:`|a|^2` for each element :math:`a` in the input vector
  and add the result to the output vector.
  In addition this routine shifts the last dimension (e.g. fastest running index) of size dimsize to
  the first dimension (e.g. slowest running index).
*/
template <class NIter>
void HFPP_FUNC_NAME(ndarray out, const NIter in_begin, const NIter in_end, HInteger dimsize)
{
  NIter in_it = in_begin;

  // Get pointers to memory of numpy array
  double* out_it = numpyBeginPtr<double>(out);
  //double* out_end = numpyEndPtr<double>(out);

  // Get vector lengths
  const int Nin = std::distance(in_it, in_end);
  //const int Nout = std::distance(out_it, out_end);
  const int Ninner = Nin / dimsize;

  // Copy and cast to correct type
  for (int i=0; i<dimsize; ++i)
  {
    in_it = in_begin + i;
    for (int j=0; j<Ninner; ++j)
    {
      *out_it += static_cast<double>((*in_it) * (*in_it));

      in_it += dimsize;

      ++out_it;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#endif /* PYCRTOOLS_WITH_NUMPY */

//$DOCSTRING: Calculates the square of the absolute value of a complex number and add to output vector
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

  Description:
  Calculate :math:`|a|^2` for each element :math:`a` in the input vector
  and add the result to the output vector.

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an ``hArray``) is here determined by
  the second parameter!

  Example:
  >>> spectrum = hArray(float,[1,128])
  >>> cplxfft = hArray(complex,[10,128],fill=1+0j)
  >>> spectrum[...].spectralpower(cplxfft[...])
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

#ifdef PYCRTOOLS_WITH_NUMPY

//$DOCSTRING: Calculates the square of the absolute value and add it to output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAbsSquareAdd
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
  Calculate :math:`|a|^2` for each element :math:`a` in the input vector
  and add the result to the output vector.
*/
template <class Iter>
void HFPP_FUNC_NAME(ndarray out, const Iter in_begin, const Iter in_end)
{
  Iter in_it = in_begin;

  // Get pointers to memory of numpy array
  double* out_it = numpyBeginPtr<double>(out);
  double* out_end = numpyEndPtr<double>(out);

  // Copy and cast to correct type
  while (out_it != out_end && in_it != in_end)
  {
    *out_it += static_cast<double>(real(*in_it * conj(*in_it)));

    ++out_it;
    ++in_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

#endif /* PYCRTOOLS_WITH_NUMPY */

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

//$DOCSTRING: Calculate steps to skip masked entries
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaskToStep
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(step)()("Array with steps to skip masked entries.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(mask)()("Array with mask defining which frequency channels are to be skipped. Expected array of lenght Nf (where Nf is the number of frequency channels) containing zeros (for channels to be included) and ones (for channels to be masked and not used for imaging).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class IIter>
void HFPP_FUNC_NAME (const IIter step, const IIter step_end,
    const IIter mask, const IIter mask_end
    )
{
  // Step counter
  int count = 0;

  // Get iterators
  IIter it_step = step;
  IIter it_mask = mask;

  // Calculate steps
  while (it_step != step_end && it_mask != mask_end)
  {
    if (*it_mask == 0)
    {
      *it_step = count;

      count = 1;

      ++it_step;
    }
    else
    {
      ++count;
    }
    ++it_mask;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Count the number of zero elements
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCountZero
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class IIter>
HInteger HFPP_FUNC_NAME (const IIter vec, const IIter vec_end)
{
  // Get iterator
  IIter it_vec = vec;

  // Calculate vecs
  int count = 0;
  while (it_vec != vec_end)
  {
    if (*it_vec == 0) ++count;
    ++it_vec;
  }

  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Count the number of non-zero elements
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCountNonZero
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class IIter>
HInteger HFPP_FUNC_NAME (const IIter vec, const IIter vec_end)
{
  // Get iterator
  IIter it_vec = vec;

  // Calculate vecs
  int count = 0;
  while (it_vec != vec_end)
  {
    if (*it_vec != 0) ++count;
    ++it_vec;
  }

  return count;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Add absolute value squared at shifted position
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hShiftedAbsSquareAdd
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(target)()("")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(source)()("")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(shifts)()("")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class NIter, class IIter>
void HFPP_FUNC_NAME (const NIter target, const NIter target_end,
    const CIter source, const CIter source_end,
    const IIter shifts, const IIter shifts_end
    )
{
  // Variables
  int i,j;

  // Get dimensions of input
  const int Ntarget = std::distance(target, target_end);
  const int Nsource = std::distance(source, source_end);
  const int Nshifts = std::distance(shifts, shifts_end);

  // Get fractions relating input and output
  const int Nss = Nsource / Nshifts;
  const int Nts = Ntarget / Nss;

  // Sanity checks
  if (Nshifts > Nsource)
  {
    throw PyCR::ValueError("Shift dimensions cannot exceed source dimensions.");
  }
  if (Nsource > Ntarget)
  {
    throw PyCR::ValueError("Source dimensions cannot exceed target dimensions.");
  }
  if (Nsource != Nss * Nshifts)
  {
    throw PyCR::ValueError("Shifts must fit an integer number of times in source.");
  }
  if (Ntarget < Nss)
  {
    throw PyCR::ValueError("Target dimensions too small.");
  }

  // Get iterators
  NIter target_it = target;
  CIter source_it = source;
  IIter shifts_it = shifts;

  // Loop over shifts, which is also assumed to be the fastest index in
  // the source array, so offset calculation is only needed for outer loop

#ifdef _OPENMP
#pragma omp parallel for private(target_it, source_it, shifts_it, j)
#endif // _OPENMP
 for (i=0; i<Nshifts; ++i)
  {
    // Shift for current iteration
    shifts_it = shifts + i;

    // Only inner loop if shift gives a valid offset into the target array
    if (*shifts_it >= 0 && *shifts_it < Nts)
    {
      // Get offset into source and target arrays
      source_it = source + i;
      target_it = target + *shifts_it * Nss;

      // Loop over second fastest index in source array stepping over
      // the fastest index (with steps of Nshifts)
      for (j=0; j<Nss; ++j)
      {
        // Add absolute value squared to the correct position in the target
        // array
        *target_it += real(*source_it * conj(*source_it));

        // Next position in source array (skipping fastest index)
        source_it += Nshifts;

        // Next position in target array
        ++target_it;
      }
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns true if all values are finite
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hIsFinite
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class NIter>
bool HFPP_FUNC_NAME (const NIter vec, const NIter vec_end)
{
  bool status = true;

  // Get iterator
  NIter it_vec = vec;

  while (it_vec != vec_end)
  {
    if (!boost::math::isfinite(*it_vec)) status = false;
    ++it_vec;
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns true if both the real and imaginary part of all values are finite
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hIsFiniteComplex
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter>
bool HFPP_FUNC_NAME (const CIter vec, const CIter vec_end)
{
  bool status = true;

  // Get iterator
  CIter it_vec = vec;

  while (it_vec != vec_end)
  {
    if (!boost::math::isfinite(real(*it_vec)) || !boost::math::isfinite(imag(*it_vec))) status = false;
    ++it_vec;
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns true if all values are finite and not denormalised
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hIsNormal
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (bool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class NIter>
bool HFPP_FUNC_NAME (const NIter vec, const NIter vec_end)
{
  bool status = true;

  // Get iterator
  NIter it_vec = vec;

  while (it_vec != vec_end)
  {
    if (!boost::math::isnormal(*it_vec)) status = false;
    ++it_vec;
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate atan2 of both vectors and store in output
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAtan2
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(out)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(y)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(x)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter out, const NIter out_end,
                     const NIter y, const NIter y_end,
                     const NIter x, const NIter x_end)
{
  int N = std::distance(out, out_end);

  if (N != std::distance(y, y_end) || N != std::distance(x, x_end))
  {
    throw PyCR::ValueError("Vectors do not have the correct siye.");
  }

  // Get iterator
  NIter out_it = out;
  NIter y_it = y;
  NIter x_it = x;

  for (int i=0; i<N; i++)
  {
    *out_it++ = atan2(*y_it++, *x_it++);
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the signal to noise ratio of the maximum.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hMaxSNR
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(snr)()("Signal to noise ratio of maximum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(mean)()("Mean of noise window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(rms)()("RMS of noise window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(max)()("Maximum of signal window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HInteger)(maxpos)()("Position of maximum with respect to signal_start.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_6 (HInteger)(signal_start)()("Start of signal part where to look for maximum.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HInteger)(signal_end)()("End of signal part where to look for maximum.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Calculates the signal to noise by first finding the maximum in the specified (*signal_start*, *signal_end*) region, calculating the root mean
  square of the noise (defined to be everything outside the signal region) and dividing the two. In addition the position of the maximum is
  calculated relative to the *signal_start*.
*/
template <class IIter, class NIter>
void HFPP_FUNC_NAME (const NIter snr, const NIter snr_end,
                     const NIter mean, const NIter mean_end,
                     const NIter rms, const NIter rms_end,
                     const NIter max, const NIter max_end,
                     const IIter maxpos, const IIter maxpos_end,
                     const NIter vec, const NIter vec_end,
                     const HInteger signal_start, const HInteger signal_end)
{
  // Get vector length
  const HInteger n = std::distance(vec, vec_end);
  const HInteger nm = n - (signal_end - signal_start);

  // Initialize values
  *snr = 0;
  *mean = 0;
  *rms = 0;
  *max = 0;
  *maxpos = 0;

  // Get iterator
  NIter it = vec;

  // Sanity checks
  if (signal_start < 0)
  {
    char error_message[256];
    sprintf(error_message, "[hMaxSNR] signal_start[=%ld] < 0", signal_start);
    throw PyCR::ValueError(error_message);
  }

  if (signal_end < signal_start)
  {
    char error_message[256];
    sprintf(error_message, "[hMaxSNR] signal_end[=%ld] < signal_start[=%ld]", signal_end, signal_start);
    throw PyCR::ValueError(error_message);
  }

  if (signal_end > n)
  {
    char error_message[256];
    sprintf(error_message, "[hMaxSNR] signal_end[=%ld] > n=[%ld]", signal_end, n);
    throw PyCR::ValueError(error_message);
  }

  // Calculate the mean
  for (HInteger i=0; i<signal_start; i++)
  {
    *mean += *it / nm;
    it++;
  }

  it += (signal_end - signal_start);

  for (HInteger i=0; i<signal_start; i++)
  {
    *mean += *it / nm;
    it++;
  }

  // Noise before signal window
  it = vec;
  for (HInteger i=0; i<signal_start; i++)
  {
    *rms += ((*it - *mean) * (*it - *mean)) / nm;
    it++;
  }

  // Pulse window to find maximum in (should be excluded from RMS)
  *max = *it;
  for (HInteger i=signal_start; i<signal_end; i++)
  {
    if (*it > *max)
    {
      *max = *it;
      *maxpos = i - signal_start;
    }

    it++;
  }

  // Noise after signal window
  for (HInteger i=signal_end; i<n; i++)
  {
    *rms += ((*it - *mean) * (*it - *mean)) / nm;
    it++;
  }

  *rms = sqrt(*rms);
  *snr = (*max - *mean) / *rms;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate integrated pulse power
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hIntegratedPulsePower
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(power)()("Integrated pulse power.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(noise_power)()("Integrated noise power per sample.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(signal_start)()("Start of signal window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HInteger)(signal_end)()("End of signal window.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Calculates the integrated pulse power.
*/
template <class NIter, class Iter>
void HFPP_FUNC_NAME (const NIter power, const NIter power_end,
                     const NIter noise_power, const NIter noise_power_end,
                     const NIter vec, const NIter vec_end,
                     const Iter signal_start, const Iter signal_start_stub,
                     const Iter signal_end, const Iter signal_end_stub)
{
  // Get vector length
  const HInteger n = std::distance(vec, vec_end);
  const HInteger nm = n - (*signal_end - *signal_start);

  // Get iterator
  NIter it = vec;
  NIter it_power = power;
  NIter it_noise_power = noise_power;

  *it_power = 0;

  // Sanity checks
  if (*signal_start < 0)
  {
    char error_message[256];
    sprintf(error_message, "[hIntegratedPulsePower] *signal_start[=%ld] < 0", *signal_start);
    throw PyCR::ValueError(error_message);
  }

  if (*signal_end < *signal_start)
  {
    char error_message[256];
    sprintf(error_message, "[hIntegratedPulsePower] *signal_end[=%ld] < *signal_start[=%ld]", *signal_end, *signal_start);
    throw PyCR::ValueError(error_message);
  }

  if (*signal_end > n)
  {
    char error_message[256];
    sprintf(error_message, "[hIntegratedPulsePower] *signal_end[=%ld] > n=[%ld]", *signal_end, n);
    throw PyCR::ValueError(error_message);
  }

  // Power before signal window
  it = vec;
  for (HInteger i=0; i<*signal_start; i++)
  {
    *it_noise_power += (*it * *it);
    it++;
  }

  // Power in pulse window
  for (HInteger i=*signal_start; i<*signal_end; i++)
  {
    *power += (*it * *it);
    it++;
  }

  // Noise after signal window
  for (HInteger i=*signal_end; i<n; i++)
  {
    *it_noise_power += (*it * *it);
    it++;
  }

  *it_noise_power /= nm;

  *it_power -= *it_noise_power * static_cast<HNumber>(n - nm);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculate modulus of vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hModulus
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(m)()("Vector.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class NIter>
void HFPP_FUNC_NAME (const NIter vec, const NIter vec_end,
                     const HInteger m)
{
  // Get iterator
  NIter vec_it = vec;

  while (vec_it != vec_end)
  {
    *vec_it = *vec_it % m;
    vec_it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Test slicing
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hTestSlice
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vec0)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vec1)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(vec2)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class NIter, class Iter>
void HFPP_FUNC_NAME (const NIter vec0, const NIter vec0_end,
                     const NIter vec1, const NIter vec1_end,
                     const Iter vec2, const Iter vec2_end)
{
  // Get iterator
  NIter vec0_it = vec0;
  NIter vec1_it = vec1;

  std::cout<<*vec0_it<<" "<<*vec1_it<<" "<<*vec2<<std::endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

