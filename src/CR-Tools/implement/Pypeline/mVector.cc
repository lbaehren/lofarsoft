/**************************************************************************
 *  Vector module for CR Pipeline Python bindings.                        *
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


// ========================================================================
//
//  Implementation
//
// ========================================================================

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: Vector functions
// ========================================================================


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
  hFill(vec,0) -> [0,0,0,...]
  vec.fill(0) -> [0,0,0,...]

  \brief $DOCSTRING
  $PARDOCSTRING

See also: hSet
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
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE_2)(fill_vec)()("Vector of values to fill it with")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  hFill(vec,[0,1,2]) -> [0,1,2,0,1,2,...]
  vec.fill([0,1,2]) -> [0,1,2,0,1,2,...]

  \brief $DOCSTRING
  $PARDOCSTRING

If fill_vec is shorther than vec, the procedure will wrap around and
start from the beginning of fill_vec again. Hence, in this case
fill_vec will appear repeated multiple times in vec.

*/
template <class Iter, class IterT>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const IterT fill_vec, const IterT fill_vec_end)
{
  Iter it1=vec;
  IterT it2=fill_vec;
  if (it2==fill_vec_end) return;
  while (it1!=vec_end) {
    *it1=hfcast<IterValueType>(*it2);
    ++it1;++it2;
    if (it2==fill_vec_end) it2=fill_vec;
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

See also: hFill, hCopy
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
  vec = [a_0,a_1,....,a_n-1,a_n]
  vec.flip() ->  [a_n,a_n-1,a_n-2,...,a_0]

  \brief $DOCSTRING
  $PARDOCSTRING

  The order of the elements in the vector will be reversed.
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
#define HFPP_PARDEF_1 (HInteger)(maxlen)()("Maximum length to output.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hPrettyString(vec,3) -> [vec_0,vec_1,vec_3,...]

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
HString HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger maxlen)
{
  HString s=("[");
  Iter it(vec), maxit;
  if (maxlen>=0)
    maxit=(vec+hfmin(maxlen,vec_end-vec));
  else
    maxit=vec_end;
  if (vec<vec_end)
    s+=hf2string(*it);
  ++it;
  while (it<maxit) {
    s+=","+hf2string(*it);
    ++it;
  };
  if ( it < vec_end ) s+=",...";
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
  hPrettyString(vec,3) -> '[vec_0,vec_1,vec_3,...]'

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const HInteger maxlen)
{
  cout << hPrettyString(vec,vec_end,maxlen) << endl;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Fills a vector with a series of values starting at a start value and then increasing by an increment until it is filled
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFillRange
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector to fill.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(start)()("Start value of the range.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HFPP_TEMPLATED_TYPE)(increment)()("Increment (to multiply loop variable with).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  hFillRange(vec,-2.5,2.) -> [-2.5,-0.5,1.5,3.5,...]
  vec.fillrange(start,increment) -> vec=[start, start+1*increment, start+2*increment ...]

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const  IterValueType start,  const IterValueType increment)
{
  Iter it=vec;
  HInteger i=0;
  while (it!=vec_end) {
    *it=start+increment*i;
    ++it; ++i;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Combines two vectors into one where the elements of the vectors follow each other alternating between the input vectors. Equivalent to python "zip".
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
  hZipper(vec,vecA,vecB) -> vec=[vecA_0,vecB_0,vecA_1,vecB_1,...,vecA_n,vecB_n]
  vec.zipper(vecA,vecB) -> vec=[vecA_0,vecB_0,vecA_1,vecB_1,...,vecA_n,vecB_n]

  \brief $DOCSTRING
  $PARDOCSTRING

  If the input vectors are shorther than vec they will wrap around and
  start from the beginning.

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
  vec1.convert(vec2) -> Copy vec1 to vec2

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class Iter2>
void HFPP_FUNC_NAME(const Iter vec1,const Iter vec1_end, const Iter2 vec2,const Iter2 vec2_end)
{
  typedef IterValueType T;
  Iter it1(vec1);
  Iter2 it2(vec2);
  while (it1!=vec1_end) {
    *it1=hfcast<T>(*it2);
    ++it1; ++it2;
    if (it2==vec2_end) it2=vec2;
  };
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
  vec1.convert(vec2) -> Copy vec1 to vec2

  \brief $DOCSTRING
  $PARDOCSTRING
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
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  vec1.copy(vec2) -> copy elements of vec2 to vec1

  If the input vector is shorter than the output vector, it will be
  copied mutliple times until the output vector is filled.  Use
  vec.resize first if you want to ensure that both vectors have the
  same size.

  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  PyCR::Vector::hCopy(vecout, vecout_end, vecin, vecin_end);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Copies all elements provided in an indexlist from one vector to another (no resizing is done!).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCopy
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vecout)()("Vector containing a copy of the input values")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(vecin)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(indexlist)()("Index list containing the positions of the elements to be copied over, (e.g. [0,2,4,...] will copy every second element).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(number_of_elements)()("Maximum number of elements to be copied, if this value is <0, then copying will be repeated until the output vector is filled.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  If the index vector is shorter than the output vector, the (indexed
  part of the) input vector will be copied multiple times until the
  output vector is filled.  Use vec.resize first if you want to ensure
  that both vectors have the same size.

  If number_of_elements is larger than zero and smaller than the
  length of the index vector, the remaining elements in the index
  vector are simply ignored.

See also: hFindGreaterThan, etc..
*/
template <class Iter, class Iterin, class IterI>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end, const IterI index, const IterI index_end, const HInteger number_of_elements)
{
  PyCR::Vector::hCopy(vecout,  vecout_end,  vecin,  vecin_end,  index,  index_end,  number_of_elements);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Redistributes the values in one vector sequentially into another vector, given an offset and stride (interval) - can be used for a transpose operation
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRedistribute
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(invec)()("Input Vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(offset)()("Offset from first element in output vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(stride)()("Stride (interval) between subsequent elements in the output Vector")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  vec=[0,1,2,3]
  vec.redistribute(invec,0,2) -> invec = [0,0,0,1,0,0,2,0,0,3,0,0]

  \brief $DOCSTRING
  $PARDOCSTRING

  Operation will stop whenever the end of one of the vectors is reached.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger offset, HInteger stride)
{
  PyCR::Vector::hRedistribute(vec,vec_end,invec,invec_end,offset,stride);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Transposes the values in one vector given a certain 2D shape and return result in a second vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hTranspose
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_1)(invec)()("Input Vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(ncolumns)()("Number of columns in input vector (i.e., width of a row, or the last and fastest running index)")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
Usage:
  invec=[0,1,2,3,4,5]; ncolumns=2
  hTranspose(outvec,invec,ncolumns) -> outvec = [0,2,4,1,3,5]

  \brief $DOCSTRING
  $PARDOCSTRING

  Operation will stop whenever the end of one of the input vector is reached.

  >>  x=hArray(range(6),[3,2])
  >>  y=hArray(int,[2,3],fill=-1)

  >>  x.mprint()
  [0,1]
  [2,3]
  [4,5]

  >>  hTranspose(y,x,2)
  >>  y.mprint()

  [0,2,4]
  [1,3,5]

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec,const Iter vec_end, const Iter invec,const Iter invec_end, HInteger ncolumns)
{
  PyCR::Vector::hTranspose(vec,vec_end,invec,invec_end,ncolumns);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

