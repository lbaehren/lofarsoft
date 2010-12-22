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



// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mIO.def.h"

#endif /* CR_PIPELINE_IO_H */

