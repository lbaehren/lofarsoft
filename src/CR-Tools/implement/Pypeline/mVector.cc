/**************************************************************************
 *  Vector module for CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *  Heino Falcke <h.falcke@astro.ru.nl>                                   *
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
//  Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mVector.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mVector.h"
#include <sstream>

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
//$SECTION: Vector functions
// ========================================================================

// ________________________________________________________________________
//                                        Vector to raw string for storrage

template HString hWriteRawVector(std::vector<HInteger> &vec);
template HString hWriteRawVector(std::vector<HNumber> &vec);
template HString hWriteRawVector(std::vector<HComplex> &vec);

template void hReadRawVector(std::vector<HInteger> &vec, HString raw);
template void hReadRawVector(std::vector<HNumber> &vec, HString raw);
template void hReadRawVector(std::vector<HComplex> &vec, HString raw);

//$DOCSTRING: Return the nth element of the vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hElem
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(N)()("Zero-based index of element in vector to return.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  ``hElem(vec,N) -> vec_N``

  Reference:

  hFirst, hLast, hElem

  Example:

  v=Vector(range(5))
  v.elem(3) -> 3
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, HInteger N)
{
  if (vec+N>=vec_end) throw PyCR::ValueError("hElem: requested element does not exist!");
  else return *(vec+N);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the first element of the vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFirst
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  ``hFirst(vec) -> vec_0``

  Reference:

  hFirst, hLast, hElem

  Example:

  v=Vector(range(5)) -> Vector(int, 5, fill=[0,1,2,3,4])
  v.first() -> 0
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  if (vec>=vec_end) throw PyCR::ValueError("hElem: requested element does not exist!");
  else return *(vec);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Return the last element of the vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLast
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  ``hLast(vec) -> vec[len(vec)-1]``

  Reference:

  hFirst, hLast, hElem

  Example:

  v=Vector(range(5)) -> Vector(int, 5, fill=[0,1,2,3,4])
  v.last() -> 4
*/
template <class Iter>
IterValueType HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  if (vec_end<=vec) throw PyCR::ValueError("hElem: requested element does not exist!");
  else return *(vec_end-1);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Fills a vector with a constant value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFill
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec)()("Vector to fill")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(fill_value)()("Fill value")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hFill(vec,0) -> [0,0,0,...]
  vec.fill(0) -> [0,0,0,...]

  See also:
  hSet
*/
template <class Iter, class T>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const T fill_value)
{
  Iter it=vec;
  IterValueType val_t=hfcast<IterValueType>(fill_value);
  while (it!=vec_end) {
    *it=val_t;
    ++it;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Fills a vector with the content of another vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFill
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec)()("Vector to fill")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(fillvec)()("Vector of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hFill(vec,[0,1,2]) -> [0,1,2,0,1,2,...]
  vec.fill([0,1,2]) -> [0,1,2,0,1,2,...]

  Description:
  If ``fillvec`` is shorther than ``vec``, the procedure will wrap
  around and start from the beginning of ``fillvec`` again. Hence, in
  this case ``fillvec`` will appear repeated multiple times in
  ``vec``.
*/
template <class Iter, class IterT>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const IterT fillvec, const IterT fillvec_end)
{
  if ((fillvec_end<=fillvec) || (vec_end<=vec)) return;
  Iter it1=vec;
  IterT it2=fillvec;
  while (it1!=vec_end) {
    *it1=hfcast<IterValueType>(*it2);
    ++it1;++it2;
    if (it2==fillvec_end) it2=fillvec;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Sets certain elements specified in an indexlist to a constant value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSet
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Vector of in which to set elements.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(indexlist)()("Index list containing the positions of the elements to be set, (e.g. [0,2,4,...] will set every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(val)()("Value to assign to the indexed elements.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec.set(indexlist,val) -> set elements in vec at the positions given in indexlist to value 'val'

  Example:
  vec=hArray(range(10))
  indx=hArray([2,4])
  vec -> hArray(int, [10], fill=[0, 1, 99, 3, 99, 5, 6, 7, 8, 9]) # len=10 slice=[0:10])

  See also:
  hFill, hCopy
*/
template <class Iter, class IterI, class T>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const IterI index, const IterI index_end, const T val)
{
  // Sanity check
  if (index >= index_end) return;

  // Variables
  Iter it;
  IterI itidx(index);
  IterValueType val_t=hfcast<IterValueType>(val);

  while (itidx != index_end) {
    it = vec + *itidx;
    if ((it < vec_end) && (it >= vec)) {
      *it=val_t;
    }
    ++itidx;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Will reverse the order of elements in a vector, such the data will run from right to left rather than left to right.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFlip
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input and output vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec = [a_0,a_1,....,a_n-1,a_n]
  vec.flip() ->  [a_n,a_n-1,a_n-2,...,a_0]

  Description:
  The order of the elements in the vector will be reversed. Same as ``hReverse``.

  See also:
  hReverse
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  Iter it1(vec),it2(vec_end-1);
  typename Iter::value_type tmp;
  while (it2 >= it1) {
    tmp=*it1;
    *it1=*it2;
    *it2=tmp;
    ++it1; --it2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Will reverse the order of elements in a vector, such the data will run from right to left rather than left to right.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hReverse
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input and output vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec = [a_0,a_1,....,a_n-1,a_n]
  vec.reverse() ->  [a_n,a_n-1,a_n-2,...,a_0]

  Description:
  The order of the elements in the vector will be reversed (same as :func:`hFlip`).

  See also:
  hFlip
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end)
{
  Iter it1(vec),it2(vec_end-1);
  typename Iter::value_type tmp;
  while (it2 >= it1) {
    tmp=*it1;
    *it1=*it2;
    *it2=tmp;
    ++it1; --it2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Returns the contents of a vector (up to a maximum length) as a pretty string for printing
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPrettyString
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HString)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(maxlen)()("Maximum length to output (all if negative).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hPrettyString(vec,start_slice,end_slice,maxlen) -> "[vec_0,vec_1,vec_3,...,vec_n-2,vec_n-1,vec_n]"

  Example:
  >>> a=hArray(range(10))
  >>> hPrettyString(a.vec(),8)
  '[0,1,2,3,...,6,7,8,9]'

  >>> hPrettyString(a.vec(),10)
  '[0,1,2,3,4,5,6,7,8,9]'

  >>> hPrettyString(a.vec(),12)
  '[0,1,2,3,4,5,6,7,8,9]'

  >>> hPrettyString(a.vec(),1)
  '[0,...,9]'

  >>> hPrettyString(a.vec(),0)
  '[0,...,9]'
*/
template <class Iter>
HString HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger maxlen)
{
  return hPrettyString(vec,vec_end,0,vec_end-vec,maxlen);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the contents of a slice of a vector (up to a maximum length) as a pretty string for printing
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPrettyString
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HString)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(start_slice)()("Start index.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(end_slice)()("End index).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(maxlen)()("Maximum length to output (all if negative).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hPrettyString(vec,start_slice,end_slice,maxlen) -> "[vec_0,vec_1,vec_3,...,vec_n-2,vec_n-1,vec_n]"

  Example:
  >>> a=hArray(range(10))
  >>> hPrettyString(a.vec(),0,10,8)
  '[0,1,2,3,...,6,7,8,9]'

  >>> hPrettyString(a.vec(),0,10,10)
  '[0,1,2,3,4,5,6,7,8,9]'

  >>> hPrettyString(a.vec(),0,10,12)
  '[0,1,2,3,4,5,6,7,8,9]'

  >>> hPrettyString(a.vec(),0,10,1)
  '[0,...,9]'

  >>> hPrettyString(a.vec(),0,10,0)
  '[0,...,9]'

  >>> hPrettyString(a.vec(),4,10,8)
  '[4,5,6,7,8,9]'

  >>> hPrettyString(a.vec(),4,10,4)
  '[4,5,...,8,9]'
*/
template <class Iter>
HString HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger start_slice, const HInteger end_slice, const HInteger maxlen)
{
  HString s("[");
  HInteger l=hfmax(maxlen/2,1);
  if (maxlen<0) l=(vec_end-vec+1)/2;

  Iter it, vec_start1, vec_end1, vec_start2, vec_end2;

  if (start_slice>=0) vec_start1=vec+start_slice;
  else vec_start1=vec_end+start_slice;

  if (end_slice>=0) vec_end2=vec+end_slice;
  else vec_end2=vec_end+end_slice;

  vec_end1=vec_start1+l;
  vec_start2=vec_end2-l;

  if (vec_start1<vec) vec_start1=vec;
  if (vec_start1>vec_end) vec_start1=vec_end;

  if (vec_end1<vec) vec_end1=vec;
  if (vec_end1>vec_end) vec_end1=vec_end;

  if (vec_start2<vec_end1) vec_start2=vec_end1;
  if (vec_start2>vec_end) vec_start2=vec_end;

  if (vec_end2<vec_end1) vec_end2=vec_end1;
  if (vec_end2>vec_end) vec_end2=vec_end;

  it=vec_start1;

  if (it<vec_end1) {
      s+=hf2string(*it);
      ++it;
    };

    while(it<vec_end1) {
      s+=","+hf2string(*it);
      ++it;
    };

    if (it<vec_start2) s+=",...";
    it=vec_start2;

    while (it<vec_end2) {
      s+=","+hf2string(*it);
      ++it;
    };

  s+="]";
  return s;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Prints the contents of a vector (up to a maximum length) as a pretty string
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPPrint
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(maxlen)()("Maximum length to output.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hPrettyString(vec,3) -> '[vec_0,vec_1,vec_3,...]'
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger maxlen)
{
  cout << hPrettyString(vec,vec_end,maxlen) << endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Prints the entire contents of a vector as a pretty string
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPPrint
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to output")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hPrettyString(vec) -> '[vec_0,vec_1,vec_3,...]'
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end)
{
  cout << hPrettyString(vec,vec_end,-1) << endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Fills a vector with a series of values starting at a start value and then increasing by an increment until it is filled
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFillRange
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Vector to fill.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(start)()("Start value of the range.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_2)(increment)()("Increment (to multiply loop variable with).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  hFillRange(vec,-2.5,2.) -> [-2.5,-0.5,1.5,3.5,...]
  vec.fillrange(start,increment) -> vec=[start, start+1*increment, start+2*increment ...]
*/
template <class Iter,class T>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const  T start,  const T increment)
{
  Iter it=vec;
  HInteger i=0;
  while (it!=vec_end) {
    *it=hfcast<IterValueType>(start+increment*i);
    ++it; ++i;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Combines two vectors into one where the elements of the vectors follow each other alternating between the input vectors. Equivalent to python 'zip'.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hZipper
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to fill")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vecA)()("Input vector #1 of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(vecB)()("Input vector #2 of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  If the input vectors are shorther than ``vec`` they will wrap around and
  start from the beginning.

  Usage:
  hZipper(vec,vecA,vecB) -> vec=[vecA_0,vecB_0,vecA_1,vecB_1,...,vecA_n,vecB_n]
  vec.zipper(vecA,vecB) -> vec=[vecA_0,vecB_0,vecA_1,vecB_1,...,vecA_n,vecB_n]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter vecA, const Iter vecA_end, const Iter vecB, const Iter vecB_end)
{
  Iter it=vec;
  Iter itA=vecA;
  Iter itB=vecB;
  if (itA>=vecA_end) return;
  if (itB>=vecB_end) return;
  while (it!=vec_end) {
    *it=*itA;
    ++it; if (it==vec_end) return;
    ++itA; if (itA==vecA_end) itA=vecA;
    *it=*itB;
    ++it; if (it==vec_end) return;
    ++itB; if (itB==vecB_end) itB=vecB;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Make and return a new vector of the same size and type as the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNew
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
std::vector<T> HFPP_FUNC_NAME(std::vector<T> & vec)
{
  std::vector<T> vnew(vec.size());
  return vnew;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Resize a vector to a new length.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size of vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
 */
template <class T>
void HFPP_FUNC_NAME (std::vector<T> & vec, HInteger newsize)
{
  vec.resize(newsize);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Resize a vector to a new length and fill new elements in vector with a specific value.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(newsize)()("New size")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(fill)()("Value to fill new vector elements with")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T>
void HFPP_FUNC_NAME (std::vector<T> & vec, HInteger newsize, T fill)
{
  vec.resize(newsize,fill);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Resize an STL vector to the same length as a second vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/

template <class T, class S>
void HFPP_FUNC_NAME (std::vector<T> & vec1,std::vector<S> & vec2)
{
  vec1.resize(vec2.size());
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Resize a casa vector to the same length as a second vector.
//$COPY_TO HFILE START --------------------------------------------------#
#define HFPP_FUNC_NAME hResize
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
//#define HFPP_FUNC_SLICED HFPP_FALSE
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE_1)(vec1)()("Input vector to be resized")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(vec2)()("Reference vector")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class T, class S>
void HFPP_FUNC_NAME (casa::Vector<T> & vec1,casa::Vector<S> & vec2)
{
  vec1.resize(*(vec2.shape().begin()));
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies and converts a vector to a vector of another type.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvert
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec1)()("Output vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec1.convert(vec2) -> Copy vec1 to vec2
*/
template <class Iter, class Iter2>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter2 vec2,const Iter2 vec2_end)
{
  typedef IterValueType T;
  Iter it1(vec1);
  Iter2 it2(vec2);
  HInteger lenIn = std::distance(vec2, vec2_end);
  HInteger lenOut = std::distance(vec1, vec1_end);

  if (lenOut <= lenIn) {
    // Fast method: without looping
    while (it1 != vec1_end) {
      *it1=hfcast<T>(*it2);
      ++it1; ++it2;
    }
  } else {
    // Slower method: with looping
    while (it1 != vec1_end) {
      *it1=hfcast<T>(*it2);
      ++it1; ++it2;
      if (it2==vec2_end)
        it2=vec2;
    }
  }

}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
//$DOCSTRING: Copies and converts a vector to a vector of another type and resizes the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hConvertResize
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES (STL) // since it does memory management, using resize
#define HFPP_PYTHON_WRAPPER_CLASSES (STL) // since it does memory management, using resize
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_CODE_PRE hResize(vecout,vecin); //code will be inserted in wrapper generation in cc file
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec1.convert(vec2) -> Copy vec1 to vec2
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  typedef IterValueType T;
  Iter itout(vecout);
  Iterin itin(vecin);
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout=hfcast<T>(*itin);
     ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Copies a vector to another one without resizing them.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  vec1.copy(vec2) -> copy elements of vec2 to vec1

  Description:
  If the input vector is shorter than the output vector, it will be
  copied mutliple times until the output vector is filled.  Use
  ``vec.resize`` first if you want to ensure that both vectors have the
  same size.
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  // Declaration of variables
  typedef IterValueType T;
  Iter itout(vecout);
  Iterin itin(vecin);
  HInteger lenIn  = std::distance(vecin, vecin_end);
  HInteger lenOut = std::distance(vecout, vecout_end);

  if (lenIn < lenOut) {
    // Fast copying (no looping required)
    while (itout != vecout_end) {
      *itout=hfcast<T>(*itin);
      ++itin;
      ++itout;
    }
  } else {
    // Coping with looping (including extra check for end of input vector)
    while (itout != vecout_end) {
      *itout=hfcast<T>(*itin);
      ++itin;
      ++itout;
      if (itin==vecin_end)
        itin=vecin;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Copies all elements provided in an indexlist from one vector to another (no resizing is done!).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(indexlist)()("Index list containing the positions of the elements to be copied over, (e.g. [0,2,4,...] will copy every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(number_of_elements)()("Maximum number of elements to be copied, if this value is <0, then copying will be repeated until the output vector is filled.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  If the index vector is shorter than the output vector, the (indexed
  part of the) input vector will be copied multiple times until the
  output vector is filled.  Use ``vec.resize`` first if you want to ensure
  that both vectors have the same size.
  If ``number_of_elements`` is larger than 0 and smaller than the
  length of the index vector, the remaining elements in the index
  vector are simply ignored.

  See also:
  hFindGreaterThan
*/
template <class Iter, class Iterin, class IterI>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end, const IterI index, const IterI index_end, const HInteger number_of_elements)
{
  // Declaration of variables
  typedef IterValueType T;
  Iter itout(vecout);
  IterI itidx(index);
  Iterin itin;
  HInteger count(number_of_elements);

  HInteger lenIn = std::distance(vecin, vecin_end);
  HInteger lenOut = std::distance(vecout, vecout_end);

  // Sanity check
  if ((index >= index_end) || (count==0))
    return;
  if (vecin >= vecin_end)
    return;
  if ((count<0) || (count > lenOut))
    count = lenOut;

  // Checking indices
  while (itidx != index_end) {
    if ((*itidx >= lenIn) || (*itidx < -lenIn)) {
      std::string errormessage;
      std::stringstream sstream (stringstream::in | stringstream::out);
      sstream << "Error: Illegal index value: " << *itidx;
      errormessage = sstream.str();
      throw PyCR::ValueError(errormessage);
    } else {
      if (*itidx < 0) {
        *itidx += lenIn;
      }
    }
    ++itidx;
  }
  itidx = index;

  // Copying of data
  while ((itout != vecout_end) && (count > 0)) {
    itin = vecin + *itidx;
    if ((itin >= vecin) && (itin < vecin_end))
      *itout=hfcast<T>(*itin);
    --count;
    ++itidx;
    ++itout;
    if (itidx==index_end)
      itidx=index;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Redistributes the values in one vector sequentially into another vector, given an offset and stride (interval) - can be used for a transpose operation
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRedistribute
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(invec)()("Input Vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(offset)()("Offset from first element in output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(stride)()("Stride (interval) between subsequent elements in the output Vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Operation will stop whenever the end of one of the vectors is reached.

  Usage:
  vec=[0,1,2,3]
  vec.redistribute(invec,0,2) -> invec = [0,0,0,1,0,0,2,0,0,3,0,0]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger offset, HInteger stride)
{
  // Variables
  Iter it(vec+offset);
  Iter itin(invec);

  // Sanity check
  if (stride < 0) {
    throw PyCR::ValueError("Negative value for stride parameter is not allowed");
  }
  // Redistribute
  while ((itin != invec_end) && (it < vec_end)) {
    *it = *itin;
    ++itin;
    it += stride;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Transposes the values in one vector given a certain 2D shape (lowest indices only)  and return result in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hTranspose
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(invec)()("Input Vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(ncolumns)()("Number of columns in input vector (i.e., width of a row, or the last and fastest running index)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  invec=[0,1,2,3,4,5]; ncolumns=2
  hTranspose(outvec,invec,ncolumns) -> outvec = [0,2,4,1,3,5]

  Description:
  Operation will stop whenever the end of one of the input vector is reached.

  Example:
  >>>  x=hArray(range(6),[3,2])
  >>>  y=hArray(int,[2,3],fill=-1)

  >>>  x.mprint()
  [0,1]
  [2,3]
  [4,5]

  >>>  hTranspose(y,x,2)
  >>>  y.mprint()
  [0,2,4]
  [1,3,5]
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger ncolumns)
{
  // Variables
  Iter it(invec),itend(it+ncolumns);
  HInteger nrows=(invec_end-invec)/ncolumns;
  HInteger i(0);
  while (itend<=invec_end) {
    hRedistribute(vec,vec_end,it,itend,i,nrows);
    ++i;
    it=itend;
    itend+=ncolumns;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Transposes the values in one vector given a certain 2D shape and return result in a second vector (can be applied to higher dimensions as well).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hTranspose
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(invec)()("Input Vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nrows)()("Number of rows in input vector (i.e., width of a column, or the slower running index)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(ncolumns)()("Number of columns in input vector (i.e., width of a row, or the faster running index)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  invec=[0,1,2,3,4,5]; ncolumns=2
  hTranspose(outvec,invec,ncolumns) -> outvec = [0,2,4,1,3,5]

  Description:
  Operation will stop whenever the end of one of the input vector is reached.

  Example:
  >>>  x=hArray(range(6),[3,2])
  >>>  y=hArray(int,[2,3],fill=-1)
  >>>  x.mprint()
  [0,1]
  [2,3]
  [4,5]

  >>>  hTranspose(y,x,2)
  >>>  y.mprint()
  [0,2,4]
  [1,3,5]
*/

template <class Iter>
void hTranspose(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger nrows, HInteger ncolumns)
{
  // Variables
  HInteger size=(invec_end-invec)/(ncolumns*nrows),block=ncolumns*size;
  Iter start(invec);
  Iter iti(start),itiend(iti+size);
  Iter ito(vec),itoend(ito+size);

  while (itoend<=vec_end) {
    hCopy(ito,itoend,iti,itiend);
    ito=itoend; itoend+=size;
    iti+=block;
    if (iti >= invec_end) {
      start+=size;
      iti=start;
    };
    itiend=iti+size;
  };

}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the lengths or norm of a vector (i.e. Sqrt(Sum_i(xi*+2))).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hVectorLength
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber hVectorLength (const Iter vec, const Iter vec_end)
{
  HNumber sum=0;
  Iter it=vec;
  while (it!=vec_end) {sum += (*it) * (*it); ++it;};
  return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns the absolute length of a vector between two ther vectors, i.e. Sqrt(Sum_i((x2_i-x1_i)^2)).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hVectorLength
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec2)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HNumber hVectorLength (const Iter vec1,const Iter vec1_end, const Iter vec2,const Iter vec2_end)
{
  HNumber sum=0;
  Iter it1(vec1),it2(vec2);

  if ((vec1_end-vec1)<0) ERROR_RETURN("Illegal size of input vector 'vec1'.");
  if ((vec2_end-vec2)<0) ERROR_RETURN("Illegal size of input vector 'vec2'.");

  while (it1!=vec1_end && it2!=vec2_end) {sum += pow(hfcast<HNumber>(*it2 - *it1),2); ++it1; ++it2;};
  return sqrt(sum);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Normalizes a vector to length unity.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNormalize
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
{
  HNumber norm=hVectorLength(vec,vec_end);
  Iter it=vec;
  while (it!=vec_end) {*it=(*it)/norm; ++it;};
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
