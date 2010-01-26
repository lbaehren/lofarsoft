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
  HFTOOLS Version 0.1 - Tools to manipulate vectors and generate
  wrappers for python
*/


using namespace std;

#undef HFPP_VERBOSE 
#include "hftools.h"

//Some definitions needed for the preprosessor programming:
//Copy source code (where specified with COPY_TO HFILE START) to the file hwrappers-hftools.iter.cc.h
//-----------------------------------------------------------------------
//$FILENAME: HFILE=hfwrappers-$SELF.h  
//-----------------------------------------------------------------------

// Tell the preprocessor (for generating wrappers) that this is a c++
// source code file (brackets are crucial)
#undef HFPP_FILETYPE
//-----------------------------------------------------------------------
#define HFPP_FILETYPE CC
//-----------------------------------------------------------------------


//Identity
template<class T> inline T hfcast(/*const*/ T v){return v;}

//Convert to arbitrary class T if not specified otherwise
template<class T> inline T hfcast(HInteger v){return static_cast<T>(v);}
template<class T> inline T hfcast(HNumber v){return static_cast<T>(v);}
template<class T> inline T hfcast(HComplex v){return static_cast<T>(v);}

//Convert Numbers to Numbers and loose information (round float, absolute of complex)
template<>  inline HInteger hfcast<HInteger>(HNumber v){return static_cast<HInteger>(floor(v+0.5));}
template<>  inline HInteger hfcast<HInteger>(HComplex v){return static_cast<HInteger>(floor(real(v)+0.5));}
template<>  inline HNumber hfcast<HNumber>(HComplex v){return real(v);}


inline HInteger ptr2int(HPointer v){return reinterpret_cast<HInteger>(v);}
inline HPointer int2ptr(HInteger v){return reinterpret_cast<HPointer>(v);}



//$COPY_TO HFILE START --------------------------------------------------
//#define HFPP_WRAPPER_TYPES (HInteger)(HNumber)
#define HFPP_FUNCDEF  (HFPP_VOID)(hNegate)(1)("multiplies each element in the vector with -1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#include "hfppnew-generatewrappers.def"
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
  Iter it=data_start;
  IterValueType fac = -1;
  while (it!=data_end) {
    *it=(*it) * fac;
    ++it;
  };
}


//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(hFill)(2)("Fills a vector with a constant value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(fill_value)()("Fill value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE) 
#include "hfppnew-generatewrappers.def"
//$COPY_TO END --------------------------------------------------
/*!

  \brief Fills a vector with a constant value.

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param fill_value: Value to fill vector with
*/
template <class Iter> 
void hFill(const Iter data_start,const Iter data_end, const IterValueType fill_value)
{
  Iter it=data_start;
  while (it!=data_end) {
    *it=fill_value;
    ++it;
  };
}


//$NOITERATE MFUNC acos,asin,atan,ceil,floor 
//atan, ceil, floor, acos, asin

//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,sin,sinh,sqrt,tan,tanh

//$DOCSTRING1: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(h$MFUNC!CAPS)(1)("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#include "hfppnew-generatewrappers.def"
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter> 
void h$MFUNC!CAPS(const Iter data_start,const Iter data_end)
{
  Iter it=data_start;
  while (it!=data_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}


//$DOCSTRING2: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_VARIANT 1
#define HFPP_FUNCDEF  (HFPP_VOID)(h$MFUNC!CAPS)(2)("$DOCSTRING2")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#include "hfppnew-generatewrappers.def"
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING2

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param data_start: STL Iterator pointing to the first element of an array with
         output values.
  \param data_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter> 
void h$MFUNC!CAPS(const Iter data_start,const Iter data_end, const Iter out_start,const Iter out_end)
{
  Iter it=data_start;
  Iter itout=out_start;
  while (it!=data_end & itout !=out_end) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$ENDITERATE

//$ITERATE MFUNC acos,asin,atan,ceil,floor 

//$DOCSTRING1: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h$MFUNC!CAPS)(1)("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#include "hfppnew-generatewrappers.def"
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter> 
void h$MFUNC!CAPS(const Iter data_start,const Iter data_end)
{
  Iter it=data_start;
  while (it!=data_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}


//$DOCSTRING2: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_VARIANT 1
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h$MFUNC!CAPS)(2)("$DOCSTRING2")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#include "hfppnew-generatewrappers.def"
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING2

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param data_start: STL Iterator pointing to the first element of an array with
         output values.
  \param data_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter> 
void h$MFUNC!CAPS(const Iter data_start,const Iter data_end, const Iter out_start,const Iter out_end)
{
  Iter it=data_start;
  Iter itout=out_start;
  while (it!=data_end & itout !=out_end) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$ENDITERATE


#include <GUI/hftools.hpp>
