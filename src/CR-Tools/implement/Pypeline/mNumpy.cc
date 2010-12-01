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

//$DOCSTRING: Copy input array to numpy array
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopyToNumpy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (ndarray)(out)()("Numpy vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  Copy to numpy array
*/
template <class Iter>
void HFPP_FUNC_NAME(ndarray out, const Iter in, const Iter in_end)
{
  Iter in_it = in;

  // Get pointers to memory of ndarray
  IterValueType* out_it = numpyBeginPtr<IterValueType>(out);
  IterValueType* out_end = numpyEndPtr<IterValueType>(out);

  // Copy and cast to correct type
  while (out_it != out_end && in_it != in_end)
  {
    *out_it = static_cast<IterValueType>(*in_it);

    ++out_it;
    ++in_it;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

