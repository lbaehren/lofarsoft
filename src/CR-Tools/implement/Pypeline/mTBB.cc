/**************************************************************************
 *  IO module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Pim Schellart <p.schellart@astro.ru.nl>                               *
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
//$FILENAME: HFILE=mIO.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mTBB.h"


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
//$SECTION: IO functions
// ========================================================================

// =========================================================================
//
//  Construction
//
// =========================================================================

//__________________________________________________________________________
//                                                                   TBBData

//__________________________________________________________________________
//                                                                   TBBData

/*!
  \param filename -- Name of the file from which to read in the data
  \param blocksize -- Size of a block of data, [samples]
*/
TBBData::TBBData (std::string const &filename,
		uint const &blocksize)
  : DAL::TBB_Timeseries (filename)
{
  init ();
}

// =========================================================================
//
//  Destruction
//
// =========================================================================

//__________________________________________________________________________
//                                                                  ~TBBData

TBBData::~TBBData ()
{
  destroy();
}

//__________________________________________________________________________
//                                                                   destroy

void TBBData::destroy ()
{;}

// =========================================================================
//
//  Parameter access
//
// =========================================================================

// =========================================================================
//
//  Methods
//
// =========================================================================

//__________________________________________________________________________
//                                                                      init

bool TBBData::init ()
{
  return true;
}

