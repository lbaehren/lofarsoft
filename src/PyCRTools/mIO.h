/**************************************************************************
 *  Header file of the IO module for the CR Pipeline Python bindings.     *
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

#ifndef CR_PIPELINE_IO_H
#define CR_PIPELINE_IO_H

#include "core.h"
#include "mArray.h"
#include "mImaging.h"
#include "mModule.h"

#include "IO/LopesEventIn.h"
#include "IO/LOFAR_TBB.h"
#include <Calibration/CalTableReader.h>

// ________________________________________________________________________
//                                                       AERA include files
#ifdef PYCRTOOLS_WITH_AERA

#include "aera/Datareader.h"
#include "aera/Data/Event.h"
#include "aera/Data/LocalStation.h"

#endif /* PYCRTOOLS_WITH_AERA */


// ========================================================================
//  Definitions
// ========================================================================

typedef CR::DataReader* DataReaderPointer; // Used in wrapper preprocessor
typedef CR::DataReader CRDataReader; // Used in wrapper preprocessor

template<class T> inline T hfcast(CR::DataReader v){return hfcast<T>((HInteger)((void*)&v));}
template<> inline HString hf2string(CR::DataReader v){return hf2string((HInteger)((void*)&v));}

//========================================================================
//                             I/O Functions
//========================================================================

HIntPointer hOpenFile(HString Filename);
void hCloseFile(HIntPointer iptr);

HString hgetFiletype(HString filename);
HString hgetFileExtension(HString filename);

void hFileSummary(CRDataReader & dr);
CRDataReader& hFileOpen(HString Filename);
HPyObject hFileGetParameter(CRDataReader &dr, HString key);
bool hFileSetParameter(CRDataReader &dr, HString key, HPyObjectPtr pyob);
HPyObjectPtr hCalTable(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob);
std::vector<HNumber> hCalTableVector(HString filename, HString keyword, HInteger date, HPyObjectPtr pyob);


// ========================================================================
//
//  AERA
//
// ========================================================================

#ifdef PYCRTOOLS_WITH_AERA

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



#endif /* PYCRTOOLS_WITH_AERA */

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "mIO.def.h"

#endif /* CR_PIPELINE_IO_H */

