/*****************************************************************************
 *                                                                           *
 *  Header file of the IO module for the Python bindings                     *
 *                                                                           *
 *  Copyright (c) 2010                                                       *
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

#ifndef CR_PIPELINE_IO_AERA_H
#define CR_PIPELINE_IO_AERA_H

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
#include "mArray.h"
#include "mModule.h"

// ________________________________________________________________________
//                                                      Other include files

#include "AERA/Datareader.h"
#include "AERA/Data/Event.h"
#include "AERA/Data/LocalStation.h"

// ________________________________________________________________________
//                                                         Type definitions

typedef AERA::Datareader AERADatareader; // Used in wrapper preprocessor

template<class T> inline T hfcast(AERA::Datareader v){return hfcast<T>((HInteger)((void*)&v));}
template<> inline HString hf2string(AERA::Datareader v){return hf2string((HInteger)((void*)&v));}

// ________________________________________________________________________
//                                                     Function definitions

AERA::Datareader& hAERAFileOpen (const HString filename);
bool hAERAFileClose (AERA::Datareader& dr);
HPyObject hAERAGetAttribute (AERA::Datareader& dr, const std::string key);
bool hAERAFirstEvent (AERA::Datareader& dr);
bool hAERAPrevEvent (AERA::Datareader& dr);
bool hAERANextEvent (AERA::Datareader& dr);
bool hAERALastEvent (AERA::Datareader& dr);
bool hAERAFirstLocalStation (AERA::Datareader& dr);
bool hAERAPrevLocalStation (AERA::Datareader& dr);
bool hAERANextLocalStation (AERA::Datareader& dr);
bool hAERALastLocalStation (AERA::Datareader& dr);
void hAERAFileSummary (AERA::Datareader& dr);
void hAERAEventSummary (AERA::Datareader& dr);
void hAERALocalStationSummary (AERA::Datareader& dr);


// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "mIO_AERA.def.h"

#endif /* CR_PIPELINE_IO_AERA_H */


