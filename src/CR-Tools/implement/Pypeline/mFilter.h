/**************************************************************************
 *  Header file of the Filter module for the CR Pipeline Python bindings. *
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

#ifndef CR_PIPELINE_FILTER_H
#define CR_PIPELINE_FILTER_H

#include "core.h"
#include "mArray.h"
#include "mModule.h"

using namespace std;

// ========================================================================
//  Filter functions
// ========================================================================

namespace PyCR { // Namespace PyCR -- begin

  namespace Filter { // Namespace Filter -- begin



  } // Namespace Filter -- end

} // Namespace PyCR -- end


// ========================================================================
//  Wrapper declarations
// ========================================================================

#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mFilter.def.h"


#endif /* CR_PIPELINE_FILTER_H */

