/**************************************************************************
 *  Calibration module for the CR Pipeline Python bindings.               *
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

//$DOCSTRING: Calibrate polarization
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCalibratePolarization
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(pol0)()("Polarization 0.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(pol1)()("Polarization 1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(frequencies)()("Frequencies in Hz.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(az)()("Azimuth with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(el)()("Elevation with respect to antenna frame.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class CIter, class NIter>
void HFPP_FUNC_NAME (const CIter pol0, const CIter pol0_end,
    const CIter pol1, const CIter pol1_end,
    const NIter frequencies, const NIter frequencies_end,
    const HNumber az, const HNumber el)
{
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

