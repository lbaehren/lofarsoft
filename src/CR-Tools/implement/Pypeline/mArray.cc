/**************************************************************************
 *  Array module for CR Pipeline Python bindings.                         *
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
//  Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mArray.def.h
//-----------------------------------------------------------------------


// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mArray.h"


// ========================================================================
//
//  Implementation
//
// ========================================================================

#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE CC
//-----------------------

// ========================================================================
//$SECTION: Array functionality
// ========================================================================


// ========================================================================
//
//  Definitions
//
// ========================================================================


HBool hArray_trackHistory_value = false;


//========================================================================
//                 Casting & Conversion Functions
//========================================================================

void hArray_trackHistory(HBool on){
  hArray_trackHistory_value = on;
}

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



//$DOCSTRING: Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2))).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hVectorLength
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
  return PyCR::Array::hVectorLength(vec, vec_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Normalizes a vector to length unity.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNormalize
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
 $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  PyCR::Array::hNormalize (vec, vec_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


#undef HFPP_FILETYPE
