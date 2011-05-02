/*****************************************************************************
 *                                                                           *
 *  Header file for the endpoint radiation module for the Python bindings    *
 *                                                                           *
 *  Copyright (c) 2011                                                       *
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

#ifndef CR_PIPELINE_ENDPOINTRADIATION_H
#define CR_PIPELINE_ENDPOINTRADIATION_H

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



// ========================================================================
//
//  Header definition
//
// ========================================================================

// ________________________________________________________________________
//                                                     Function definitions

// ________________________________________________________________________
//                                    Add declarations of wrapper functions

#undef HFPP_FILETYPE
// --------------------
#define HFPP_FILETYPE hFILE
// --------------------
#include "mEndPointRadiation.def.h"


#endif /* CR_PIPELINE_ENDPOINTRADIATION_H */

