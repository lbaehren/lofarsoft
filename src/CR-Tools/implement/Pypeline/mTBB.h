/**************************************************************************
 *  Header file of the TBB module for the CR Pipeline Python bindings.    *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  P.Schellart@astro.ru.nl <p.schellart@astro.ru.nl>                     *
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

#ifndef CR_PIPELINE_TBB_H
#define CR_PIPELINE_TBB_H

#include "core.h"

/* DAL header files */
#include <dal/TBB_Timeseries.h>

class TBBData : private DAL::TBB_Timeseries {

public:
  
  // === Construction ======================================================

  //! Argumented constructor
  TBBData (std::string const &filename, uint const &blocksize=1);

  // === Destruction =======================================================

  //! Destructor
  ~TBBData ();

  // === Parameters access =================================================

  // === Methods ===========================================================

private:

  //! Initialize the internal settings
  bool init ();

  //! Unconditional deletion 
  void destroy(void);
};
  
// ________________________________________________________________________
//                                    Add declarations of wrapper functions

// Tell the wrapper preprocessor that this is a c++ header file
#undef HFPP_FILETYPE
//-----------------------
#define HFPP_FILETYPE hFILE
//-----------------------
#include "../../../../build/cr/implement/Pypeline/mTBB.def.h"

#endif /* CR_PIPELINE_TBB_H */

