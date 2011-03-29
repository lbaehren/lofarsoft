/**************************************************************************
 *  Numpy module for CR Pipeline Python bindings.                          *
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
//$FILENAME: HFILE=mNumpy.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mArray.h"
#include "mNumpy.h"

#include "casa/BasicSL/Constants.h"

#include <iostream>

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
//$SECTION: Numpy functions
// ========================================================================

//$DOCSTRING: Copy input array to numpy ndarray
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (ndarray)(out)()("Numpy vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Copy to numpy ndarray.

  WARNING! Be careful with this function as it automatically typecasts to
  the type of the output array. In general you should always use matched
  types.

  If the input vector is shorter than the output vector, it will be
  copied mutliple times until the output vector is filled.
*/
template <class Iter>
void HFPP_FUNC_NAME(ndarray out, const Iter in_begin, const Iter in_end)
{
  Iter in_it = in_begin;

  // Get pointers to memory of numpy array
  IterValueType* out_it = numpyBeginPtr<IterValueType>(out);
  IterValueType* out_end = numpyEndPtr<IterValueType>(out);

  // Get size of numpy array
  const int N = num_util::size(out);

  // Check if looping over input vector is required to fill output vector
  if (N > in_end - in_begin)
  {
    // Copy and cast to correct type
    while (out_it != out_end)
    {
      *out_it = static_cast<IterValueType>(*in_it);

      ++out_it;
      ++in_it;

      if (in_it == in_end) in_it = in_begin;
    }
  }
  else // No looping
  {
    // Copy and cast to correct type
    while (out_it != out_end)
    {
      *out_it = static_cast<IterValueType>(*in_it);

      ++out_it;
      ++in_it;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Copy from input numpy ndarray to output array
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (ndarray)(out)()("Numpy vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Copy from numpy ndarray.

  WARNING! Be carefull with this function as it automatically typecasts to
  the type of the output array. In general you should always use matched
  types.

  If the input vector is shorter than the output vector, it will be
  copied mutliple times until the output vector is filled.
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter out, const Iter out_end, ndarray in)
{
  Iter out_it = out;

  // Get pointers to memory of numpy array
  IterValueType* in_begin = numpyBeginPtr<IterValueType>(in);
  IterValueType* in_end = numpyEndPtr<IterValueType>(in);
  IterValueType* in_it = in_begin;

  // Get size of numpy array
  const int N = num_util::size(in);

  // Check if looping over input vector is required to fill output vector
  if (out_end - out > N)
  {
    // Copy and cast to correct type
    while (out_it != out_end)
    {
      *out_it = static_cast<IterValueType>(*in_it);

      ++out_it;
      ++in_it;

      if (in_it == in_end) in_it = in_begin;
    }
  }
  else // No looping
  {
    // Copy and cast to correct type
    while (out_it != out_end)
    {
      *out_it = static_cast<IterValueType>(*in_it);

      ++out_it;
      ++in_it;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates the square of the absolute value and add to output vector
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

  Calculate absolute value squared for each value in the input vector
  and add result to output vector.
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
    *out_it = static_cast<double>(real(*in_it * conj(*in_it)));

    ++out_it;
    ++in_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

