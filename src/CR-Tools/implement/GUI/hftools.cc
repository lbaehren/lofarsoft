/*-------------------------------------------------------------------------*
 | $Id:: hftools.cc 3995 2010-01-18 10:09:09Z falcke                       $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Heino Falcke (www.astro.ru.nl/~falcke)                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*! 
  HFTOOLS Version 0.1 - Tools to manipulate vectors 
*/


using namespace std;

#include "hftools.h"

//Some definitions needed for the preprosessor programming:
//Copy source code (where specified with COPY_TO HFILE START) to the file hwrappers-hfanalysis.cc.h
//-----------------------------------------------------------------------
//$FILENAME: HFILE=hfwrappers-$SELF.h  
//-----------------------------------------------------------------------

// Tell the preprocessor (for generating wrappers) that this is a c++
// source code file (brackets are crucial)
#undef HFPP_FILETYPE
//-----------------------------------------------------------------------
#define HFPP_FILETYPE() (CC)  
//-----------------------------------------------------------------------

//$COPY_TO HFILE START --------------------------------------------------
#include "hfppnew-undef.h"
#define HFPP_FUNCDEF  (void)(hNegate)(1)("multiplies each element in the vector with -1")(HFPP_PAR_IS_SCALAR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#include "hfppnew-generatewrappers.h"
//$COPY_TO END --------------------------------------------------
/*!

  \brief Multiplies each element in the vector with
  -1 in place, i.e. the input vector is also the output vector

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector
*/

template <class Iter> 
void hNegate(const Iter data_start,const Iter data_end)
{
  typedef IterValueType T;
  Iter it=data_start;
  //  T fac = -1;
  while (it!=data_end) {
    *it=(*it) * -1;
    ++it;
  };
}



#include <GUI/hftools.hpp>
