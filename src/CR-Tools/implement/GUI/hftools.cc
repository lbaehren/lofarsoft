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


inline HComplex operator*(HInteger i, HComplex c) {return hfcast<HComplex>(i)*c;}
inline HComplex operator*(HComplex c, HInteger i) {return hfcast<HComplex>(i)*c;}
inline HComplex operator+(HInteger i, HComplex c) {return hfcast<HComplex>(i)+c;}
inline HComplex operator+(HComplex c, HInteger i) {return hfcast<HComplex>(i)+c;}
inline HComplex operator-(HInteger i, HComplex c) {return hfcast<HComplex>(i)-c;}
inline HComplex operator-(HComplex c, HInteger i) {return c-hfcast<HComplex>(i);}
inline HComplex operator/(HInteger i, HComplex c) {return hfcast<HComplex>(i)/c;}
inline HComplex operator/(HComplex c, HInteger i) {return c/hfcast<HComplex>(i);}

//inline void hPowerAdd(HComplex & in, HNumber & inout){inout+=real(in*conj(in));}


//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(hNegate)("multiplies each element in the vector with -1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
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
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES 
#define HFPP_FUNCDEF  (HFPP_VOID)(hFill)("Fills a vector with a constant value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(fill_value)()("Fill value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE) 
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
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING1: Resize a vector to a new length.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES 
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param vec: Input vector
         input values.
  \param newsize: new size of vector
*/
template <class T> 
void hResize(std::vector<T> & vec, HInteger newsize)
{
  vec.resize(newsize);
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING1: Resize a vector to a new length and will new elements in vector with a specific value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_WRAPPER_CLASSES
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES 
#define HFPP_FUNCDEF  (HFPP_VOID)(hResize)("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE) 
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(fill)()("Value to fill new vector elements with")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param vec: Input vector
         input values.
  \param newsize: new size of vector

  \param: fill: Value to fill new vector elements with
*/
template <class T> 
void hResize(std::vector<T> & vec, HInteger newsize, T fill)
{
  vec.resize(newsize,fill);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvert
//-----------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("Fills a vector with a constant value.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief Fills a vector with a constant value.

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param out_start: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin, class Iter> 
void HFPP_FUNC_NAME(const Iterin data_start,const Iterin data_end, const Iter out_start,const Iter out_end)
{
  typedef IterValueType T;
  Iterin it=data_start;
  Iter itout=out_start;
  while (it!=data_end & itout !=out_end) {
    *itout=hfcast<T>(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//========================================================================
//$ITERATE MFUNC abs,cos,cosh,exp,log,log10,sin,sinh,sqrt,tan,tanh
//========================================================================

//$DOCSTRING1: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter> 
void h{$MFUNC!CAPS}1(const Iter data_start,const Iter data_end)
{
  Iter it=data_start;
  while (it!=data_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING2: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING2")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING2

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param out_start: STL Iterator pointing to the first element of an array with
         output values.
  \param out_end: STL Iterator pointing to the end of the output vector

*/
template <class Iter> 
void h{$MFUNC!CAPS}2(const Iter data_start,const Iter data_end, const Iter out_start,const Iter out_end)
{
  Iter it=data_start;
  Iter itout=out_start;
  while (it!=data_end & itout !=out_end) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE

//========================================================================
//$ITERATE MFUNC acos,asin,atan,ceil,floor 
//========================================================================

//$DOCSTRING1: Take the $MFUNC of all the elements in the vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 1
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param data_start: STL Iterator pointing to the first element of an array with
         input values.
  \param data_end: STL Iterator pointing to the end of the input vector

*/
template <class Iter> 
void h{$MFUNC!CAPS}1(const Iter data_start,const Iter data_end)
{
  Iter it=data_start;
  while (it!=data_end) {
    *it=$MFUNC(*it);
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING2: Take the $MFUNC of all the elements in the vector and return results in a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(h{$MFUNC!CAPS})("$DOCSTRING2")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecout)()("Numeric output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
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
template <class Iter1,class Iter2> 
void h{$MFUNC!CAPS}2(const Iter1 data_start,const Iter1 data_end, const Iter2 out_start,const Iter2 out_end)
{
  Iter1 it=data_start;
  Iter2 itout=out_start;
  while (it!=data_end & itout !=out_end) {
    *itout=$MFUNC(*it);
    ++it; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE


#define HFPP_OPERATOR_Mul *
#define HFPP_OPERATOR_INPLACE_Mul *=
#define HFPP_OPERATOR_Add +
#define HFPP_OPERATOR_INPLACE_Add +=
#define HFPP_OPERATOR_Div /
#define HFPP_OPERATOR_INPLACE_Div /=
#define HFPP_OPERATOR_Sub -
#define HFPP_OPERATOR_INPLACE_Sub -=

//========================================================================
//$ITERATE MFUNC Mul,Add,Div,Sub
//========================================================================


//$DOCSTRING1: Performs a $MFUNC between the two vectors, which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hi$MFUNC
//-----------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param data_start: STL Iterator pointing to the first element of an array with
         input and output values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param vec2_start: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector
*/
template <class Iter, class Iterin> 
void HFPP_FUNC_NAME(const Iter data_start,const Iter data_end, const Iterin vec2_start,const Iterin vec2_end)
{
  typedef IterValueType T;
  Iter it=data_start;
  Iterin itout=vec2_start;
  while (it!=data_end & itout !=vec2_end) {
    *it HFPP_OPERATOR_INPLACE_$MFUNC hfcast<T>(*itout);
    ++it; ++itout;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING1: Performs a $MFUNC between the vector and a scalar (applied to each element), which is returned in the first vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNC_NAME hi$MFUNC
//-----------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING1")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING1

  \param data_start: STL Iterator pointing to the first element of an array with
         input and output values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param val: Value containing the second operand
*/
template <class Iter, class S> 
void hi{$MFUNC}2(const Iter data_start,const Iter data_end, S val)
{
  typedef IterValueType T;
  Iter it=data_start;
  T val_t = hfcast<T>(val);
  while (it!=data_end) {
    *it HFPP_OPERATOR_INPLACE_$MFUNC val_t;
    ++it;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING2: Performs a $MFUNC between the two vectors, which is returned in the third vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING2")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Vector containing the second operands")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec3)()("Vector containing the results")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING2

  \param data_start: STL Iterator pointing to the first element of an array with
         input  values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param vec2_start: STL Iterator pointing to the first element of an array with
         input values.
  \param vec2_end: STL Iterator pointing to the end of the input vector

  \param vec3_start: STL Iterator pointing to the first element of an array with
         output values.
  \param vec3_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class Iterin2, class Iter> 
void HFPP_FUNC_NAME(const Iterin1 data_start,const Iterin1 data_end, const Iterin2 vec2_start,const Iterin2 vec2_end, const Iter vec3_start,const Iter vec3_end)
{
  typedef IterValueType T;
  Iterin1 it1=data_start;
  Iterin2 it2=vec2_start;
  Iter itout=vec3_start;
  while (it1!=data_end & it2 !=vec2_end & itout !=vec3_end) {
    *itout = hfcast<T>((*it1) HFPP_OPERATOR_$MFUNC  (*it2));
    ++it1; ++it2; ++itout;
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING2: Performs a $MFUNC between the vector and a scalar, where the result is returned in the second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_VARIANT 2
#define HFPP_FUNC_NAME h$MFUNC
//-----------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING2")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(val)()("Value containing the second operand")(HFPP_PAR_IS_SCALAR)(STDIT)(HFPP_PASS_AS_VALUE) 
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_3)(vec2)()("Vector containing the output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING2

  \param data_start: STL Iterator pointing to the first element of an array with
         input  values.
  \param data_end: STL Iterator pointing to the end of the input vector

  \param val parameter containing the second operand

  \param vec2_start: STL Iterator pointing to the first element of an array with
         output values.
  \param vec2_end: STL Iterator pointing to the end of the output vector
*/
template <class Iterin1, class S, class Iter> 
void h{$MFUNC}2(const Iterin1 data_start,const Iterin1 data_end, S val, const Iter vec2_start,const Iter vec2_end)
{
  typedef IterValueType T;
  Iterin1 itin=data_start;
  Iter itout=vec2_start;
  T val_t=hfcast<T>(val);
  while (itin!=data_end & itout !=vec2_end) {
    *itout = hfcast<T>(*itin) HFPP_OPERATOR_$MFUNC  val_t;
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$ENDITERATE



#include <GUI/hftools.hpp>
